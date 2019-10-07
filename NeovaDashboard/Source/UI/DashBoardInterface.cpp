/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#include "DashBoardInterface.h"

//==============================================================================
DashBoardInterface::DashBoardInterface (HubConfiguration& data, DataReader& reader)
    : hubConfig (data), dataReader (reader)
{
    TRACE_IN;

    setLookAndFeel (&dashBoardLookAndFeel);

    // Creates Components
    optionsPanel = std::make_unique<OptionsPanel> (hubConfig, getCommandManager());
    addAndMakeVisible (*optionsPanel);

    header = std::make_unique<HeaderComponent> (*optionsPanel, hubConfig, dataReader);
    addAndMakeVisible (*header);
	
    uploadButton = std::make_unique<UploadButton> (getCommandManager());
    addAndMakeVisible (*uploadButton);

    newGesturePanel = std::make_unique<NewGesturePanel> (hubConfig, getCommandManager());
    addAndMakeVisible (*newGesturePanel);

    gesturePanel = std::make_unique<GesturePanel> (hubConfig, dataReader, *newGesturePanel,
                                                   getCommandManager(), neova_dash::ui::FRAMERATE);
    addAndMakeVisible (*gesturePanel);

    hubComponent = std::make_unique<HubComponent> (hubConfig, *newGesturePanel, getCommandManager());
    addAndMakeVisible (*hubComponent);

    presetSelector = std::make_unique<PresetSelectorComponent> (hubConfig, getCommandManager());
    addAndMakeVisible (*presetSelector);
    presetSelector->addMouseListener (this, true);

    // Top panel properties
    newGesturePanel->hidePanel();
    newGesturePanel->setAlwaysOnTop (true);
    optionsPanel->setVisible (false);
    optionsPanel->setAlwaysOnTop (true);

    // Sets settings
    Rectangle<int> screenArea  = Desktop::getInstance().getDisplays()
                                                       .getMainDisplay()
                                                       .userArea;

    int dashWidth = jmin (screenArea.getHeight()*63/60, // screenH * 9/10 * AspectRatio^-1 (= 7/6)
                          screenArea.getWidth()*3/4);

    //dashWidth = 850; // TO DELETE

    setSize (dashWidth,
             dashWidth*6/7);
}

DashBoardInterface::~DashBoardInterface()
{
    TRACE_IN;

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
    {
        auto uploadShadowBounds = uploadButton->getBounds().reduced (neova_dash::ui::MARGIN)
                                                           .withLeft (getX()-10);
        if (uploadButton->isDown()) uploadShadowBounds = uploadShadowBounds.withTrimmedRight (1);

        shadowPath.addRoundedRectangle (uploadShadowBounds.toFloat(), 8.0f);
    }

    DropShadow shadow (Colour (0x40000000), 10, {2, 3});
    shadow.drawForPath (g, shadowPath);
}

void DashBoardInterface::resized()
{
    using namespace neova_dash::ui;

    auto area = getLocalBounds();
    optionsPanel->setBounds (area);

	auto gPanelArea = area.removeFromBottom (area.getHeight() / 2 - 5);

    gesturePanel->setBounds (gPanelArea.reduced (0, MARGIN));
    newGesturePanel->setBounds (gPanelArea);

    header->setBounds (area.removeFromTop (HEADER_HEIGHT).reduced (MARGIN_SMALL, MARGIN));

    presetSelector->setBounds (area.removeFromBottom (10).withSizeKeepingCentre (area.getWidth()/6, 30));
    hubComponent->setBounds (area.withSizeKeepingCentre (area.getHeight(), area.getHeight())
                                 .translated (0, -10));
    uploadButton->setBounds (area.withSize (jmax (100, area.getWidth()/7), area.getHeight()*6/10)
                                 .withSizeKeepingCentre (jmax (100, area.getWidth()/7), HEADER_HEIGHT));
}

//==============================================================================
void DashBoardInterface::mouseUp (const MouseEvent& event)
{
	if (hubComponent->getControlButtonDown()
        && event.eventComponent->getParentComponent() == presetSelector.get())
    {
        hubComponent->setControlButtonDown (false);
    }
}

void DashBoardInterface::mouseEnter (const MouseEvent& event)
{
    if ((event.eventComponent == presetSelector.get() ||
         event.eventComponent->getParentComponent() == presetSelector.get())
        && hubComponent->getCurrentMode() == HubComponent::gestureMute)
    {
        hubComponent->switchHubMode();
    }
}

void DashBoardInterface::mouseExit (const MouseEvent& event)
{
    if ((event.eventComponent == presetSelector.get() ||
         event.eventComponent->getParentComponent() == presetSelector.get())
        && hubComponent->getCurrentMode() == HubComponent::presetSelection
        && !commandKeyDown && !hubComponent->getControlButtonDown())
    {
        hubComponent->switchHubMode();
    }
}

void DashBoardInterface::modifierKeysChanged (const ModifierKeys& modifiers)
{
    if (modifiers.isCommandDown() && !commandKeyDown
                                  && hubComponent->getCurrentMode() != HubComponent::presetSelection)
    {
        hubComponent->switchHubMode();
    }
    else if (!modifiers.isCommandDown() && commandKeyDown
                                        && hubComponent->getCurrentMode() == HubComponent::presetSelection
                                        && !presetSelector->isMouseOver()
                                        && !presetSelector->getChildComponent (0)->isMouseOver()
                                        && !presetSelector->getChildComponent (1)->isMouseOver())
    {
        hubComponent->switchHubMode();
    }

    commandKeyDown = modifiers.isCommandDown();
}

bool DashBoardInterface::keyPressed (const KeyPress& key)
{
    if (key == KeyPress ('s', ModifierKeys (ModifierKeys::commandModifier), 's'))
    {
        uploadButton->triggerClick();
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
                            updateInterfaceLEDs,
                            updateBatteryDisplay
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
        case updateInterfaceLEDs:
            result.setInfo ("Update LEDs", "Udpates Hub LEDs To Current Hub Configuration", "Interface", 0);
            break;
        case updateBatteryDisplay:
            result.setInfo ("Update Battery Display", "Udpates Battery Display Inside The Header", "Interface", 0);
            break;
        default:
            break;
    }
}

bool DashBoardInterface::perform (const InvocationInfo& info)
{
    TRACE_IN;

    using namespace neova_dash::commands;
    Logger::writeToLog ("Front performs : " + String (getCommandManager().getNameOfCommand (info.commandID)));

    switch (info.commandID)
    {
        case updateDashInterface:
            update();
			return true;
			
        case updateBatteryDisplay:
            header->update();
            return true;

        case updateInterfaceLEDs:
            hubComponent->repaintLEDs();
            return true;

        default:
            return false;
    }
}

void DashBoardInterface::createAndShowAlertPanel (const String& title, const String& message,
                                                   const String& buttonText)
{
    alertPanel.reset (new DashAlertPanel (title, message, buttonText));
    addAndMakeVisible (*alertPanel);

    alertPanel->setVisible (true);
    alertPanel->setAlwaysOnTop (true);
    alertPanel->setLookAndFeel (&dashBoardLookAndFeel);
    alertPanel->setBounds (getLocalBounds());

    //if (!buttonText.isEmpty()) alertPanel->addButton (buttonText, 0, KeyPress (KeyPress::escapeKey));

    //alertPanel->setOpaque (false);
    alertPanel->enterModalState (true, ModalCallbackFunction::forComponent (alertPanelCallback, this), false);
}

void DashBoardInterface::closePendingAlertPanel()
{
    if (alertPanel->isCurrentlyModal()) alertPanel->exitModalState (0);
    alertPanel.reset (nullptr);
}

void DashBoardInterface::alertPanelCallback (int modalResult, DashBoardInterface* interface)
{
    interface->closePendingAlertPanel();
}

//==============================================================================
void DashBoardInterface::update()
{
    hubComponent->update();
    gesturePanel->update();
    presetSelector->update();
    header->update();
}