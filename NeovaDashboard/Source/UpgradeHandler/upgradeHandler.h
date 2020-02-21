/*
 ==============================================================================
 
 This file was auto-generated!
 
 ==============================================================================
 */

#ifndef __UPGRADEHANDLER__
#define __UPGRADEHANDLER__

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "../Common/DashCommon.h"
#include "../DataReader/DashPipe.h"
#include "../Common/HubConfiguration.h"

#if JUCE_WINDOWS
#include <windows.h>
#include <stdio.h>
#include <tchar.h>

#endif //JUCE_WINDOWS

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
        //normal states
        checkingReleases = 0,
        waitingForUpgradeFirm = 1,
        upgradeFirmConnected = 2,
        upgradeInProgress = 3,
        upgradeSuccessfull = 4,

        //error states
        err_ringIsNotConnected = 10,
        err_waitingForUpgradeFirmTimeOut = 11,
        err_upgradeLaunchFailed = 12,
        err_upgradefailed = 13,
        err_unknow = 14
    };
    
    //==============================================================================
    UpgradeHandler(DashPipe& dashPipe, HubConfiguration& config);
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

    void setUpgradeState(uint16_t state);

    uint16_t getUpgradeState();

    //==============================================================================
    static VOID CALLBACK childProcessExitCallback(PVOID lpParameter, BOOLEAN TimerOrWaitFired);
     
    void createChildOutputHandlers();

    void launchNrfutil(UpgradeFirm FirmType, String portCOM);

    void closeNrfutil();

    void checkReleasesVersion();

    void launchUpgradeProcedure();

    void startRingUpgrade();

    void startHubUpgrade();

    //==============================================================================
private:
    HANDLE childOut = NULL;
    HANDLE childErr = NULL;
    PROCESS_INFORMATION pi;
    STARTUPINFO si;
    String childOutFileName = "nrfutilOut.log";
    String childErrFileName = "nrfutilErr.log";
    String nrfutilRelativePath = "\\Enhancia\\utilities\\nrfutil_5.1.0.exe";
    String releaseRelativePath = "\\Enhancia\\NeovaDashboard\\Releases\\";
    String logsRelativePath = "\\Enhancia\\NeovaDashboard\\Logs\\";
    String upgradeHubCommandLine = "nrfutil dfu usb-serial -pkg ";
    String upgradeRingCommandLine = "nrfutil dfu ble -ic NRF52 -n \"Neova DFU\" -pkg ";
    bool upgradeCommandReceived = false;
    uint16_t hubReleaseVersion = 0;
    uint16_t ringReleaseVersion = 0;
    uint16_t upgradeState = 0;
    //==============================================================================
    DashPipe& pipe;
    HubConfiguration& hubConfig;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (UpgradeHandler)
};


#endif // __STATUTPIPE__
