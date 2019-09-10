/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#include "DashBoardInterface.h"

//==============================================================================
DashBoardInterface::DashBoardInterface (HubConfiguration& data) : hubConfig (data)
{
    // Creates Components
    header = std::make_unique<HeaderComponent>();
    addAndMakeVisible (*header);

    presetSelector = std::make_unique<PresetSelectorComponent> (hubConfig, getCommandManager());
    addAndMakeVisible (*presetSelector);
	
    uploadButton = std::make_unique<UploadButton> (getCommandManager());
    addAndMakeVisible (*uploadButton);

    newGesturePanel = std::make_unique<NewGesturePanel> (hubConfig, getCommandManager());
    addAndMakeVisible (*newGesturePanel);

    gesturePanel = std::make_unique<GesturePanel> (hubConfig, *newGesturePanel, neova_dash::ui::FRAMERATE);
    addAndMakeVisible (*gesturePanel);

    // Top panel properties
    newGesturePanel->hidePanel();
    newGesturePanel->setAlwaysOnTop (true);


    // Sets settings
    setSize (neova_dash::ui::DASHBOARD_WIDTH, neova_dash::ui::DASHBOARD_HEIGHT);
}

DashBoardInterface::~DashBoardInterface()
{
    header = nullptr;
    presetSelector = nullptr;
    newGesturePanel = nullptr;
    //gesturePanel = nullptr;
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

	auto gPanelArea = area.removeFromBottom(area.getHeight() / 2);

    gesturePanel->setBounds (gPanelArea.reduced (0, MARGIN));
    newGesturePanel->setBounds (gPanelArea.reduced (0, MARGIN));

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
            result.setInfo ("Update", "Udpates Interface To Current Hub Configuration", "Interface", 0);
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
            gesturePanel->update();
			return true;
        default:
            return false;
    }
}