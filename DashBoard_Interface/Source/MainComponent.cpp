/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent (Colour& colRef) : colourRef (colRef)
{
    setSize (600, 400);
}

MainComponent::~MainComponent()
{
}

//==============================================================================
void MainComponent::paint (Graphics& g)
{
    g.fillAll (colourRef);

    g.setFont (Font (16.0f));
    g.setColour (Colours::white);
    g.drawText ("D'ache borde!", getLocalBounds(), Justification::centred, true);
}

void MainComponent::resized()
{
}

//==============================================================================
void MainComponent::mouseDown (const MouseEvent& event)
{
	if (event.mods.isLeftButtonDown())
	{
		DBG ("Mouse Down left (1)");
		getCommandManager().invokeDirectly (1, true);
	}
	else if (event.mods.isRightButtonDown())
	{
		DBG ("Mouse Down right (2)");
		getCommandManager().invokeDirectly (2, true);
	}
}

//==============================================================================
ApplicationCommandTarget* MainComponent::getNextCommandTarget()
{
    return findFirstTargetParentComponent();
}

void MainComponent::getAllCommands (Array<CommandID> &commands)
{
    int commandIDs[] = { 3 // Repaint
    				   };

    commands.addArray (commandIDs, numElementsInArray (commandIDs));
}

void MainComponent::getCommandInfo (CommandID commandID, ApplicationCommandInfo& result)
{
    switch (commandID)
    {
        case 3:
            result.setInfo ("Repaint", "Repaints Interface", "Interface", 0);
            break;
        default:
            break;
    }
}

bool MainComponent::perform (const InvocationInfo& info)
{
    switch (info.commandID)
    {
        case (3):
			DBG ("Executes 3");
			repaint();
			return true;
        default:
            return false;
    }
}