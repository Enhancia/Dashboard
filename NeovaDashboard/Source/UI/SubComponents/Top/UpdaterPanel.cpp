/*
  ==============================================================================

    UpdaterPanel.cpp
    Created: 12 Mar 2020 4:36:00pm
    Author:  aleva

  ==============================================================================
*/

#include "UpdaterPanel.h"

//==============================================================================
UpdaterPanel::UpdaterPanel (DashUpdater& updtr, float& updateProgress) : updater (updtr), progress (updateProgress)
{
    createLabels();
    createButtons();

    updateComponentsForSpecificStep (downloadAvailable);
}

UpdaterPanel::~UpdaterPanel()
{
}

void UpdaterPanel::paint (Graphics& g)
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

void UpdaterPanel::resized()
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

    int buttonHeight = area.getHeight()/5;

    bottomButton->setBounds (area.removeFromBottom (buttonHeight)
                                .withSizeKeepingCentre
                                    (bottomButton->getBestWidthForHeight (buttonHeight),
                                     buttonHeight));

    bodyText->setBounds (area.reduced (neova_dash::ui::MARGIN));
}

void UpdaterPanel::timerCallback()
{
	jassert (currentProgress == inProgress);

	if (updater.getDownloadState() == DashUpdater::downloadFinished)
	{
		stopTimer();

		updateComponentsForSpecificStep (downloadFinished);
		
		return;
	}

	bodyText->setText (String (int (progress*100)) + " %", dontSendNotification);
}


void UpdaterPanel::buttonClicked (Button* bttn)
{
	if (bttn == closeButton.get())
	{
		closeAndResetPanel();
	}

	else if (bttn == bottomButton.get())
	{
		switch (currentProgress)
		{
			case downloadAvailable:
				updateComponentsForSpecificStep (inProgress);
				startTimerHz (2);
				updater.startDownloadProcess();
				break;
			case inProgress:
				/* Uh, hitting this should not be possible, since the button should not be visible during the download... */
				jassertfalse;
				break;
			case downloadFinished:
				if (updater.wasSuccessful())
				{
					launchInstaller();
				}
				closeAndResetPanel();
				break;
			default:
				break;
		}
	}
}

void UpdaterPanel::closeAndResetPanel()
{
	if (isTimerRunning ()) stopTimer();

	setVisible (false);
	updateComponentsForSpecificStep (downloadAvailable);
}

void UpdaterPanel::createLabels()
{
    titleLabel.reset (new Label ("Title Label", ""));
    addAndMakeVisible (*titleLabel);
    titleLabel->setJustificationType (Justification::centred);

    bodyText.reset (new Label ("Body Text", ""));
    addAndMakeVisible (*bodyText);
    bodyText->setJustificationType (Justification::centred);
}

void UpdaterPanel::createButtons()
{
	// Bottom button
    bottomButton.reset (new TextButton ("Bottom Button"));
    addAndMakeVisible (*bottomButton);

    bottomButton->setButtonText ("");
    bottomButton->addListener (this);

	// Close button
    closeButton = std::make_unique <DashShapeButton> ("Close Options Button",
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

void UpdaterPanel::updateComponentsForSpecificStep (downloadProgress downloadStep)
{
	currentProgress = downloadStep;

	switch (currentProgress)
	{
			case downloadAvailable:
				closeButton->setVisible (true);
				bottomButton->setVisible (true);
				bottomButton->setButtonText ("Download");

				titleLabel->setText ("New Dashboard Version Available !", dontSendNotification);

				bodyText->setText ("Current : " + JUCEApplication::getInstance()->getApplicationVersion()
												+ "\n\nNew : " + updater.getLatestVersionString(),
					               dontSendNotification);
				break;

			case inProgress:
				closeButton->setVisible (false);
				bottomButton->setVisible (false);

				titleLabel->setText ("Download In Progress . . .", dontSendNotification);
				bodyText->setText ("Waiting to download", dontSendNotification);
				break;

			case downloadFinished:
				closeButton->setVisible (true);
				bottomButton->setVisible (true);
				titleLabel->setText ("Download ended", dontSendNotification);

				if (updater.wasSuccessful())
				{
					bottomButton->setButtonText ("Install");
					bodyText->setText ("Successfully downloaded installer!", dontSendNotification);
				}
				else
				{
					bottomButton->setButtonText ("Close");
					bodyText->setText ("Failed to download installer...", dontSendNotification);
				}
				break;
			default:
				break;
	}
}

void UpdaterPanel::launchInstaller()
{
	if (updater.wasSuccessful())
	{
		if (!updater.getDownloadedFile().startAsProcess())
		{
			updater.getDownloadedFile().revealToUser();
		}
	}
}