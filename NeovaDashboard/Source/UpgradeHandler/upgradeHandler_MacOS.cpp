/*
 ==============================================================================
 
 This file was auto-generated!
 
 ==============================================================================
 */

#include "../JuceLibraryCode/JuceHeader.h"

#if JUCE_MAC

#include "upgradeHandler_MacOS.h"

UpgradeHandler * UpgradeHandler::instanceUp = nullptr;

//==============================================================================
UpgradeHandler::UpgradeHandler(DashPipe& dashPipe, HubConfiguration& config, ApplicationCommandManager& manager)
    : dPipe (dashPipe), hubConfig(config), commandManager (manager)
{
    instanceUp = this;
}
UpgradeHandler::~UpgradeHandler() {}

//==============================================================================
void UpgradeHandler::timerCallback()
{
	set_upgradeCommandReceived(false);
	setUpgradeState(err_waitingForUpgradeFirmTimeOut);
	stopTimer();
}

//==============================================================================
void UpgradeHandler::set_upgradeCommandReceived(bool st)
{
	upgradeCommandReceived = st;
}

bool UpgradeHandler::get_upgradeCommandReceived()
{
	return upgradeCommandReceived;
}

void UpgradeHandler::setHubReleaseVersion(uint16_t version)
{
	hubReleaseVersion = version;
}

uint16_t UpgradeHandler::getHubReleaseVersion()
{
	return hubReleaseVersion;
}

void UpgradeHandler::setRingReleaseVersion(uint16_t version)
{
	ringReleaseVersion = version;
}

uint16_t UpgradeHandler::getRingReleaseVersion()
{
	return ringReleaseVersion;
}

void UpgradeHandler::setUpgradeState(int state)
{
	upgradeState = state;

    if (upgradeState < 0) successiveUpgrade = false; // Stops the HUB from updating upon restart in case of a successive upgrade
}

int UpgradeHandler::getUpgradeState()
{
	return upgradeState;
}

//==============================================================================
void UpgradeHandler::childProcessExitCallback(int signal)
{
    if(instanceUp!=nullptr)
    {
        instanceUp->closeNrfutil();
    }
}

void UpgradeHandler::launchNrfutil(UpgradeFirm FirmType, uint8_t * portCOM)
{
    stopTimer();
    
    String portPath = String(CharPointer_UTF8((char *)(portCOM+1)), *(uint8_t *) portCOM);
    
    auto nrfutilPath = File::getSpecialLocation(File::commonApplicationDataDirectory).getFullPathName() + nrfutilRelativePath;

    uint8_t minor;
    uint8_t major;
    String releasePath;
    char const * arr[5+7] = {nrfutilPath.getCharPointer()};
    int commandLineSize=0;
    
    //Recupère les path des fichiers release et la ligne de commande nrfutil en fonction du firm à upgrade
    if (FirmType == upgradeFirmRing)
    {
        minor = getRingReleaseVersion() & 0xFF;
        major = (getRingReleaseVersion()>>8) & 0xFF;
        
        releasePath = File::getSpecialLocation(File::userApplicationDataDirectory).getFullPathName() + releaseRelativePath + "ring_" + String(major) + "." + String(minor) + ".zip";
    
        std::copy(upgradeRingCommandLine,upgradeRingCommandLine + 7, arr+1);
        commandLineSize = 7;
    }
    else if (FirmType == upgradeFirmHub)
    {
        minor = getHubReleaseVersion() & 0xFF;
        major = (getHubReleaseVersion() >> 8) & 0xFF;
        
        releasePath = File::getSpecialLocation(File::userApplicationDataDirectory).getFullPathName() + releaseRelativePath + "hub_" + String(major) + "." + String(minor) + ".zip";
    
        std::copy(upgradeHubCommandLine,upgradeHubCommandLine + 3, arr+1);
        commandLineSize = 3;
    }
    char const * arrEnd[] = {releasePath.getCharPointer(), "-p", portPath.getCharPointer(), 0};
    std::copy(arrEnd,arrEnd + 4, arr + 1 + commandLineSize);
    
    
    pipe(out_pipe); //create a pipe to get stdout & stderr of the child
    pipe(err_pipe);
    
    //Launch nrfutil as a child process & set a callback to monitor its status
    signal(SIGCHLD, childProcessExitCallback);
    if(!(childPid = fork())) //spawn child
    {
        // Child. Close the read end of the pipe
        close(out_pipe[0]);
        close(err_pipe[0]);
        // redirect stdout and stderr to the write end of the pipe
        dup2(out_pipe[1], STDOUT_FILENO);
        dup2(err_pipe[1], STDERR_FILENO);
        execv(arr[0], (char **)arr); //child will terminate here, replaced by nrfutil
    }
    //Only parent gets here. Close write end of the pipe
    close(out_pipe[1]);
    close(err_pipe[1]);
    
    setUpgradeState(upgradeInProgress);
}

void UpgradeHandler::closeNrfutil()
{
    int status = 0, return_pid = 0, exit_status = 0;
    struct stat out_stat, err_stat;
    int out_length = 0, err_length = 0;
    
    return_pid = waitpid(childPid, &status, 0);

    if (WIFSIGNALED(status))
    {
        //exit forcé
        DBG("Child exited via signal " + String(WTERMSIG(status)));
        setUpgradeState(err_unknow);
    }
    if (!WIFEXITED(status))
    {
        //exit forcé
        DBG("Child exited abnormally");
        setUpgradeState(err_unknow);
    }
    else
    {
        //exit normal en fin de programme
        exit_status = WEXITSTATUS(status);
        if (exit_status==0)
        {
            setUpgradeState(upgradeSuccessfull);
        }
        else
        {
            setUpgradeState(err_upgradefailed);
        }
    }

    //Get child stdout & stderr content
    fstat(out_pipe[0], &out_stat);
    fstat(err_pipe[0], &err_stat);

    out_length = (int) out_stat.st_size;
    err_length = (int) err_stat.st_size;

    char out_data[2048];
    char err_data[2048];
    
    read(out_pipe[0], out_data, out_length);
    read(err_pipe[0], err_data, err_length);

    if (out_length>0)
    {
        DBG(String(CharPointer_UTF8(out_data)));
    }
    else if (err_length>0)
    {
        DBG(String(CharPointer_UTF8(err_data)));
    }
    else
    {
        DBG("Should not be there");
        setUpgradeState(err_unknow);
    }
    
    DBG("exit status : " + String(exit_status) +"\nout characters : " + String(out_length) + "\nerr characters : " + String(err_length));

    //Creer & save logs
    File err_f = File::getSpecialLocation(File::userApplicationDataDirectory).getFullPathName() + logsRelativePath + childErrFileName;
    File out_f = File::getSpecialLocation(File::userApplicationDataDirectory).getFullPathName() + logsRelativePath + childOutFileName;
    
    err_f.replaceWithText(String(CharPointer_UTF8(err_data)),false,false,nullptr);
    out_f.replaceWithText(String(CharPointer_UTF8(out_data)),false,false,nullptr);
}

void UpgradeHandler::checkReleasesVersion()
{
    setUpgradeState(checkingReleases);
    
    auto releasePath = File::getSpecialLocation(File::userApplicationDataDirectory).getFullPathName() + releaseRelativePath;
    File f(releasePath);
    
    Array<File> hubFiles = f.findChildFiles(3, false, "hub*");
    
    if (hubFiles.size() == 0)
    {
        setHubReleaseVersion(0);
        //TODO cacaARefaire
        //AlertWindow::showMessageBox(AlertWindow::InfoIcon, "Info", "Hub Already up to date", "Ok", nullptr);
    }
    else if (hubFiles.size() == 1)
    {
        String name = hubFiles.getFirst().getFileNameWithoutExtension();
        uint8_t minor = name.getTrailingIntValue();
        uint8_t major = name.trimCharactersAtEnd("." + String(minor)).getTrailingIntValue();
        uint16_t version = ((uint16_t)major << 8) | minor;
        setHubReleaseVersion(version);
    }
    else
    {
        DBG("Should not be there");
        setUpgradeState(err_unknow);
    }

    Array<File> ringFiles = f.findChildFiles(3, false, "ring*");
    if (ringFiles.size() == 0)
    {
        setRingReleaseVersion(0);
    }
    else if (ringFiles.size() == 1)
    {
        String name = ringFiles.getFirst().getFileNameWithoutExtension();
        uint8_t minor = name.getTrailingIntValue();
        uint8_t major = name.trimCharactersAtEnd("." + String(minor)).getTrailingIntValue();
        uint16_t version = ((uint16_t)major << 8) | minor;
        setRingReleaseVersion(version);
    }
    else
    {
        DBG("Should not be there");
        setUpgradeState(err_unknow);
    }
}

void UpgradeHandler::launchUpgradeProcedure()
{
    checkReleasesVersion();
    
    commandManager.invokeDirectly (neova_dash::commands::openFirmUpgradePanel, true);
}

void UpgradeHandler::startUpgrade()
{
    bool hubAvailable = getHubReleaseVersion() > hubConfig.getHubFirmwareVersionUint16();
    bool ringAvailable = (getRingReleaseVersion() > hubConfig.getRingFirmwareVersionUint16()) && hubConfig.getRingIsConnected();

    if (hubAvailable != ringAvailable)
    {
        if (hubAvailable) startHubUpgrade();
        else              startRingUpgrade();
    }
    else if (hubAvailable) // Both need to be upgraded
    {
        successiveUpgrade = true;
        startRingUpgrade();
    }
}

void UpgradeHandler::startRingUpgrade()
{
	//check if ring is connected
	if (!hubConfig.getRingIsConnected())
	{
		setUpgradeState(err_ringIsNotConnected);
	}
	else
	{
		//ask Hub for changing Mode (connectivity firm) 
		uint8_t data[4];
		memcpy(data, "ring", sizeof("ring"));
		dPipe.sendString(data, 4);
		//start Timer to track Timeout
		startTimer(60000);
		//set upgradeCommandReceived. Main will check it when connectivity firm will appear before calling launchNrfutil
		set_upgradeCommandReceived(true);
		setUpgradeState(waitingForUpgradeFirm);
	}
}

void UpgradeHandler::startHubUpgrade()
{
	//ask Hub for changing Mode (bootloader)
	uint8_t data[4];
	memcpy(data, "upgr", sizeof("upgr"));
	dPipe.sendString(data, 4);
	//start Timer to track Timeout
	startTimer(60000);
	//set upgradeCommandReceived. Main will check it when bootloader firm will appear before calling launchNrfutil
	set_upgradeCommandReceived(true);
	setUpgradeState(waitingForUpgradeFirm);
}

void UpgradeHandler::checkForSuccessiveUpgrade()
{
    if (upgradeState == upgradeSuccessfull && successiveUpgrade)
    {
        successiveUpgrade = false;
        startHubUpgrade();
    }
}
//==============================================================================
#endif //JUCE_MAC
