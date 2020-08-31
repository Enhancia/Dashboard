/*
 ==============================================================================
 
 This file was auto-generated!
 
 ==============================================================================
 */



#ifndef __UPGRADEHANDLER__
#define __UPGRADEHANDLER__

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#if JUCE_WINDOWS

#include "../Common/DashCommon.h"
#include "../DataReader/dashPipe.h"
#include "../Common/HubConfiguration.h"

#include <windows.h>
#include <stdio.h>
#include <tchar.h>

//==============================================================================

class UpgradeHandler : public Component, public Timer
{
public:
    enum UpgradeFirm
    {
        upgradeFirmHub = 0,
        upgradeFirmRing = 1,
        err_two_hub = 10
    };

    enum UpgradeState
    {
        //error states
        err_ringIsNotConnected = -5,
        err_waitingForUpgradeFirmTimeOut = -4,
        err_upgradeLaunchFailed = -3,
        err_upgradefailed = -2,
        err_unknow = -1,
        
        //normal states
        checkingReleases = 0,
        waitingForUpgradeFirm = 1,
        upgradeFirmConnected = 2,
        upgradeInProgress = 3,
        upgradeSuccessfull = 4
    };
    
    //==============================================================================
    UpgradeHandler(DashPipe& dashPipe, HubConfiguration& config, ApplicationCommandManager& cmdManager);
    ~UpgradeHandler();
    //==============================================================================
    void timerCallback() override;

    //==============================================================================
    void set_upgradeCommandReceived(bool st);

    bool get_upgradeCommandReceived();

    void setHubReleaseVersion(uint16_t version);

    uint16_t getHubReleaseVersion();

    void setRingReleaseVersion(uint16_t version);

    uint16_t getRingReleaseVersion();

    void setUpgradeState(int state);

    int getUpgradeState();

    bool waitsForSuccessiveUpgrade();
    bool isUpgrading();

    //==============================================================================
    static VOID CALLBACK childProcessExitCallback(PVOID lpParameter, BOOLEAN TimerOrWaitFired);
    
    void createChildOutputHandlers();

    void launchNrfutil(UpgradeFirm FirmType, uint8_t * numCOM);

    void closeNrfutil();
    
    void checkReleasesVersion();

    void launchUpgradeProcedure();
    
    void startUpgrade();
    void startRingUpgrade();
    void startHubUpgrade();

    void checkForSuccessiveUpgrade();
    //==============================================================================
private:
    HANDLE childOut = NULL;
    HANDLE childErr = NULL;
    PROCESS_INFORMATION pi;
    STARTUPINFO si;
    String childOutFileName = "nrfutilOut.log";
    String childErrFileName = "nrfutilErr.log";
    String nrfutilRelativePath = "\\Enhancia\\utilities\\nrfutil.exe";
    String releaseRelativePath = "\\Enhancia\\NeovaDashboard\\Releases\\";
    String logsRelativePath = "\\Enhancia\\NeovaDashboard\\Logs\\";
    String upgradeHubCommandLine = "nrfutil dfu usb-serial -pkg ";
    String upgradeRingCommandLine = "nrfutil dfu ble -ic NRF52 -n \"Neova DFU\" -pkg ";
    bool upgradeCommandReceived = false;
    uint16_t hubReleaseVersion = 0;
    uint16_t ringReleaseVersion = 0;
    int upgradeState = 0;
    bool successiveUpgrade = false;
    //==============================================================================
    DashPipe& dPipe;
    HubConfiguration& hubConfig;
    ApplicationCommandManager& commandManager;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (UpgradeHandler)
};


#endif // __STATUTPIPE__
#endif //JUCE_WINDOWS
