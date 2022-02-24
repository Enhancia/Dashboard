/*
  ==============================================================================

    FirmUpgradePanel.cpp
    Created: 3 Apr 2020 3:24:54pm
    Author:  aleva

  ==============================================================================
*/

#include "FirmUpgradePanel.h"

//==============================================================================
FirmUpgradePanel::FirmUpgradePanel (HubConfiguration& config, UpgradeHandler& handler, ApplicationCommandManager& manager)
	: hubConfig (config), upgradeHandler (handler), commandManager (manager)
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
    
    // firmware panel area
    g.setColour (topPanelBackground);
    g.fillRoundedRectangle (panelArea.reduced (1).toFloat(), 10.0f);

    if (currentState > 0 && currentState < upgradeSuccessfull)
    {
    	g.setColour (subText);
    	g.setFont (neova_dash::font::dashFontNorms.withHeight (13.0f));
    	g.drawFittedText ("Do not disconnect Neova during the upgrade.\n\nPlease keep your ring charging during the process.", panelArea.reduced (neova_dash::ui::MARGIN),
    				Justification::centredBottom, 3);
    }

    grabKeyboardFocus ();
}

void FirmUpgradePanel::resized()
{
	using namespace neova_dash::ui;

    //panelArea = getLocalBounds().withSizeKeepingCentre (getLocalBounds().getWidth()/3, getLocalBounds().getHeight()/3);
    //panelArea = getLocalBounds().withSizeKeepingCentre (getLocalBounds().getWidth()*3/5, getLocalBounds().getHeight()*3/5);
    panelArea = getLocalBounds().reduced (getWidth()/5, getHeight()/4);
    
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

    upgradeButton->setBounds (buttonArea.withSizeKeepingCentre (upgradeButton->getBestWidthForHeight (buttonArea.getHeight()),
                                     						    buttonArea.getHeight()));

    bodyText->setBounds (area.reduced (area.getWidth()/4, neova_dash::ui::MARGIN));
}

void FirmUpgradePanel::timerCallback()
{
	DBG ("Upgrade timer : state " + String (int (currentState)) + " - Internal " + String (upgradeHandler.getUpgradeState()));
	if (upgradeHandler.getUpgradeState() == int (checkingReleases) && currentState == preInstallationWarning) return;

	// The timer is checking backend state and updates interface accordingly
	if (upgradeHandler.getUpgradeState() != int (currentState))
	{
		if (upgradeHandler.getUpgradeState() != UpgradeHandler::upgradeSuccessfull)
		{
			if (currentState < 0) stopTimer(); // Error: terminates the upgrade process

			updateComponentsForSpecificState (upgradeHandler.getUpgradeState());
			repaint();
		}
		else if (currentState != waitingForHubReconnect)
		{
			jassert (currentState == upgradeInProgress);
			updateComponentsForSpecificState (waitingForHubReconnect);
			startTimeoutCount();
			
			if (hubConfig.getHubIsConnected())
			{
				updateAfterHubConnection();
			}
		}
	}

	if (currentState == waitingForHubReconnect)
	{
		timeoutCheck();
	}

	animateUpgrade();
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

			upgradeHandler.startUpgrade();

			startTimer (1000);
		}
		else
		{
			closeAndResetPanel();
		}
	}

	else if (bttn == upgradeButton.get())
	{
		bool hubAvailable = (upgradeHandler.getHubReleaseVersion() > hubConfig.getHubFirmwareVersionUint16() && hubConfig.getHubIsConnected());
		bool ringAvailable = (upgradeHandler.getRingReleaseVersion() > hubConfig.getRingFirmwareVersionUint16() && hubConfig.getRingIsConnected());

		currentUpgrade = (!ringAvailable && hubAvailable) ? hub : ring;
		updateComponentsForSpecificState (preInstallationWarning);
	}
}

bool FirmUpgradePanel::keyPressed (const KeyPress& key)
{
    if (key == neova_dash::keyboard_shortcut::closeWindow)
    {
        closeAndResetPanel ();
    }

    return false;
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
    
    commandManager.invokeDirectly (neova_dash::commands::checkAndUpdateNotifications, true);
}

void FirmUpgradePanel::updateAfterHubConnection()
{
	if (currentState == waitingForHubReconnect)
	{
		stopTimer();

		if (upgradeHandler.getUpgradeState() == UpgradeHandler::upgradeSuccessfull)
		{
			updateComponentsForSpecificState (upgradeSuccessfull);
		}
		else // successive upgrade
		{
			currentUpgrade = hub;
			updateComponentsForSpecificState (upgradeHandler.getUpgradeState());
			startTimer (1000);
		}
	}
	else
	{
		// Shouldn't be here, hub should not reconnect during install unless we are waiting for it
		jassertfalse;
	}
}

bool FirmUpgradePanel::isWaitingForHubReconnect()
{
	return currentState == waitingForHubReconnect;
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
    upgradeButton.reset (new TextButton ("Upgrade Button"));
    addAndMakeVisible (*upgradeButton);
    upgradeButton->setButtonText ("Upgrade");
    upgradeButton->addListener (this);

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
		bool hubAvailable = (upgradeHandler.getHubReleaseVersion() > hubConfig.getHubFirmwareVersionUint16()) && hubConfig.getHubIsConnected();
		bool ringAvailable = (upgradeHandler.getRingReleaseVersion() > hubConfig.getRingFirmwareVersionUint16()) && hubConfig.getRingIsConnected();

		closeButton->setVisible (false);
		okButton->setVisible (false);
		upgradeButton->setVisible (false);
    	bodyText->setJustificationType (Justification::centredLeft);

		switch (upgradeStateToUpdateTo)
		{
				case checkingReleases:
					closeButton->setVisible (true);
    				okButton->setButtonText ("Ok");
					upgradeButton->setVisible (hubAvailable || ringAvailable);
					if (!hubAvailable && !ringAvailable) okButton->setVisible (true);

					titleLabel->setText ("Firmware Upgrade", dontSendNotification);
    				bodyText->setJustificationType (Justification::centred);

					if (!hubConfig.getHubIsConnected())
					{
						bodyTextString = "No device currently connected to the computer.\n\nPlease connect Neova and retry.";
					}

					else if (!hubAvailable && !ringAvailable)
					{
						bodyTextString = "Neova is up to date!";
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
					break;

				case waitingForUpgradeFirm:
					if (currentUpgrade == hub)
					{
						titleLabel->setText ("Upgrade In Progress : Neova HUB", dontSendNotification);
					}
					else
					{
						titleLabel->setText ("Upgrade In Progress : Neova Ring", dontSendNotification);
					}

					bodyTextString = "Step 1 : Waiting for Device Setup";
					break;

				case upgradeFirmConnected:
					if (currentUpgrade == hub)
					{
						titleLabel->setText ("Upgrade In Progress : Neova HUB", dontSendNotification);
					}
					else
					{
						titleLabel->setText ("Upgrade In Progress : Neova Ring", dontSendNotification);
					}
        
					bodyTextString = "Step 1 : Waiting for Device Setup - OK\n\n  \n\n ";
					break;

				case upgradeInProgress:
					if (currentUpgrade == hub)
					{
						titleLabel->setText ("Upgrade In Progress : Neova HUB", dontSendNotification);
					}
					else
					{
						titleLabel->setText ("Upgrade In Progress : Neova Ring", dontSendNotification);
					}
        
					bodyTextString = "Step 1 : Waiting for Device Setup - OK\n\nStep 2 : Upgrading Firmware";
					break;

				case waitingForHubReconnect:
					if (currentUpgrade == hub)
					{
						titleLabel->setText ("Upgrade In Progress : Neova HUB", dontSendNotification);
					}
					else
					{
						titleLabel->setText ("Upgrade In Progress : Neova Ring", dontSendNotification);
					}
        
					bodyTextString = "Step 1 : Waiting for Device Setup - OK\n\nStep 2 : Upgrading Firmware - OK\n\n"
									 "Step 3 : Waiting for Device Reboot";
					break;

				case upgradeSuccessfull:
					closeButton->setVisible (true);
    				okButton->setButtonText ("Ok");
					okButton->setVisible (true);
    				bodyText->setJustificationType (Justification::centred);

					titleLabel->setText ("Upgrade Finished", dontSendNotification);
					bodyTextString = "Succesfully Upgraded Firmware!";
					break;

				case preInstallationWarning:
					closeButton->setVisible (true);
    				okButton->setButtonText ("Start");
					okButton->setVisible (true);

					titleLabel->setText ("Warning", dontSendNotification);
					bodyTextString = "Please make sure your hub is connected with your ring charging on top.\n\n"
									   "Do not disconnect your HUB during the process.\n\n\n"
									   "The upgrade may take a several minutes to complete.";
					break;

				default:
					break;
		}

		bodyText->setText (bodyTextString, dontSendNotification);
	}
}

void FirmUpgradePanel::updateComponentsForError (UpgradeState upgradeStateToUpdateTo)
{
	stopTimer();
	currentState = upgradeStateToUpdateTo;


	closeButton->setVisible (true);
	okButton->setVisible (true);
	upgradeButton->setVisible (false);
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
			bodyText->setText ("Device took too long to respond.\n\nPlease unplug the Neova and retry.",
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

		case err_timeout:
			bodyText->setText ("Neova took too long to reboot.\n\nPlease disconnect and reconnect Neova.",
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
	if (currentState > 0 && currentState != upgradeSuccessfull)
	{
		if (upgradeAnimationString.length() >= 6) upgradeAnimationString = "";
		else upgradeAnimationString += " .";

		bodyText->setText (bodyTextString + upgradeAnimationString, dontSendNotification);
	}
}

void FirmUpgradePanel::startTimeoutCount()
{
	timeoutCounter = 0;
}

void FirmUpgradePanel::timeoutCheck()
{
	if (timeoutCounter >= 45) // While waiting for the hub, the timer acts as a timeout in case hub doesnt reconnect
	{
		DBG ("HUB didn't reconnect");
		stopTimer();

		updateComponentsForError (err_timeout);
	}

	timeoutCounter += 1;
}