/*
 ==============================================================================
 
 This file was auto-generated!
 
 ==============================================================================
 */

#include "../JuceLibraryCode/JuceHeader.h"

#if JUCE_WINDOWS

#include "upgradeHandler_Win.h"
//==============================================================================
UpgradeHandler::UpgradeHandler(DashPipe& dashPipe, HubConfiguration& config, ApplicationCommandManager& manager)
	: dPipe (dashPipe), hubConfig(config), commandManager (manager) {}
UpgradeHandler::~UpgradeHandler() {}

//==============================================================================
void UpgradeHandler::timerCallback()
{
	
	set_upgradeCommandReceived(false);
	setUpgradeState(err_waitingForUpgradeFirmTimeOut);

	//TODO cacaARefaireCarBlockProcess 
	//AlertWindow::showMessageBox(AlertWindow::WarningIcon, "Erreur", "Device took too long time to respond please unplug the hub and retry", "Ok", nullptr);
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
}

int UpgradeHandler::getUpgradeState()
{
	return upgradeState;
}

//==============================================================================

VOID CALLBACK UpgradeHandler::childProcessExitCallback(PVOID lpParameter, BOOLEAN TimerOrWaitFired)
{
	UpgradeHandler* context = reinterpret_cast<UpgradeHandler*>(lpParameter);
	context->closeNrfutil();
}

void UpgradeHandler::createChildOutputHandlers()
{
    SECURITY_ATTRIBUTES sa;
    sa.nLength = sizeof(sa);
    sa.lpSecurityDescriptor = NULL;
    sa.bInheritHandle = TRUE;

	String out_f = File::getSpecialLocation(File::userApplicationDataDirectory).getFullPathName() + logsRelativePath + childOutFileName;
	String err_f = File::getSpecialLocation(File::userApplicationDataDirectory).getFullPathName() + logsRelativePath + childErrFileName;

    childOut = CreateFile(_T(out_f.getCharPointer()),
        FILE_GENERIC_READ | FILE_GENERIC_WRITE,
        FILE_SHARE_WRITE | FILE_SHARE_READ,
        &sa,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_HIDDEN,
        NULL);

    childErr= CreateFile(_T(err_f.getCharPointer()),
        FILE_GENERIC_READ | FILE_GENERIC_WRITE,
        FILE_SHARE_WRITE | FILE_SHARE_READ,
        &sa,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_HIDDEN,
        NULL);
}

void UpgradeHandler::launchNrfutil(UpgradeFirm FirmType, uint8_t * numCOM)
{
    stopTimer();

	//handlers on stdOut & stderr
	createChildOutputHandlers();
	
	uint8_t minor;
	uint8_t major;

	auto nrfutilPath = File::getSpecialLocation(File::globalApplicationsDirectory).getFullPathName() + nrfutilRelativePath;
	String commandLine;
    String portCOM = "COM" + String(*(uint8_t*)numCOM);
	
    if (FirmType == upgradeFirmRing)
	{
		minor = getRingReleaseVersion() & 0xFF;
		major = (getRingReleaseVersion()>>8) & 0xFF;
		
		auto releasePath = File::getSpecialLocation(File::userApplicationDataDirectory).getFullPathName() + releaseRelativePath + "ring_" + String(major) + "." + String(minor) + ".zip";
		commandLine = upgradeRingCommandLine + releasePath + " -p " + portCOM;
	}
	else if (FirmType == upgradeFirmHub)
	{
		minor = getHubReleaseVersion() & 0xFF;
		major = (getHubReleaseVersion() >> 8) & 0xFF;
		auto releasePath = File::getSpecialLocation(File::userApplicationDataDirectory).getFullPathName() + releaseRelativePath + "hub_" + String(major) + "." + String(minor) + ".zip";
		commandLine = upgradeHubCommandLine + releasePath + " -p " + portCOM;
	}

	ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));
	ZeroMemory(&si, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);
	si.dwFlags |= STARTF_USESTDHANDLES;
	si.hStdInput = NULL;
	si.hStdError = childErr;
	si.hStdOutput = childOut;


	// Start the child process. 
	if (!CreateProcess((LPCSTR)nrfutilPath.getCharPointer(),   // No module name (use command line)
		(LPSTR)commandLine.toStdString().c_str(),     // Command line (LPSTR)commandLine.toWideCharPointer()
		NULL,           // Process handle not inheritable
		NULL,           // Thread handle not inheritable
		TRUE,          // Set handle inheritance to FALSE
		CREATE_NO_WINDOW,              // No creation flags CREATE_NO_WINDOW
		NULL,           // Use parent's environment block
		NULL,           // Use parent's starting directory 
		&si,            // Pointer to STARTUPINFO structure
		&pi)           // Pointer to PROCESS_INFORMATION structure
		)
	{
		Logger::writeToLog("CreateProcess failed (%d).\n" + String(GetLastError()));
		setUpgradeState(err_upgradeLaunchFailed);
		return;
	}


	// Wait asynchronously until child process exits
	if (pi.hProcess)
	{
		setUpgradeState(upgradeInProgress);
		HANDLE hNewHandle;
		RegisterWaitForSingleObject(&hNewHandle, pi.hProcess, (WAITORTIMERCALLBACK)UpgradeHandler::childProcessExitCallback, reinterpret_cast<void*>(this), INFINITE, WT_EXECUTEONLYONCE);
	}
}

void UpgradeHandler::closeNrfutil()
{
    File out_f = File::getSpecialLocation(File::userApplicationDataDirectory).getFullPathName() + logsRelativePath + childOutFileName;
	File err_f = File::getSpecialLocation(File::userApplicationDataDirectory).getFullPathName() + logsRelativePath + childErrFileName;

	int ouf_size = out_f.loadFileAsString().length();
	int err_size = err_f.loadFileAsString().length();
	
	if (ouf_size > 0)
	{
		DBG(out_f.loadFileAsString());
		setUpgradeState(upgradeSuccessfull);
	}
	else if (err_size > 0)
	{
		DBG(err_f.loadFileAsString());
		setUpgradeState(err_upgradefailed);
    }
    else
    {
        DBG("Should not be there");
        setUpgradeState(err_unknow);
    }
    
	//Close process and thread handles.
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
    
    //TODO cacaARefaire
    /*
    if (getUpgradeState()>=0)
    {
        AlertWindow::showMessageBox(AlertWindow::InfoIcon, "Info", "Upgrade successfull", "Ok", nullptr);
    }
    else
    {
        AlertWindow::showMessageBox(AlertWindow::InfoIcon, "Info", "Upgrade failed", "Ok", nullptr);
    }*/
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
		//TODO cacaARefaireCarBlockProcess 
		//AlertWindow::showMessageBox(AlertWindow::InfoIcon, "Info", "Hub Already up to date", "Ok", nullptr);
	}
	else if (hubFiles.size() == 1)
	{
		String name = hubFiles.getFirst().getFileNameWithoutExtension();
		uint8_t minor = name.getTrailingIntValue();
		uint8_t major = name.trimCharactersAtEnd("." + String(minor)).getTrailingIntValue();
		uint16_t version = ((uint16_t)major << 8) | minor;
		setHubReleaseVersion(version);

		if (version <= hubConfig.getHubFirmwareVersionUint16())
		{
			//TODO cacaARefaireCarBlockProcess 
			//AlertWindow::showMessageBox(AlertWindow::InfoIcon, "Info", "Hub Already up to date", "Ok", nullptr);
		}
		else
		{
			//TODO cacaARefaireCarBlockProcess 
			//AlertWindow::showMessageBox(AlertWindow::InfoIcon, "Info", "Hub Update " + String(major) + "." + String(minor) + " available", "Ok", nullptr);
		}
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
		//TODO cacaARefaireCarBlockProcess 
		//AlertWindow::showMessageBox(AlertWindow::InfoIcon, "Info", "Ring Already up to date", "Ok", nullptr);
	}
	else if (ringFiles.size() == 1)
	{
		String name = ringFiles.getFirst().getFileNameWithoutExtension();
		uint8_t minor = name.getTrailingIntValue();
		uint8_t major = name.trimCharactersAtEnd("." + String(minor)).getTrailingIntValue();
		uint16_t version = ((uint16_t)major << 8) | minor;
		setRingReleaseVersion(version);

		if (version <= hubConfig.getRingFirmwareVersionUint16())
		{
			//TODO cacaARefaireCarBlockProcess
			//AlertWindow::showMessageBox(AlertWindow::InfoIcon, "Info", "Ring Already up to date", "Ok", nullptr);
		}
		else
		{
			//TODO cacaARefaireCarBlockProcess
			//AlertWindow::showMessageBox(AlertWindow::InfoIcon, "Info", "Ring Update " + String(major) + "." + String(minor) + " available", "Ok", nullptr);
		}
	}
	else
	{
		DBG("Should not be there");
		setUpgradeState(err_unknow);
	}
}


void UpgradeHandler::launchUpgradeProcedure()
{
	//checkVersion 
	checkReleasesVersion();
	//TODO Alex
	//sendCommand to open versionUpgradeWindow => versionUpgradeWindow will call startRingUpgrade() || startHubUpgrade()
	commandManager.invokeDirectly (neova_dash::commands::openFirmUpgradePanel, true);

	//startHubUpgrade();
}


void UpgradeHandler::startRingUpgrade()
{
	//check if ring is connected
	if (!hubConfig.getRingIsConnected())
	{
		//TODO cacaARefaireCarBlockProcess 
		//AlertWindow::showMessageBox(AlertWindow::WarningIcon, "Erreur", "Ring is not connected", "Ok", nullptr);
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
//==============================================================================
#endif //JUCE_WINDOWS
