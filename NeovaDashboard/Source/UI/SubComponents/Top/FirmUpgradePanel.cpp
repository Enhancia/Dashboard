/*
  ==============================================================================

    FirmUpgradePanel.cpp
    Created: 3 Apr 2020 3:24:54pm
    Author:  aleva

  ==============================================================================
*/

#include "FirmUpgradePanel.h"

//==============================================================================
FirmUpgradePanel::FirmUpgradePanel (HubConfiguration& config, UpgradeHandler& handler)
	: hubConfig (config), upgradeHandler (handler)
{
    createLabels();
    createButtons();
}

FirmUpgradePanel::~FirmUpgradePanel()
{
}

void FirmUpgradePanel::paint (Graphics& g)
{
    using namespace neova_dash::colour;
    
    // options panel area
    g.setColour (topPanelBackground);
    g.fillRoundedRectangle (panelArea.toFloat(), 10.0f);
    
    /*
    // options panel outline
    auto gradOut = ColourGradient::horizontal (Colour (0x10ffffff),
                                               0.0f, 
                                               Colour (0x10ffffff),
                                               float(getWidth()));
    gradOut.addColour (0.5, Colour (0x50ffffff));

    g.setGradientFill (gradOut);
    g.drawRoundedRectangle (panelArea.reduced (1).toFloat(), 10.0f, 1.0f);*/
}

void FirmUpgradePanel::resized()
{
	using namespace neova_dash::ui;

    //panelArea = getLocalBounds().withSizeKeepingCentre (getLocalBounds().getWidth()/3, getLocalBounds().getHeight()/3);
    panelArea = getLocalBounds().withSizeKeepingCentre (getLocalBounds().getWidth()*3/5, getLocalBounds().getHeight()*3/5);
    
    // Close Button
    #if JUCE_WINDOWS
    closeButton->setBounds (juce::Rectangle<int> (25, 25).withRightX (panelArea.getRight() - MARGIN_SMALL)
                                                         .withY (panelArea.getY() + MARGIN_SMALL));
    #elif JUCE_MAC
    closeButton->setBounds (juce::Rectangle<int> (25, 25).withPosition (panelArea.getTopLeft()
    																			 .translated (MARGIN_SMALL,
    																			   			  MARGIN_SMALL)));
    #endif

    auto area = panelArea.reduced (neova_dash::ui::MARGIN);

    titleLabel->setBounds (area.removeFromTop (area.getHeight()/5));

    auto buttonArea = area.removeFromBottom (jmin (area.getHeight()/5, 40));
    okButton->setBounds (buttonArea.withSizeKeepingCentre (okButton->getBestWidthForHeight (buttonArea.getHeight()),
                                     						buttonArea.getHeight()));

    hubUpgradeButton->setBounds (buttonArea.removeFromLeft (buttonArea.getWidth()/2)
                                	 .withSizeKeepingCentre
                                    (area.getWidth()/3,
                                     buttonArea.getHeight()));

    ringUpgradeButton->setBounds (buttonArea.withSizeKeepingCentre
                                    (area.getWidth()/3,
                                     buttonArea.getHeight()));

    bodyText->setBounds (area.reduced (area.getWidth()/4, neova_dash::ui::MARGIN));
}

void FirmUpgradePanel::timerCallback()
{
	DBG ("Upgrade timer : state " + String (int (currentState)) + " - Internal " + String (upgradeHandler.getUpgradeState()));
	if (upgradeHandler.getUpgradeState() == int (checkingReleases) && currentState == preInstallationWarning) return;

	// Standart case, the timer is checking backend state and updates interface accordingly
	if (currentState != waitingForHubReconnect)
	{
		if (upgradeHandler.getUpgradeState() != int (currentState))
		{
			if (upgradeHandler.getUpgradeState() != UpgradeHandler::upgradeSuccessfull)
			{
				if (currentState < 0) stopTimer(); // Error: terminates the upgrade process

				updateComponentsForSpecificState (upgradeHandler.getUpgradeState());
				animateUpgrade();
			}
			else
			{
				jassert (currentState == upgradeInProgress);

				stopTimer();
				updateComponentsForSpecificState (waitingForHubReconnect);

				startTimer (30000);
			}
		}
	}
	else // While waiting for the hub, the timer acts as a timeout in case hub doesnt reconnect
	{
		DBG ("HUB didn't reconnect");

		stopTimer();
		updateComponentsForSpecificState (err_unknow);
	}
}


void FirmUpgradePanel::buttonClicked (Button* bttn)
{
	if (bttn == closeButton.get())
	{
		closeAndResetPanel();
	}

	else if (bttn == okButton.get())
	{
		if (currentState == preInstallationWarning)
		{
			jassert (currentUpgrade != none);
			if (upgradeHandler.getUpgradeState() != int (checkingReleases))
				updateComponentsForSpecificState (upgradeHandler.getUpgradeState());

			if      (currentUpgrade == hub)	 upgradeHandler.startHubUpgrade();
			else if (currentUpgrade == ring) upgradeHandler.startRingUpgrade();

			startTimer (1000);
		}
		else
		{
			closeAndResetPanel();
		}
	}

	else if (bttn == hubUpgradeButton.get())
	{
		currentUpgrade = hub;
		updateComponentsForSpecificState (preInstallationWarning);
	}

	else if (bttn == ringUpgradeButton.get())
	{
		currentUpgrade = ring;
		updateComponentsForSpecificState (preInstallationWarning);
	}
}

void FirmUpgradePanel::setAndOpenPanel()
{
	currentUpgrade = none;
	updateComponentsForSpecificState (checkingReleases);
	setVisible (true);
}

void FirmUpgradePanel::closeAndResetPanel()
{
	if (isTimerRunning ()) stopTimer();

	setVisible (false);
	currentUpgrade = none;
	updateComponentsForSpecificState (checkingReleases);
}

void FirmUpgradePanel::updateAfterHubConnection()
{
	if (currentState == waitingForHubReconnect)
	{
		stopTimer();
		updateComponentsForSpecificState (upgradeSuccessfull);
	}
	else
	{
		// Shouldn't be here, hub should not reconnect during install unless we are waiting for it
		jassertfalse;
	}
}


void FirmUpgradePanel::createLabels()
{
    titleLabel.reset (new Label ("Title Label", ""));
    addAndMakeVisible (*titleLabel);
    titleLabel->setFont (neova_dash::font::dashFontBold.withHeight (25.0f));
    titleLabel->setJustificationType (Justification::centred);

    bodyText.reset (new Label ("Body Text", ""));
    addAndMakeVisible (*bodyText);
    bodyText->setJustificationType (Justification::centredLeft);
}

void FirmUpgradePanel::createButtons()
{
	// Bottom buttons
    hubUpgradeButton.reset (new TextButton ("hub Button"));
    addAndMakeVisible (*hubUpgradeButton);
    hubUpgradeButton->setButtonText ("Upgrade HUB");
    hubUpgradeButton->addListener (this);

    ringUpgradeButton.reset (new TextButton ("ring Button"));
    addAndMakeVisible (*ringUpgradeButton);
    ringUpgradeButton->setButtonText ("Upgrade Ring");
    ringUpgradeButton->addListener (this);

    okButton.reset (new TextButton ("Ok Button"));
    addAndMakeVisible (*okButton);
    okButton->setButtonText ("Ok");
    okButton->addListener (this);

	// Close button
    closeButton = std::make_unique <DashShapeButton> ("Close Firm Update Button",
                                                       Colour(0),
                                                       neova_dash::colour::mainText);
    addAndMakeVisible (*closeButton);

    Path p;
    p.startNewSubPath (0, 0);
    p.lineTo (3*neova_dash::ui::MARGIN, 3*neova_dash::ui::MARGIN);
    p.startNewSubPath (0, 3*neova_dash::ui::MARGIN);
    p.lineTo (3*neova_dash::ui::MARGIN, 0);

    closeButton->setShape (p, false, true, false);
	closeButton->addListener (this);
}

void FirmUpgradePanel::updateComponentsForSpecificState (int upgradeStateToUpdateTo)
{
	switch (upgradeStateToUpdateTo)
	{
		// Errors
		case (int (err_ringIsNotConnected))           : updateComponentsForError (err_ringIsNotConnected);           break;
		case (int (err_waitingForUpgradeFirmTimeOut)) : updateComponentsForError (err_waitingForUpgradeFirmTimeOut); break;
		case (int (err_upgradeLaunchFailed))          : updateComponentsForError (err_upgradeLaunchFailed);          break;
		case (int (err_upgradefailed))                : updateComponentsForError (err_upgradefailed);                break;
		case (int (err_unknow))                       : updateComponentsForError (err_unknow);                       break;

		// States
		case (int (checkingReleases))       : updateComponentsForSpecificState (checkingReleases);       break;
		case (int (waitingForUpgradeFirm))  : updateComponentsForSpecificState (waitingForUpgradeFirm);  break;
		case (int (upgradeFirmConnected))   : updateComponentsForSpecificState (upgradeFirmConnected);   break;
		case (int (upgradeInProgress))      : updateComponentsForSpecificState (upgradeInProgress);      break;
		case (int (upgradeSuccessfull))     : updateComponentsForSpecificState (upgradeSuccessfull);     break;
		case (int (preInstallationWarning)) : updateComponentsForSpecificState (preInstallationWarning); break;
		case (int (waitingForHubReconnect)) : updateComponentsForSpecificState (waitingForHubReconnect); break;
	}
}

void FirmUpgradePanel::updateComponentsForSpecificState (UpgradeState upgradeStateToUpdateTo)
{
	currentState = upgradeStateToUpdateTo;

	if (int (upgradeStateToUpdateTo) < 0)
	{
		updateComponentsForError (upgradeStateToUpdateTo);
	}
	else
	{
		String bodyTextString;
		bool hubAvailable = upgradeHandler.getHubReleaseVersion() > hubConfig.getHubFirmwareVersionUint16();
		bool ringAvailable = upgradeHandler.getRingReleaseVersion() > hubConfig.getRingFirmwareVersionUint16();

		closeButton->setVisible (false);
		okButton->setVisible (false);
		hubUpgradeButton->setVisible (false);
		ringUpgradeButton->setVisible (false);
    	bodyText->setJustificationType (Justification::centredLeft);

		switch (upgradeStateToUpdateTo)
		{
				case checkingReleases:
					closeButton->setVisible (true);
    				okButton->setButtonText ("Ok");
					hubUpgradeButton->setVisible (hubAvailable);
					ringUpgradeButton->setVisible (ringAvailable);
					if (!hubAvailable && !ringAvailable) okButton->setVisible (true);

					titleLabel->setText ("Firmware Update", dontSendNotification);
    				bodyText->setJustificationType (Justification::centred);

					if (!hubConfig.getHubIsConnected())
					{
						bodyTextString = "No HUB currently connected to the computer.\n\nPlease connect a HUB and retry.";
					}

					else if (!hubAvailable && !ringAvailable)
					{
						bodyTextString = "Your HUB and Ring are up to date!";
					}

					else
					{
						bodyTextString = String ("HUB : ");

						if (hubAvailable)
						{
							bodyTextString += "Upgrade Available (v" + getFormattedVersionString (upgradeHandler.getHubReleaseVersion()) + ")";
						}
						else
						{
							bodyTextString += "Up to date";
						}
						
						bodyTextString += String ("\n\n\nRing : ");

						if (ringAvailable)
						{
							bodyTextString += "Upgrade Available : (v" + getFormattedVersionString (upgradeHandler.getRingReleaseVersion()) + ")";
						}
						else
						{
							bodyTextString += "Up to date";
						}
					}
					bodyText->setText (bodyTextString,
						               dontSendNotification);
					break;

				case waitingForUpgradeFirm:

					titleLabel->setText ("Upgrade In Progress", dontSendNotification);
					bodyText->setText ("Step 1 : Waiting for Device Setup" + upgradeAnimationString + "\n\n \n\n ", dontSendNotification);
					break;

				case upgradeFirmConnected:

					titleLabel->setText ("Upgrade In Progress", dontSendNotification);
					bodyText->setText ("Step 1 : Waiting for Device Setup - OK\n\n  \n\n ", dontSendNotification);

					break;

				case upgradeInProgress:

					titleLabel->setText ("Upgrade In Progress", dontSendNotification);
					bodyText->setText ("Step 1 : Waiting for Device Setup - OK\n\nStep 2 : Upgrading Firmware" + upgradeAnimationString + "\n\n ", dontSendNotification);
					break;

				case waitingForHubReconnect:

					titleLabel->setText ("Upgrade In Progress", dontSendNotification);
					bodyText->setText ("Step 1 : Waiting for Device Setup - OK\n\nStep 2 : Upgrading Firmware - OK\n\n"
									   "Step 3 : Waiting for Device Reboot" + upgradeAnimationString, dontSendNotification);
					break;

				case upgradeSuccessfull:
					closeButton->setVisible (true);
    				okButton->setButtonText ("Ok");
					okButton->setVisible (true);
    				bodyText->setJustificationType (Justification::centred);

					titleLabel->setText ("Upgrade Finished", dontSendNotification);
					bodyText->setText ("Succesfully Upgraded Firmware!", dontSendNotification);
					break;

				case preInstallationWarning:
					closeButton->setVisible (true);
    				okButton->setButtonText ("Start");
					okButton->setVisible (true);

					titleLabel->setText ("Warning", dontSendNotification);
					bodyText->setText ("Please make sure your hub is connected with your ring charging on top.\n\n"
									   "Make sure you ring is connected and has some battery. Do not disconnect your HUB during the process.\n\n"
									   "Please note that it may take a several minutes to complete.", dontSendNotification);
					break;

				default:
					break;
		}
	}
}

void FirmUpgradePanel::updateComponentsForError (UpgradeState upgradeStateToUpdateTo)
{
	stopTimer();
	currentState = upgradeStateToUpdateTo;


	closeButton->setVisible (true);
	okButton->setVisible (true);
	ringUpgradeButton->setVisible (false);
	hubUpgradeButton->setVisible (false);
    bodyText->setJustificationType (Justification::centred);
	titleLabel->setText ("Error", dontSendNotification);
    okButton->setButtonText ("Close");

	switch (upgradeStateToUpdateTo)
	{
		case err_ringIsNotConnected:
			bodyText->setText ("Your ring is disconnected.\n\nPlease connect your ring and retry.",
				               dontSendNotification);
			break;

		case err_waitingForUpgradeFirmTimeOut:
			bodyText->setText ("Device took too long to respond.\n\nPlease unplug the HUB and retry.",
				               dontSendNotification);
			break;

		case err_upgradeLaunchFailed:
			bodyText->setText ("Upgrade process failed to launch properly.\n\nPlease try again later.",
				               dontSendNotification);
			break;

		case err_upgradefailed:
			bodyText->setText ("Upgrade failed to install.\n\nPlease try again later.",
				               dontSendNotification);
			break;

		case err_unknow:
			bodyText->setText ("An unexpected error occured.\n\nPlease try again later.",
				               dontSendNotification);
			break;

		default:
			break;
	}	
}

String FirmUpgradePanel::getFormattedVersionString (uint16_t version)
{
	uint8_t minor = uint8_t (version & 0xff);
	uint8_t major = uint8_t (version >> 8 & 0xff);

	return String (major) + "." + String (minor); 
}

void FirmUpgradePanel::animateUpgrade()
{
	if (upgradeAnimationString.length() == 3) upgradeAnimationString = "";
	else upgradeAnimationString += " .";
}
