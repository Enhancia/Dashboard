/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#include "DashBoardInterface.h"

//==============================================================================
DashBoardInterface::DashBoardInterface (HubConfiguration& data, DataReader& reader, DashUpdater& updtr, UpgradeHandler& upgrdHandler)
    : hubConfig (data), dataReader (reader), updater (updtr), upgradeHandler (upgrdHandler)
{
    setLookAndFeel (&dashBoardLookAndFeel);

    // Creates Components
    optionsPanel = std::make_unique<OptionsPanel> (hubConfig, updater, upgradeHandler, getCommandManager());
    addAndMakeVisible (*optionsPanel);

    firmUpgradePanel = std::make_unique<FirmUpgradePanel> (hubConfig, upgrdHandler, getCommandManager(), dataReader);
    addAndMakeVisible (*firmUpgradePanel);

    updaterPanel = std::make_unique<UpdaterPanel> (updater, getCommandManager(), updater.getDownloadProgressReference());
    addAndMakeVisible (*updaterPanel);

    bugReportPanel = std::make_unique<BugReportPanel> (getCommandManager());
    addAndMakeVisible (*bugReportPanel);

    header = std::make_unique<HeaderComponent> (*optionsPanel, hubConfig, dataReader);
    addAndMakeVisible (*header);
	
    uploadButton = std::make_unique<UploadButton> (hubConfig, getCommandManager());
    addAndMakeVisible (*uploadButton);

    newGesturePanel = std::make_unique<NewGesturePanel> (hubConfig, getCommandManager());
    addAndMakeVisible (*newGesturePanel);

    gesturePanel = std::make_unique<GesturePanel> (hubConfig, dataReader, *newGesturePanel,
                                                   getCommandManager(), neova_dash::ui::FRAMERATE);
    addAndMakeVisible (*gesturePanel);

    hubComponent = std::make_unique<HubComponent> (hubConfig, *newGesturePanel,
                                                   getCommandManager(), presetModeState, state);
    addAndMakeVisible (*hubComponent);

    midiOutputChannelComponent = std::make_unique<MidiChannelComponent> (hubConfig, false);
    addAndMakeVisible (*midiOutputChannelComponent);

    midiInputChannelComponent = std::make_unique<MidiChannelComponent> (hubConfig, true);
    addAndMakeVisible (*midiInputChannelComponent);

    bankSelector = std::make_unique<BankSelectorComponent> (hubConfig, getCommandManager());
    addAndMakeVisible (*bankSelector);
    bankSelector->addMouseListener (this, true);

    // Top panel properties
    newGesturePanel->hidePanel();
    newGesturePanel->setAlwaysOnTop (true);
    optionsPanel->setVisible (false);
    optionsPanel->setAlwaysOnTop (true);
    firmUpgradePanel->setVisible (false);
    firmUpgradePanel->setAlwaysOnTop (true);
    updaterPanel->setAlwaysOnTop (true);
    if (updater.hasNewAvailableVersion())
    {
        optionsPanel->setVisible (true);
        updaterPanel->resetAndOpenPanel();
    }
    else updaterPanel->setVisible (false);
    bugReportPanel->setVisible (false);
    bugReportPanel->setAlwaysOnTop (true);

    // Sets settings
    juce::Rectangle<int> screenArea  = Desktop::getInstance().getDisplays()
                                                       .getMainDisplay()
                                                       .userArea;

    int dashWidth = jmin (screenArea.getHeight()*63/60, // screenH * 9/10 * AspectRatio^-1 (= 7/6)
                          screenArea.getWidth()*3/4);

    //dashWidth = 600; // TO DELETE

    setSize (dashWidth,
             dashWidth*6/7);

    setInterfaceStateAndUpdate (waitingForConnection);

    String neovaHubBase64;
	{
		neovaHubBase64 = "iVBORw0KGgoAAAANSUhEUgAAAREAAABECAYAAACmoAF+AAAACXBIWXMAAAsTAAALEwEAmpwYAAAFwmlUWHRYTUw6Y29tLmFkb2JlLnhtcAAAAAAAPD94cGFja2V0IGJlZ2luPSLvu78iIGlkPSJXNU0wTXBDZWhpSHpyZVN6TlRjemtjOWQiPz4gPHg6eG1wbWV0YSB4bWxuczp4PSJhZG9iZTpuczptZXRhLyIgeDp4bXB0az0iQWRvYmUgWE1QIENvcmUgNS42LWMxNDIgNzkuMTYwOTI0LCAyMDE3LzA3LzEzLTAxOjA2OjM5ICAgICAgICAiPiA8cmRmOlJERiB4bWxuczpyZGY9Imh0dHA6Ly93d3cudzMub3JnLzE5OTkvMDIvMjItcmRmLXN5bnRheC1ucyMiPiA8cmRmOkRlc2NyaXB0aW9uIHJkZjphYm91dD0iIiB4bWxuczp4bXA9Imh0dHA6Ly9ucy5hZG9iZS5jb20veGFwLzEuMC8iIHhtbG5zOmRjPSJodHRwOi8vcHVybC5vcmcvZGMvZWxlbWVudHMvMS4xLyIgeG1sbnM6cGhvdG9zaG9wPSJodHRwOi8vbnMuYWRvYmUuY29tL3Bob3Rvc2hvcC8xLjAvIiB4bWxuczp4bXBNTT0iaHR0cDovL25zLmFkb2JlLmNvbS94YXAvMS4wL21tLyIgeG1sbnM6c3RFdnQ9Imh0dHA6Ly9ucy5hZG9iZS5jb20veGFwLzEuMC9zVHlwZS9SZXNvdXJjZUV2ZW50IyIgeG1wOkNyZWF0b3JUb29sPSJBZG9iZSBQaG90b3Nob3AgQ0Mg"
			"KFdpbmRvd3MpIiB4bXA6Q3JlYXRlRGF0ZT0iMjAyMi0wMi0yOFQxODowNzozMSswMTowMCIgeG1wOk1vZGlmeURhdGU9IjIwMjItMDMtMTNUMTQ6MjU6MTgrMDE6MDAiIHhtcDpNZXRhZGF0YURhdGU9IjIwMjItMDMtMTNUMTQ6MjU6MTgrMDE6MDAiIGRjOmZvcm1hdD0iaW1hZ2UvcG5nIiBwaG90b3Nob3A6Q29sb3JNb2RlPSIzIiB4bXBNTTpJbnN0YW5jZUlEPSJ4bXAuaWlkOjE2ODY5YWRkLTQyYWMtMzI0Yi05YTEwLWY0MGVhN2ViZWM4ZiIgeG1wTU06RG9jdW1lbnRJRD0iYWRvYmU6ZG9jaWQ6cGhvdG9zaG9wOmE1NWMxNTZjLTlhNGQtODU0Ny05YzVhLWI1YTg4YzUxOWM2MSIgeG1wTU06T3JpZ2luYWxEb2N1bWVudElEPSJ4bXAuZGlkOmE2ZjBmZWIxLTkwMTEtODU0OC1iMGIxLTUwYjBmOTczNDdkYiI+IDx4bXBNTTpIaXN0b3J5PiA8cmRmOlNlcT4gPHJkZjpsaSBzdEV2dDphY3Rpb249ImNyZWF0ZWQiIHN0RXZ0Omluc3RhbmNlSUQ9InhtcC5paWQ6YTZmMGZlYjEtOTAxMS04NTQ4LWIwYjEtNTBiMGY5NzM0N2RiIiBzdEV2dDp3aGVuPSIyMDIyLTAyLTI4VDE4OjA3OjMxKzAxOjAwIiBzdEV2dDpzb2Z0d2FyZUFnZW50PSJBZG9iZSBQaG90b3Nob3AgQ0MgKFdpbmRvd3MpIi8+IDxyZGY6bGkgc3RF"
			"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";
	}
	MemoryOutputStream out;
	Base64::convertFromBase64 (out, neovaHubBase64);
	neovaHubImage = ImageFileFormat::loadFrom (out.getData (), out.getDataSize ());
}

DashBoardInterface::~DashBoardInterface()
{
    
    header = nullptr;
    hubComponent = nullptr;
    gesturePanel = nullptr;
    newGesturePanel = nullptr;
    uploadButton = nullptr;

	setLookAndFeel (nullptr);
}

//==============================================================================
void DashBoardInterface::paint (Graphics& g)
{
	g.drawImage (backgroundImage, getLocalBounds().toFloat());

    paintShadows (g);

    if (state != connected)
    {
        drawStateMessage (g);
    }
}

void DashBoardInterface::paintOverChildren (Graphics& g)
{
    if (!optionsPanel->isVisible())
    {    
        const bool hubUpgradeAvailable = (upgradeHandler.getHubReleaseVersion() > hubConfig.getHubFirmwareVersionUint16())  && hubConfig.getRingIsConnected();
        const bool ringUpgradeAvailable = (upgradeHandler.getRingReleaseVersion() > hubConfig.getRingFirmwareVersionUint16()) && hubConfig.getRingIsConnected();
     
        if (hubUpgradeAvailable || ringUpgradeAvailable || updater.hasNewAvailableVersion()) // if Dash Update or Firm Upgrade available
        {
            int alertCount = 0;

            if ((upgradeHandler.getHubReleaseVersion() > hubConfig.getHubFirmwareVersionUint16())
                    && hubConfig.getHubIsConnected())
                alertCount++; // Hub upgrade
            
            if ((upgradeHandler.getRingReleaseVersion() > hubConfig.getRingFirmwareVersionUint16())
                    && hubConfig.getRingIsConnected())
                alertCount++; // Ring upgrade

            if (updater.hasNewAvailableVersion())
                alertCount++; // soft update
            
            // Paint ellipse
            g.setColour (neova_dash::colour::notificationBubble);
            g.fillEllipse (notificationArea.toFloat());

            // Paint Alert Number
            g.setColour (neova_dash::colour::mainText);
            g.setFont (neova_dash::font::dashFontBold.withHeight (15.0f));
            g.drawText (String (alertCount), notificationArea, Justification::centred);
        }

	    if (alertPanel != nullptr && alertPanel->isVisible () && privateNav)
		    g.drawImageAt (neovaHubImage, alertPanel->getX () + ((alertPanel->getWidth () - neovaHubImage.getWidth ()) / 2), alertPanel->getY () + ((alertPanel->getHeight () - neovaHubImage.getHeight ()) / 2));
	    else
		    privateNav = false;
    }
}

void DashBoardInterface::paintShadows (Graphics& g)
{
    Path shadowPath;

    // Header Shadow
    {
        auto headerShadowBounds = header->getBounds();

        shadowPath.addRoundedRectangle (headerShadowBounds.toFloat(), 3.0f);
    }

    // Upload Button Shadow
    if (state == connected)
    {
        auto uploadShadowBounds = uploadButton->getBounds().reduced (neova_dash::ui::MARGIN)
                                                           .withLeft (getX()-10)
                                                           .withTrimmedRight (40);
        if (uploadButton->isDown()) uploadShadowBounds = uploadShadowBounds.withTrimmedRight (1);

        shadowPath.addRoundedRectangle (uploadShadowBounds.toFloat(), 8.0f);
    }

    DropShadow shadow (Colour (0x40000000), 10, {2, 3});
    shadow.drawForPath (g, shadowPath);
}

void DashBoardInterface::drawStateMessage (Graphics& g)
{
    auto area = getLocalBounds().withTop (hubComponent->getBounds().getBottom())
                                .reduced (neova_dash::ui::MARGIN*2);
    g.setColour (neova_dash::colour::mainText);
    g.setFont (neova_dash::font::dashFontNorms.withHeight (35.0f));

    String stateMessage;

    if (state == int (waitingForConnection))
    {
        stateMessage = "Welcome to NEOVA DASHBOARD"
                       "\n\nPlease connect your HUB.";
    }
    else if (state == int (pause))
    {
        stateMessage = "PAUSE";
    }
    else if (state == int (incompatible))
    {
        g.setFont (neova_dash::font::dashFontNorms.withHeight (30.0f));

        stateMessage = "Your Neova is not compatible with "
                       "this version of the Dashboard.\n\n"
                       "Please update Neova and the Dashboard "
                       "to their latest versions.";
    }

    g.drawFittedText (stateMessage, area, Justification::centred, 2);
}

void DashBoardInterface::resized()
{
    using namespace neova_dash::ui;

    auto area = getLocalBounds();
    optionsPanel->setBounds (area);
    firmUpgradePanel->setBounds (area);
    updaterPanel->setBounds (area);
    bugReportPanel->setBounds (area);

	auto gPanelArea = area.removeFromBottom (area.getHeight() / 2 - 35);

    gesturePanel->setBounds (gPanelArea.reduced (0, MARGIN));
    newGesturePanel->setBounds (gPanelArea);

    header->setBounds (area.removeFromTop (HEADER_HEIGHT).reduced (MARGIN_SMALL, MARGIN));

    auto presetAndMidiArea = area.removeFromBottom (15);

    bankSelector->setBounds (presetAndMidiArea.withSizeKeepingCentre (area.getWidth()/6, 30));
    
    midiOutputChannelComponent->setBounds (presetAndMidiArea.withLeft (presetAndMidiArea.getRight() - presetAndMidiArea.getWidth()/3 + MARGIN * 2)
                                                      .withRight (presetAndMidiArea.getRight() - presetAndMidiArea.getWidth()/16)
                                                      .reduced (4*MARGIN, 0)
                                                      .expanded (0, 2));

    midiInputChannelComponent->setBounds (presetAndMidiArea.withRight (presetAndMidiArea.getX() + presetAndMidiArea.getWidth()/3 - MARGIN * 2)
                                                      .withLeft (presetAndMidiArea.getX() + presetAndMidiArea.getWidth()/16)
                                                      .reduced (4*MARGIN, 0)
                                                      .expanded (0, 2));

    hubComponent->setBounds (area.withSizeKeepingCentre (area.getHeight(), area.getHeight())
                                 .translated (0, -10));

    if (state == waitingForConnection)
    {
        hubComponent->setBounds (juce::Rectangle<int> (area.getHeight(), area.getHeight())
                                                        .withCentre ({getWidth()/2, getHeight()*3/8}));
    }

    uploadButton->setBounds (area.withSize (jmax (140, area.getWidth()/7 + 40), area.getHeight()*6/10)
                                 .withSizeKeepingCentre (jmax (140, area.getWidth()/7 + 40), HEADER_HEIGHT));

    notificationArea = juce::Rectangle<int> (18, 18).withCentre (getLocalPoint (header->findChildWithID ("optionsButton"),
                                                                                header->findChildWithID ("optionsButton")
                                                                                      ->getBounds()
                                                                                      .getCentre()
                                                                                      .translated (8, -8)));
}

//==============================================================================
void DashBoardInterface::mouseUp (const MouseEvent& event)
{
	if (hubComponent->getControlButtonDown()
        && event.eventComponent->getParentComponent() == bankSelector.get())
    {
        hubComponent->setControlButtonDown (false);
    }
}

void DashBoardInterface::mouseEnter (const MouseEvent& event)
{
    if ((event.eventComponent == bankSelector.get() ||
         event.eventComponent->getParentComponent() == bankSelector.get())
        && presetModeState == normalState)
    {
        hubComponent->setPresetStateToPresetMode();
    }
}

void DashBoardInterface::mouseExit (const MouseEvent& event)
{
    if ((event.eventComponent == bankSelector.get() ||
         event.eventComponent->getParentComponent() == bankSelector.get())
        && presetModeState == presetState
        && !commandKeyDown && !hubComponent->getControlButtonDown())
    {
        hubComponent->setPresetStateToNormalMode();
    }
}

void DashBoardInterface::modifierKeysChanged (const ModifierKeys& modifiers)
{
    if (modifiers.isCommandDown() && !commandKeyDown
                                  && presetModeState == normalState)
    {
        hubComponent->setPresetStateToPresetMode();
    }
    else if (!modifiers.isCommandDown() && commandKeyDown
                                        && presetModeState == presetState
                                        && !bankSelector->isMouseOver()
                                        && !bankSelector->getChildComponent (0)->isMouseOver()
                                        && !bankSelector->getChildComponent (1)->isMouseOver())
    {
        hubComponent->setPresetStateToNormalMode();
    }

    commandKeyDown = modifiers.isCommandDown();
}

bool DashBoardInterface::keyPressed (const KeyPress& key)
{
    DBG ("KEYPRESS : " << key.getTextDescription());

    if (!key.getModifiers().isAnyModifierKeyDown())
    {
        if (key == neova_dash::keyboard_shortcut::deleteGesture)
        {
            if (gesturePanel->hasSelectedGesture())
            {
                gesturePanel->removeGestureAndGestureComponent (hubConfig.getSelectedGesture());
            }
        }
        else if (key == neova_dash::keyboard_shortcut::selectGestureLeft ||
                 key == neova_dash::keyboard_shortcut::selectGestureRight ||
                 key == neova_dash::keyboard_shortcut::selectGestureUp ||
                 key == neova_dash::keyboard_shortcut::selectGestureDown)
        {
            gesturePanel->handleKeyPress (key);
        }
        else if (key == neova_dash::keyboard_shortcut::muteGesture)
        {
            if (gesturePanel->hasSelectedGesture())
            {
                hubConfig.setUint8Value (hubConfig.getSelectedGesture(),
                                         HubConfiguration::on,
                                         (hubConfig.getGestureData (hubConfig.getSelectedGesture()).on == 1) ? 0 : 1);
                update();
            }
        }
        else if (key == neova_dash::keyboard_shortcut::muteGesture1)
        {
            hubConfig.setUint8Value (0,
                                     HubConfiguration::on,
                                     (hubConfig.getGestureData (0).on == 1) ? 0 : 1);
            update();
        }
        else if (key == neova_dash::keyboard_shortcut::muteGesture2)
        {
            hubConfig.setUint8Value (1,
                                     HubConfiguration::on,
                                     (hubConfig.getGestureData (1).on == 1) ? 0 : 1);
            update();
        }
        else if (key == neova_dash::keyboard_shortcut::muteGesture3)
        {
            hubConfig.setUint8Value (2,
                                     HubConfiguration::on,
                                     (hubConfig.getGestureData (2).on == 1) ? 0 : 1);
            update();
        }
        else if (key == neova_dash::keyboard_shortcut::muteGesture4)
        {
            hubConfig.setUint8Value (3,
                                     HubConfiguration::on,
                                     (hubConfig.getGestureData (3).on == 1) ? 0 : 1);
            update();
        }
    }

    else if (key.getModifiers().isCommandDown())
    {
        if (key == neova_dash::keyboard_shortcut::uploadToHub)
        {
            uploadButton->triggerClick();
        }
        else if (key == neova_dash::keyboard_shortcut::duplicateGesture)
        {
            hubConfig.duplicateGesture (hubConfig.getSelectedGesture());
            update();
        }
        else if (key == neova_dash::keyboard_shortcut::displayOptions)
        {
            optionsPanel->setVisible (!optionsPanel->isVisible());
        }
        else if (key == neova_dash::keyboard_shortcut::selectPreviousBank && hubConfig.getSelectedPreset() > 0)
        {
            hubConfig.setPreset (hubConfig.getSelectedPreset() - 1);
            update();
        }
        else if (key == neova_dash::keyboard_shortcut::selectNextBank && hubConfig.getSelectedPreset() < neova_dash::gesture::NUM_PRESETS - 1)
        {
            hubConfig.setPreset (hubConfig.getSelectedPreset() + 1);
            update();
        }
        else if (key == neova_dash::keyboard_shortcut::selectBank1)
        {
            hubConfig.setPreset (0);
            update();
        }
        else if (key == neova_dash::keyboard_shortcut::selectBank2)
        {
            hubConfig.setPreset (1);
            update();
        }
        else if (key == neova_dash::keyboard_shortcut::selectBank3)
        {
            hubConfig.setPreset (2);
            update();
        }
        else if (key == neova_dash::keyboard_shortcut::selectBank4)
        {
            hubConfig.setPreset (3);
            update();
        }
        else if (key.getModifiers().isShiftDown())
        {
            if (key == neova_dash::keyboard_shortcut::easterEgg)
            {
                if (!optionsPanel->isVisible ()) {
                    privateNav = !privateNav;
                    createAndShowAlertPanel ("Private Navigation", "", "Yes", true, 0);
                }
            }
        }
    }

    else if (key.getModifiers().isShiftDown())
    {
        if (key == neova_dash::keyboard_shortcut::selectPreviousBank)
        {
            hubConfig.setPreset ((hubConfig.getSelectedPreset() - 1) % neova_dash::gesture::NUM_PRESETS);
            update();
        }
    }

    return false;
}

//==============================================================================
ApplicationCommandTarget* DashBoardInterface::getNextCommandTarget()
{
    return findFirstTargetParentComponent();
}

void DashBoardInterface::getAllCommands (Array<CommandID> &commands)
{
    using namespace neova_dash::commands;

    commands.addArray ({
                            updateDashInterface,
							setStateAndUpdateDashInterface,
                            updateInterfaceLEDs,
                            updateBatteryDisplay,
                            allowUserToFlashHub,
                            disallowUserToFlashHub,
                            openFirmUpgradePanel,
                            openDashboardUpdatePanel,
                            checkAndUpdateNotifications,
                            openBugReportPanel,
                            openFactoryResetPanel
                       });
}

void DashBoardInterface::getCommandInfo (CommandID commandID, ApplicationCommandInfo& result)
{
    using namespace neova_dash::commands;

    switch (commandID)
    {
        case updateDashInterface:
            result.setInfo ("Update Full Interface", "Udpates Interface To Current Hub Configuration", "Interface", 0);
            break;
        case setStateAndUpdateDashInterface:
            result.setInfo ("Update State And Interface", "Udpates Interface To Match Current Hub State And Configuration", "Interface", 0);
            break;
        case updateInterfaceLEDs:
            result.setInfo ("Update LEDs", "Udpates Hub LEDs To Current Hub Configuration", "Interface", 0);
            break;
        case updateBatteryDisplay:
            result.setInfo ("Update Battery Display", "Udpates Battery Display Inside The Header", "Interface", 0);
            break;
        case allowUserToFlashHub:
            result.setInfo ("Update Upload Button", "Allows Upload Button To Be Clicked", "Interface", 0);
            break;
        case disallowUserToFlashHub:
            result.setInfo ("Update Upload Button", "Disallows Upload Button To Be Clicked", "Interface", 0);
            break;
        case openFirmUpgradePanel:
            result.setInfo ("Open Firm Upgrade Panel", "Opens Panel To Start Firm Upgrade Procedure", "Interface", 0);
			break;
        case openDashboardUpdatePanel:
            result.setInfo ("Open Dashboard Update Panel", "Opens Panel To Start Dash Update Procedure", "Interface", 0);
            break;
        case checkAndUpdateNotifications:
            result.setInfo ("Check And Update Notifications", "Updates info that could trigger a notification"
                                                              " and updates interface to display potential notifications",
                                                              "Interface", 0);
            break;
        case openBugReportPanel:
            result.setInfo ("Open Bug Report Panel", "Opens Panel To Bug Report Procedure", "Interface", 0);
            break;
        case openFactoryResetPanel:
            result.setInfo ("Open Factory Reset Panel", "Opens Panel To Factory Reset Procedure", "Interface", 0);
            break;
        default:
            break;
    }
}

bool DashBoardInterface::perform (const InvocationInfo& info)
{
    
    using namespace neova_dash::commands;

    neova_dash::log::writeToLog ("Executing Frontend Command : " + String (getCommandManager().getNameOfCommand (info.commandID)), neova_dash::log::ui);

    switch (info.commandID)
    {
        case updateDashInterface:
            update();
			return true;

        case setStateAndUpdateDashInterface:
            setInterfaceStateAndUpdate();
            return true;
			
        case updateBatteryDisplay:
            if ((state == int (connected) || state == int (pause)) && hubConfig.getHubIsCompatibleInt() < 0)
            {
                setInterfaceStateAndUpdate (incompatible);
            }

            if (state == int (connected))
            {
                header->update();
                hubComponent->repaint();
            }
            return true;

        case updateInterfaceLEDs:
            hubComponent->repaintLEDs();
            return true;

        case allowUserToFlashHub:
            uploadButton->setActive();
            return true;

        case disallowUserToFlashHub:
            uploadButton->setActive (false);
            return true;

        case openFirmUpgradePanel:
            if (!optionsPanel->isVisible())
            {
                optionsPanel->setVisible (true);
            }
            
            firmUpgradePanel->setAndOpenPanel();
            return true;

        case openDashboardUpdatePanel:
            if (!optionsPanel->isVisible())
            {
                optionsPanel->setVisible (true);
            }
            
            updaterPanel->resetAndOpenPanel (hubConfig.getHubIsConnected() && hubConfig.getHubIsCompatibleInt() > 0);
            return true;

        case checkAndUpdateNotifications:
            upgradeHandler.checkReleasesVersion();
            updater.checkForNewAvailableVersion();

            updateForNotifications();
            return true;

        case openBugReportPanel:
            if (!optionsPanel->isVisible())
            {
                optionsPanel->setVisible (true);
            }
            
            bugReportPanel->resetAndOpenPanel();
            return true;

        case openFactoryResetPanel:
            if (optionsPanel->isVisible ())
                optionsPanel->setVisible (false);

            createAndShowAlertPanel ("Factory Reset", "This will restore the factory settings. This will overwrite your preferences. Proceed?", "reset", true, DashAlertPanel::factoryReset);
            return true;

        default:
            return false;
    }
}

void DashBoardInterface::setInterfaceStateAndUpdate (const InterfaceState newState)
{
    if (state == int (newState)) return update();

    state = int (newState);

    optionsPanel->update();
    hubComponent->update();

    if (state == int (connected))
    {
        gesturePanel->setVisible (true);
        gesturePanel->update();
        newGesturePanel->hidePanel();
        uploadButton->setVisible (true);
        bankSelector->setVisible (true);
        midiOutputChannelComponent->setVisible (true);
        midiInputChannelComponent->setVisible (true);
        hubComponent->setInterceptsMouseClicks (true, true);
        hubConfig.selectFirstExistingGesture();
        header->setBatteryVisible (true);

        if (firmUpgradePanel->isWaitingForHubReconnect())
        {
            firmUpgradePanel->updateAfterHubConnection();
        }
    }

    else
    {
        gesturePanel->setVisible (false);
        newGesturePanel->hidePanel();
        uploadButton->setVisible (false);
        midiOutputChannelComponent->setVisible (false);
        midiInputChannelComponent->setVisible (false);

        if (state != int (pause))
        {
            uploadButton->setActive (false);
            hubConfig.resetConfigWasChanged();
            header->setBatteryVisible (false);
        }
        
        bankSelector->setVisible (false);
        hubComponent->setInterceptsMouseClicks (false, false);
    }

    resized();
    repaint();

    if (state == int (incompatible))
    {
        if (hubConfig.getHubIsCompatibleInt() > 0)
        {
            optionsPanel->setVisible (true);
            updaterPanel->resetAndOpenPanel (true);
        }

        else if (firmUpgradePanel->isWaitingForHubReconnect())
        {
            firmUpgradePanel->updateAfterHubConnection();
        }

        else if (hubConfig.getHubIsCompatibleInt() < 0)
        {
            // Open Firm upgrade alert
            if (!upgradeHandler.isUpgrading())
            {
                createAndShowAlertPanel ("Your Neova firmware is outdated!", "Please upgrade Neova "
                                                                            "to use it with the Dashboard.",
                                                                            "Upgrade Firmware",
                                                                            true,
                                                                            DashAlertPanel::outdatedFirmware);
            }
        }
    }
}

void DashBoardInterface::setInterfaceStateAndUpdate()
{
    if (hubConfig.getHubIsConnected())
    {
        if (hubConfig.getHubIsCompatibleInt() == 0)
        {
            setInterfaceStateAndUpdate (connected);
        }
        else
        {
            setInterfaceStateAndUpdate (incompatible);
        }
    }
    else
    {
        setInterfaceStateAndUpdate (waitingForConnection);
    }
}

int DashBoardInterface::getPresetModeState()
{
    return presetModeState;
}

void DashBoardInterface::hubChangedPreset()
{
    if (presetModeState == int (presetState))
    {
        if (!commandKeyDown && !bankSelector->isMouseOver()
                            && !bankSelector->getChildComponent (0)->isMouseOver()
                            && !bankSelector->getChildComponent (1)->isMouseOver())
        {
            // Should go back to Normal mode and notify backend
            setPresetStateToNormalMode();
            update();
        }
    }
    else 
    {
        /* 
           Unexpected callback here...
           This alert method should theoretically only happen when in
           "preset" state..
        */
        jassertfalse;
    }
}

void DashBoardInterface::setPresetModeState (const PresetModeState newState)
{
    if (presetModeState == int (newState)) return;

    if (newState == int (slaveState))
    {
        hubComponent->lockHubToPresetMode (true);
    }
    else if (newState == int (normalState))
    {
        if (presetModeState == int (presetState))
        {
            hubComponent->setPresetStateToNormalMode (false);
        }
        else if (presetModeState == int (slaveState))
        {
            hubComponent->lockHubToPresetMode (false);
        }
    }
    else if (newState == int (presetState))
    {
        /* No external object can set the interface to preset mode,
           this change is only allowed internally, through the user's
           actions within the interface, using setPresetStateToPresetMode().
        */
        jassertfalse;
        return;
    }

    presetModeState = int (newState);
}   

void DashBoardInterface::setPresetStateToPresetMode()
{
    hubComponent->setPresetStateToPresetMode();
}

void DashBoardInterface::setPresetStateToNormalMode()
{
    hubComponent->setPresetStateToNormalMode();
}

void DashBoardInterface::createAndShowAlertPanel (const String& title, const String& message,
                                                   const String& buttonText, const bool hasCloseButton,
                                                   int returnValue)
{
    if (alertPanel->isCurrentlyModal()) alertPanel->exitModalState (0);

    alertPanel.reset (new DashAlertPanel (title, message, returnValue, hasCloseButton, buttonText));
    addAndMakeVisible (*alertPanel);

    alertPanel->setVisible (true);
    alertPanel->setAlwaysOnTop (true);
    alertPanel->setLookAndFeel (&dashBoardLookAndFeel);
    alertPanel->setBounds (getLocalBounds());

    alertPanel->enterModalState (true, ModalCallbackFunction::forComponent (alertPanelCallback, this), false);
}

void DashBoardInterface::createAndShowAlertPanel (DashAlertPanel::SpecificReturnValue returnValue)
{
    alertPanel.reset (DashAlertPanel::createSpecificAlertPanel (returnValue));
    addAndMakeVisible (*alertPanel);

    alertPanel->setVisible (true);
    alertPanel->setAlwaysOnTop (true);
    alertPanel->setLookAndFeel (&dashBoardLookAndFeel);
    alertPanel->setBounds (getLocalBounds());

    alertPanel->enterModalState (true, ModalCallbackFunction::forComponent (alertPanelCallback, this), false);
}

void DashBoardInterface::closePendingAlertPanel()
{
    if (alertPanel->isCurrentlyModal()) alertPanel->exitModalState (0);
    alertPanel.reset (nullptr);
}

void DashBoardInterface::alertPanelCallback (int modalResult, DashBoardInterface* interf)
{
    interf->closePendingAlertPanel();
    interf->executePanelAction (modalResult);
}

void DashBoardInterface::executePanelAction (const int panelReturnValue)
{
    switch (panelReturnValue)
    {
        case DashAlertPanel::outdatedFirmware:
            getCommandManager().invokeDirectly (neova_dash::commands::upgradeHub, true);
            break;
        case DashAlertPanel::noUploadQuitting:
            JUCEApplication::getInstance()->systemRequestedQuit();
            break;
        case DashAlertPanel::upgradePending:
            JUCEApplication::getInstance()->systemRequestedQuit();
            break;
        case DashAlertPanel::factoryReset:

            DBG ("FACTORY RESET");
            getCommandManager ().invokeDirectly (neova_dash::commands::factoryReset, true);

            break;
        default: // modalResult 0 or unknown
            break; 
    }
}

//==============================================================================
void DashBoardInterface::update()
{
    if (state == connected)
    {
        hubComponent->update();
        gesturePanel->update();
        bankSelector->update();
        header->update();
        optionsPanel->update();
        midiOutputChannelComponent->update();
        midiInputChannelComponent->update();
        uploadButton->update();
        repaint (notificationArea);
    }
}

void DashBoardInterface::updateForNotifications()
{
    if (state == connected)
    {
        optionsPanel->update();
        repaint (notificationArea);
    }
}
