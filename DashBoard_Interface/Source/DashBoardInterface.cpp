/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#include "DashBoardInterface.h"

//==============================================================================
DashBoardInterface::DashBoardInterface (ConfigData& data) : configData (data)
{
    // Creates Components
    header = std::make_unique<HeaderComponent>();
    addAndMakeVisible (*header);

    presetSelector = std::make_unique<PresetSelectorComponent> (configData);
    addAndMakeVisible (*presetSelector);

    gesturePanel = std::make_unique<GesturePanel> (configData);
    addAndMakeVisible (*gesturePanel);

    uploadButton = std::make_unique<UploadButton> (getCommandManager());
    addAndMakeVisible (*uploadButton);

    // Sets settings
    setSize (neova_dash::ui::DASHBOARD_WIDTH, neova_dash::ui::DASHBOARD_HEIGHT);
}

DashBoardInterface::~DashBoardInterface()
{
    header = nullptr;
    presetSelector = nullptr;
    gesturePanel = nullptr;
    uploadButton = nullptr;
}

//==============================================================================
void DashBoardInterface::paint (Graphics& g)
{
	/*
    ColourGradient gradFill (neova_dash::colour::dashboardBackground,
                             getLocalBounds().getCentreX(),
                             getHeight()/4,
                             neova_dash::colour::dashboardBackground2,
                             getLocalBounds().getCentreX(),
                             getHeight(),
                             true);
    gradFill.addColour (0.5, neova_dash::colour::dashboardBackground);
    g.setGradientFill (gradFill);*/

    g.setColour (neova_dash::colour::dashboardBackground2);
    g.fillRect (getLocalBounds());
}

void DashBoardInterface::resized()
{
    using namespace neova_dash::ui;

    auto area = getLocalBounds();

    gesturePanel->setBounds (area.removeFromBottom (area.getHeight()/2).reduced (2*MARGIN));
    header->setBounds (area.removeFromTop (HEADER_HEIGHT).reduced (MARGIN_SMALL, MARGIN));

    presetSelector->setBounds (area.withSizeKeepingCentre (area.getHeight(), area.getHeight())
                                  .reduced (2*MARGIN));
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

//==============================================================================
ApplicationCommandTarget* DashBoardInterface::getNextCommandTarget()
{
    return findFirstTargetParentComponent();
}

void DashBoardInterface::getAllCommands (Array<CommandID> &commands)
{
    using namespace neova_dash::commands;
    
    int commandIDs[] = { updateDashInterface,
    				   };

    commands.addArray (commandIDs, numElementsInArray (commandIDs));
}

void DashBoardInterface::getCommandInfo (CommandID commandID, ApplicationCommandInfo& result)
{
    using namespace neova_dash::commands;

    switch (commandID)
    {
        case updateDashInterface:
            result.setInfo ("Repaint", "Repaints Interface", "Interface", 0);
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
			repaint();
			return true;
        default:
            return false;
    }
}