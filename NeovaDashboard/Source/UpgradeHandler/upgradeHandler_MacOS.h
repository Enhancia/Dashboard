/*
 ==============================================================================
 
 This file was auto-generated!
 
 ==============================================================================
 */




#ifndef __UPGRADEHANDLER__
#define __UPGRADEHANDLER__

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#if JUCE_MAC

#include "../Common/DashCommon.h"
#include "../DataReader/dashPipe.h"
#include "../Common/HubConfiguration.h"

#include <stdio.h>
#include <stdlib.h>
//#include <unistd.h>
#include <sys/stat.h>
#include <signal.h>

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

    void setUpgradeState(int state);

    int getUpgradeState();

    //==============================================================================
    static void childProcessExitCallback(int signal);
    
    void launchNrfutil(UpgradeFirm FirmType, uint8_t * portCOM);
    
    void closeNrfutil();
    
    void checkReleasesVersion();

    void launchUpgradeProcedure();
    
    void startRingUpgrade();

    void startHubUpgrade();
    
    String getPortPath(uint8_t * data);
    //==============================================================================
    
private:
    pid_t childPid = 0;
    bool childFinished = false;
    int out_pipe[2], err_pipe[2];
    String childOutFileName = "nrfutilOut.txt";
    String childErrFileName = "nrfutilErr.txt";
    String nrfutilRelativePath = "/Application Support/Enhancia/FirmwareReleases/nrfutil_5.1.0";
    String releaseRelativePath = "/Application Support/Enhancia/FirmwareReleases/";
    String logsRelativePath = "/Logs/Enhancia/NeovaDashboard/Logs/";
    char const * upgradeHubCommandLine[3] = {"dfu", "usb-serial", "-pkg"};
    char const * upgradeRingCommandLine[7] = {"dfu", "ble", "-ic", "NRF52", "-n", "\"Neova DFU\"", "-pkg"};
    bool upgradeCommandReceived = false;
    uint16_t hubReleaseVersion = 0;
    uint16_t ringReleaseVersion = 0;
    int upgradeState = 0;
    //==============================================================================
    DashPipe& dPipe;
    HubConfiguration& hubConfig;
    
    static UpgradeHandler * instanceUp;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (UpgradeHandler)
};


#endif // __STATUTPIPE__
#endif //JUCE_MAC
