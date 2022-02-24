/*
  ==============================================================================

    FirmUpgradePanel.h
    Created: 3 Apr 2020 3:24:54pm
    Author:  aleva

  ==============================================================================
*/

#pragma once

#include "../../../../JuceLibraryCode/JuceHeader.h"
#include "../../../Common/DashCommon.h"
#include "../../../Common/HubConfiguration.h"
#include "../DashShapeButton.h"

#if JUCE_WINDOWS
#include "../../../UpgradeHandler/upgradeHandler_Win.h"
#elif JUCE_MAC
#include "../../../UpgradeHandler/upgradeHandler_MacOS.h"
#endif

class FirmUpgradePanel    : public Component,
							public Timer,
                        	private Button::Listener
{
public:
    //==============================================================================
    enum UpgradeState
    {
        //error states
        err_timeout = -6,
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
        upgradeSuccessfull = 4,

        //Warning Panel before download
        preInstallationWarning,

        //Wainting for hub to reconnect after install success
        waitingForHubReconnect
    };

	enum FirmUpgradeType
	{
		hub =0,
		ring,
		none
	};

    //==============================================================================
    FirmUpgradePanel (HubConfiguration& config, UpgradeHandler& handler, ApplicationCommandManager& manager, DataReader& dataReaderRef);
    ~FirmUpgradePanel();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

    //==============================================================================
    void timerCallback() override;

    //==============================================================================
    void buttonClicked (Button* bttn) override;

    //==============================================================================
    void setAndOpenPanel();
    void closeAndResetPanel();
    void updateAfterHubConnection();
    bool isWaitingForHubReconnect();

private:
    //==============================================================================
    void createLabels();
    void createButtons();
    //==============================================================================
	void updateComponentsForSpecificState (UpgradeState upgradeStateToUpdateTo);
	void updateComponentsForSpecificState (int upgradeStateToUpdateTo);
	void updateComponentsForError (UpgradeState upgradeStateToUpdateTo);
	String getFormattedVersionString (uint16_t version);

    //==============================================================================
    void animateUpgrade();
    void startTimeoutCount();
    void timeoutCheck();
    String upgradeAnimationString;
    String bodyTextString;
    int timeoutCounter = 0;

    //==============================================================================
    juce::Rectangle<int> panelArea;
    std::unique_ptr<Label> bodyText;
    std::unique_ptr<Label> titleLabel;
    std::unique_ptr<DashShapeButton> closeButton;
    std::unique_ptr<TextButton> upgradeButton;
    std::unique_ptr<TextButton> okButton;

    //==============================================================================
    UpgradeHandler& upgradeHandler;
    HubConfiguration& hubConfig;
    ApplicationCommandManager& commandManager;
    DataReader& dataReader;
    UpgradeState currentState = checkingReleases; // Follows upgradeHandler upgradeState enum
    FirmUpgradeType currentUpgrade = none; // Follows upgradeHandler upgradeState enum

    //==============================================================================
    int animationStep = 0;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FirmUpgradePanel)
};