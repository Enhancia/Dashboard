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
    g.setColour (topPanelBackground.withAlpha (0.85f));
    g.fillRoundedRectangle (panelArea.toFloat(), 10.0f);
    
    // options panel outline
    auto gradOut = ColourGradient::horizontal (Colour (0x10ffffff),
                                               0.0f, 
                                               Colour (0x10ffffff),
                                               float(getWidth()));
    gradOut.addColour (0.5, Colour (0x50ffffff));

    g.setGradientFill (gradOut);
    g.drawRoundedRectangle (panelArea.reduced (1).toFloat(), 10.0f, 1.0f);
}

void FirmUpgradePanel::resized()
{
	using namespace neova_dash::ui;

    panelArea = getLocalBounds().withSizeKeepingCentre (getLocalBounds().getWidth()/3, getLocalBounds().getHeight()/3);
    
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

    auto buttonArea = area.removeFromBottom (area.getHeight()/5);

    okButton->setBounds (buttonArea.withSizeKeepingCentre (okButton->getBestWidthForHeight (buttonArea.getHeight()),
                                     						buttonArea.getHeight()));

    hubUpgradeButton->setBounds (buttonArea.removeFromLeft (buttonArea.getWidth()/2)
                                	 .withSizeKeepingCentre
                                    (area.getWidth()/2 - neova_dash::ui::MARGIN*2,
                                     buttonArea.getHeight()));

    ringUpgradeButton->setBounds (buttonArea.withSizeKeepingCentre
                                    (area.getWidth()/2 - neova_dash::ui::MARGIN,
                                     buttonArea.getHeight()));

    bodyText->setBounds (area.reduced (neova_dash::ui::MARGIN));
}

void FirmUpgradePanel::timerCallback()
{
	DBG ("Upgrade timer : state " + String (int (currentState)) + " - Internal " + String (upgradeHandler.getUpgradeState()));

	if (upgradeHandler.getUpgradeState() != int (currentState))
	{
		updateComponentsForSpecificState (upgradeHandler.getUpgradeState());

		if (currentState == upgradeSuccessfull || currentState < 0) stopTimer();
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
		closeAndResetPanel();
	}

	else if (bttn == hubUpgradeButton.get())
	{
		upgradeHandler.startHubUpgrade();
		startTimer (1000);
	}

	else if (bttn == ringUpgradeButton.get())
	{
		upgradeHandler.startRingUpgrade();
		startTimer (1000);
	}
}

void FirmUpgradePanel::setAndOpenPanel()
{
	updateComponentsForSpecificState (checkingReleases);
	setVisible (true);
}

void FirmUpgradePanel::closeAndResetPanel()
{
	if (isTimerRunning ()) stopTimer();

	setVisible (false);
	updateComponentsForSpecificState (checkingReleases);
}

void FirmUpgradePanel::createLabels()
{
    titleLabel.reset (new Label ("Title Label", ""));
    addAndMakeVisible (*titleLabel);
    titleLabel->setJustificationType (Justification::centred);

    bodyText.reset (new Label ("Body Text", ""));
    addAndMakeVisible (*bodyText);
    bodyText->setJustificationType (Justification::centred);
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
		case (int (checkingReleases))      : updateComponentsForSpecificState (checkingReleases);      break;
		case (int (waitingForUpgradeFirm)) : updateComponentsForSpecificState (waitingForUpgradeFirm); break;
		case (int (upgradeFirmConnected))  : updateComponentsForSpecificState (upgradeFirmConnected);  break;
		case (int (upgradeInProgress))     : updateComponentsForSpecificState (upgradeInProgress);     break;
		case (int (upgradeSuccessfull))    : updateComponentsForSpecificState (upgradeSuccessfull);    break;
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

		switch (upgradeStateToUpdateTo)
		{
				case checkingReleases:
					closeButton->setVisible (true);
					hubUpgradeButton->setVisible (hubAvailable);
					ringUpgradeButton->setVisible (ringAvailable);
					if (!hubAvailable && !ringAvailable) okButton->setVisible (true);

					titleLabel->setText ("Firmware Update", dontSendNotification);

					bodyTextString = String ("HUB : ");
					if (hubAvailable)
					{
						bodyTextString += "Upgrade Available (v" + String (upgradeHandler.getHubReleaseVersion()) + ")";
					}
					else
					{
						bodyTextString += "Up to date";
					}
					
					bodyTextString += String ("\n\nRing : ");

					if (ringAvailable)
					{
						bodyTextString += "Upgrade Available : (v" + String (upgradeHandler.getRingReleaseVersion()) + ")";
					}
					else
					{
						bodyTextString += "Up to date";
					}
					
					// TODO add string
					
					bodyText->setText (bodyTextString,
						               dontSendNotification);
					break;

				case waitingForUpgradeFirm:

					titleLabel->setText ("Upgrade In Progress", dontSendNotification);
					bodyText->setText ("Step 1 : Waiting for Device Setup...", dontSendNotification);
					break;

				case upgradeFirmConnected:

					titleLabel->setText ("Upgrade In Progress", dontSendNotification);
					bodyText->setText ("Step 1 : OK\n\n", dontSendNotification);

					break;

				case upgradeInProgress:

					titleLabel->setText ("Upgrade In Progress", dontSendNotification);
					bodyText->setText ("Step 1 : OK\n\nStep 2 : Upgrading Firmware...", dontSendNotification);

					break;

				case upgradeSuccessfull:
					closeButton->setVisible (true);
					okButton->setVisible (true);

					titleLabel->setText ("Upgrade Finished", dontSendNotification);
					bodyText->setText ("Succesfully Upgraded Firmware!", dontSendNotification);
					break;
				default:
					break;
		}
	}
}

void FirmUpgradePanel::updateComponentsForError (UpgradeState upgradeStateToUpdateTo)
{
	stopTimer();

	closeButton->setVisible (true);
	okButton->setVisible (true);
	ringUpgradeButton->setVisible (false);
	hubUpgradeButton->setVisible (false);
	titleLabel->setText ("Error", dontSendNotification);

	switch (upgradeStateToUpdateTo)
	{
		case err_ringIsNotConnected:
			bodyText->setText ("Your ring is disconnected.\nPlease connect your ring and retry.",
				               dontSendNotification);
			break;

		case err_waitingForUpgradeFirmTimeOut:
			bodyText->setText ("Device took too long to respond.\nPlease unplug the HUB and retry.",
				               dontSendNotification);
			break;

		case err_upgradeLaunchFailed:
			bodyText->setText ("Upgrade process failed to launch properly.\nPlease try again later.",
				               dontSendNotification);
			break;

		case err_upgradefailed:
			bodyText->setText ("Upgrade failed to install properly.\nPlease try again later.",
				               dontSendNotification);
			break;

		default: // error unknown
			bodyText->setText ("An unexpected error occured. Please try again later.",
				               dontSendNotification);

			break;
	}	
}