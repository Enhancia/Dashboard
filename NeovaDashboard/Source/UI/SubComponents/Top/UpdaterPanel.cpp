/*
  ==============================================================================

    UpdaterPanel.cpp
    Created: 12 Mar 2020 4:36:00pm
    Author:  aleva

  ==============================================================================
*/

#include "UpdaterPanel.h"

//==============================================================================
UpdaterPanel::UpdaterPanel (DashUpdater& updtr, ApplicationCommandManager& manager, float& updateProgress)
    : updater (updtr), progress (updateProgress), commandManager (manager)
{
    createLabels();
    createButtons();

    updateComponentsForSpecificStep (noDownloadAvailable);
}

UpdaterPanel::~UpdaterPanel()
{
}

void UpdaterPanel::paint (Graphics& g)
{
    using namespace neova_dash::colour;
    
    // updater panel area
    g.setColour (topPanelBackground);
    g.fillRoundedRectangle (panelArea.reduced (1).toFloat(), 10.0f);
}

void UpdaterPanel::resized()
{
    using namespace neova_dash::ui;

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
    if
    {
        bottomButton->setBounds (buttonArea.removeFromRight (buttonArea.getWidth()/2)
                                           .withSizeKeepingCentre (bottomButton->getBestWidthForHeight (buttonArea.getHeight()),
                                                                                                        buttonArea.getHeight()));

        viewNotesButton->setBounds (buttonArea.withSizeKeepingCentre (bottomButton->getBestWidthForHeight (buttonArea.getHeight()),
                                                                                                           buttonArea.getHeight()));
    }
    else
    {
        bottomButton->setBounds (buttonArea.withSizeKeepingCentre (bottomButton->getBestWidthForHeight (buttonArea.getHeight()),
                                                                                                        buttonArea.getHeight()));
    }

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

    bodyText->setText ("Progress :\n\n" + String (int (progress*100)) + " %", dontSendNotification);
}


void UpdaterPanel::buttonClicked (Button* bttn)
{
    if (bttn == closeButton.get())
    {
        closeAndResetPanel();
    }

    else if (bttn == viewNotesButton.get())
    {
        URL ("https://www.enhancia-music.com/dashboard-release-notes/").launchInDefaultBrowser();
    }

    else if (bttn == bottomButton.get())
    {
        switch (currentProgress)
        {
            case noDownloadAvailable:
                closeAndResetPanel();
                break;
            case downloadAvailable:
                updateComponentsForSpecificStep (inProgress);
                startTimerHz (2);
                updater.startDownloadProcess();
                break;

            case updateRequired:
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
                    updater.launchInstaller();
                }
                closeAndResetPanel();
                break;
            default:
                break;
        }
    }
}
void UpdaterPanel::resetAndOpenPanel (bool updateIsRequired)
{
    if (currentProgress != inProgress)
    {
        if (isTimerRunning ()) stopTimer();
        updateComponentsForSpecificStep (updateIsRequired ? updateRequired
                                                          : updater.hasNewAvailableVersion() ? downloadAvailable
                                                                                             : noDownloadAvailable);

        setVisible (true);
    }
}

void UpdaterPanel::closeAndResetPanel()
{
    if (currentProgress != inProgress)
    {
        if (isTimerRunning ()) stopTimer();

        setVisible (false);
        updateComponentsForSpecificStep (noDownloadAvailable);

        commandManager.invokeDirectly (neova_dash::commands::checkAndUpdateNotifications, true);
    }
}

void UpdaterPanel::createLabels()
{
    titleLabel.reset (new Label ("Title Label", ""));
    addAndMakeVisible (*titleLabel);
    titleLabel->setFont (neova_dash::font::dashFontBold.withHeight (25.0f));
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

    // View notes button
    viewNotesButton.reset (new TextButton ("View Notes Button"));
    addAndMakeVisible (*viewNotesButton);

    viewNotesButton->setButtonText ("View Notes");
    viewNotesButton->addListener (this);

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
            case noDownloadAvailable:
                closeButton->setVisible (true);
                bottomButton->setVisible (true);
                viewNotesButton->setVisible (false);
                bottomButton->setButtonText ("Ok");

                titleLabel->setText ("Dashboard up to date", dontSendNotification);

                bodyText->setText ("No new Dashboard version was found.",
                                   dontSendNotification);
                break;

            case downloadAvailable:
                closeButton->setVisible (true);
                bottomButton->setVisible (true);
                viewNotesButton->setVisible (true);
                bottomButton->setButtonText ("Download");

                titleLabel->setText ("Dashboard Update", dontSendNotification);

                bodyText->setText ("A new Dashboard version is available!\n\n\n"
                                   "Current : " + JUCEApplication::getInstance()->getApplicationVersion()
                                                + "\n\nNew : " + updater.getLatestVersionString(),
                                   dontSendNotification);
                break;


            case updateRequired:
                closeButton->setVisible (true);
                viewNotesButton->setVisible (false);
                bottomButton->setVisible (updater.hasNewAvailableVersion());
                bottomButton->setButtonText ("Download");

                titleLabel->setText ("Dashboard Update (Required)", dontSendNotification);
                
                if (updater.hasNewAvailableVersion())
                {
                    bodyText->setText ("Your Dashboard is outdated! Please update your Dashboard to use it with your Neova product.\n\n\n"
                                       "Current : " + JUCEApplication::getInstance()->getApplicationVersion()
                                                    + "\n\nNew : " + updater.getLatestVersionString(),
                                       dontSendNotification);
                }
                else
                {
                    bodyText->setText ("No up-to-date version found.\n\nMake sure you are connected to internet and try again.",
                                       dontSendNotification);
                }
                break;

            case inProgress:
                closeButton->setVisible (false);
                bottomButton->setVisible (false);
                viewNotesButton->setVisible (false);
                titleLabel->setText ("Downloading  . . .", dontSendNotification);
                bodyText->setText ("Preparing download", dontSendNotification);
                break;

            case downloadFinished:
                closeButton->setVisible (true);
                bottomButton->setVisible (true);
                viewNotesButton->setVisible (false);
                titleLabel->setText ("Download Finished", dontSendNotification);

                if (updater.wasSuccessful())
                {
                    bottomButton->setButtonText ("Start Installer");
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

    resized();
}