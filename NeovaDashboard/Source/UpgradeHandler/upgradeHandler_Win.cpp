/*
 ==============================================================================
 
 This file was auto-generated!
 
 ==============================================================================
 */

#include "../JuceLibraryCode/JuceHeader.h"

#if JUCE_WINDOWS

#include "upgradeHandler_Win.h"
//==============================================================================
UpgradeHandler::UpgradeHandler(DashPipe& dashPipe, HubConfiguration& config, ApplicationCommandManager& manager, DataReader& dataReaderRef)
	: dPipe (dashPipe), hubConfig(config), commandManager (manager), dataReader(dataReaderRef)

{
	checkReleasesVersion();
}

UpgradeHandler::~UpgradeHandler() {}

//==============================================================================
void UpgradeHandler::timerCallback()
{
	set_upgradeCommandReceived(false);
	setUpgradeState(err_waitingForUpgradeFirmTimeOut);
	neova_dash::log::writeToLog("Failed upgrade: timeout..", neova_dash::log::hubCommunication);

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

bool UpgradeHandler::waitsForSuccessiveUpgrade()
{
    return successiveUpgrade;
}

bool UpgradeHandler::isUpgrading()
{
	return (upgradeState > 0 && upgradeState != upgradeSuccessfull)|| successiveUpgrade;
}

//==============================================================================

VOID CALLBACK UpgradeHandler::childProcessExitCallback(PVOID lpParameter, BOOLEAN)
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

    childOut = CreateFileW((LPCWSTR)out_f.toWideCharPointer(),
        FILE_GENERIC_READ | FILE_GENERIC_WRITE,
        FILE_SHARE_WRITE | FILE_SHARE_READ,
        &sa,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_HIDDEN,
        NULL);

    childErr= CreateFileW((LPCWSTR)err_f.toWideCharPointer(),
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
		
		auto releasePath = "\"" + File::getSpecialLocation(File::userApplicationDataDirectory).getFullPathName() + "\"" + releaseRelativePath + "ring_" + String(major) + "." + String(minor) + ".zip";
		commandLine = upgradeRingCommandLine + releasePath + " -p " + portCOM;
	}
	else if (FirmType == upgradeFirmHub)
	{
		minor = getHubReleaseVersion() & 0xFF;
		major = (getHubReleaseVersion() >> 8) & 0xFF;
		auto releasePath = "\"" + File::getSpecialLocation(File::userApplicationDataDirectory).getFullPathName() + "\"" + releaseRelativePath + "hub_" + String(major) + "." + String(minor) + ".zip";
		commandLine = upgradeHubCommandLine + releasePath + " -p " + portCOM;
	}

	ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));
	ZeroMemory(&si, sizeof(STARTUPINFOW));

	si.cb = sizeof(STARTUPINFOW);
	si.dwFlags |= STARTF_USESTDHANDLES;
	si.hStdInput = NULL;
	si.hStdError = childErr;
	si.hStdOutput = childOut;
	
	// Start the child process. 
	if (!CreateProcessW((LPCWSTR)nrfutilPath.toWideCharPointer(),	// No module name (use command line)
		(LPWSTR)commandLine.toWideCharPointer(),	// Command line (LPSTR)commandLine.toWideCharPointer()
		NULL,	// Process handle not inheritable
		NULL,	// Thread handle not inheritable
		TRUE,	// Set handle inheritance to FALSE	
		CREATE_NO_WINDOW,	// No creation flags CREATE_NO_WINDOW
		NULL,	// Use parent's environment block
		NULL,	// Use parent's starting directory 
		&si,	// Pointer to STARTUPINFO structure
		&pi)	// Pointer to PROCESS_INFORMATION structure
		)
	{
		neova_dash::log::writeToLog("Failed to launch upgrade process..", neova_dash::log::hubCommunication);
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
	File out_f = (LPCWSTR)(File::getSpecialLocation(File::userApplicationDataDirectory).getFullPathName() + logsRelativePath + childOutFileName).toWideCharPointer();
	File err_f = (LPCWSTR)(File::getSpecialLocation(File::userApplicationDataDirectory).getFullPathName() + logsRelativePath + childErrFileName).toWideCharPointer();

	
	int ouf_size = out_f.loadFileAsString().length();
	int err_size = err_f.loadFileAsString().length();
	
	if (ouf_size > 0)
	{
		DBG(out_f.loadFileAsString());
		setUpgradeState(upgradeSuccessfull);
        neova_dash::log::writeToLog("Neova Upgraded succesfully !", neova_dash::log::hubCommunication);

	}
	else if (err_size > 0)
	{
		DBG(err_f.loadFileAsString());
		setUpgradeState(err_upgradefailed);
		neova_dash::log::writeToLog("Failed to upgrade Neova..", neova_dash::log::hubCommunication);
    }
    else
    {
        DBG("Should not be there");
        setUpgradeState(err_unknow);
    }
    
	//Close process and thread handles.
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
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
	}
	else if (hubFiles.size() == 1)
	{
		String name = hubFiles.getFirst().getFileNameWithoutExtension();
		uint8_t minor = (uint8_t)name.getTrailingIntValue();
		uint8_t major = (uint8_t)name.upToLastOccurrenceOf(".", false, true).getTrailingIntValue();
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
		uint8_t minor = (uint8_t)name.getTrailingIntValue();
		uint8_t major = (uint8_t)name.upToLastOccurrenceOf(".", false, true).getTrailingIntValue();
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
		else 			  startRingUpgrade();
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
        Logger::writeToLog("CreateProcess failed (%d).\n" + String(GetLastError()));

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

/**
 * @brief check battery level before upgrade
 * @return true if battery level is greater than 20%.
*/
bool UpgradeHandler::checkBatteryForUpgrade () const
{
	const auto percentBattery = dataReader.getBatteryLevel(false);

	if(percentBattery >= 0.2f)
		return true;
	else
		return false;
}

//==============================================================================
#endif //JUCE_WINDOWS
