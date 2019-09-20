/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#include "DashBoardInterface.h"

//==============================================================================
DashBoardInterface::DashBoardInterface (HubConfiguration& data) : hubConfig (data)
{
    setLookAndFeel (&dashBoardLookAndFeel);

    // Creates Components
    optionsPanel = std::make_unique<OptionsPanel> (hubConfig, getCommandManager());
    addAndMakeVisible (*optionsPanel);

    header = std::make_unique<HeaderComponent> (*optionsPanel);
    addAndMakeVisible (*header);

    hubComponent = std::make_unique<HubComponent> (hubConfig, getCommandManager());
    addAndMakeVisible (*hubComponent);
	
    uploadButton = std::make_unique<UploadButton> (getCommandManager());
    addAndMakeVisible (*uploadButton);

    newGesturePanel = std::make_unique<NewGesturePanel> (hubConfig, getCommandManager());
    addAndMakeVisible (*newGesturePanel);

    gesturePanel = std::make_unique<GesturePanel> (hubConfig, *newGesturePanel,
                                                   getCommandManager(), neova_dash::ui::FRAMERATE);
    addAndMakeVisible (*gesturePanel);

    presetSelector = std::make_unique<PresetSelectorComponent> (hubConfig, getCommandManager());
    addAndMakeVisible (*presetSelector);
    presetSelector->addMouseListener (this, true);

    // Top panel properties
    newGesturePanel->hidePanel();
    newGesturePanel->setAlwaysOnTop (true);
    optionsPanel->setVisible (false);
    optionsPanel->setAlwaysOnTop (true);

    // Sets settings
    setSize (neova_dash::ui::DASHBOARD_WIDTH, neova_dash::ui::DASHBOARD_HEIGHT);
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

    presetSelector->setBounds (area.removeFromBottom (10).withSizeKeepingCentre (160, 30));
    hubComponent->setBounds (area.withSizeKeepingCentre (area.getHeight(), area.getHeight())
                                 .translated (0, -10));
    uploadButton->setBounds (area.withSize (area.getWidth()/7, area.getHeight()/2)
                                 .withSizeKeepingCentre (area.getWidth()/5, HEADER_HEIGHT));
}

//==============================================================================
void DashBoardInterface::mouseDown (const MouseEvent& event)
{
	if (event.mods.isLeftButtonDown())
	{
	}
	else if (event.mods.isRightButtonDown())
	{
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
        && !commandKeyDown)
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

//==============================================================================
ApplicationCommandTarget* DashBoardInterface::getNextCommandTarget()
{
    return findFirstTargetParentComponent();
}

void DashBoardInterface::getAllCommands (Array<CommandID> &commands)
{
    using namespace neova_dash::commands;
    
    int commandIDs[] = {
                            updateDashInterface,
                            updateInterfaceLEDs
    				   };

    commands.addArray (commandIDs, numElementsInArray (commandIDs));
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
        default:
            break;
    }
}

bool DashBoardInterface::perform (const InvocationInfo& info)
{
    using namespace neova_dash::commands;
    DBG ("Front performs : " << getCommandManager().getNameOfCommand (info.commandID));

    switch (info.commandID)
    {
        case updateDashInterface:
            hubComponent->update();
            gesturePanel->update();
            presetSelector->update();
			return true;

        case updateInterfaceLEDs:
            hubComponent->repaintLEDs();
            return true;

        default:
            return false;
    }
}