/*
  ==============================================================================

    GesturePanel.cpp
    Created: 3 Sep 2019 11:38:56am
    Author:  Enhancia Dev

  ==============================================================================
*/

#include "GesturePanel.h"

//==============================================================================
GesturePanel::GesturePanel (HubConfiguration& data, NewGesturePanel& newGest, ApplicationCommandManager& manager,
                            int freqHz)
                            : hubConfig (data), newGesturePanel (newGest), commandManager (manager),
                              freq (freqHz)
{
    TRACE_IN;

    setComponentID ("gesturePanel");
    setWantsKeyboardFocus (true);

    gestureSettings = std::make_unique<GestureSettingsComponent> (int (hubConfig.getGestureData
                                                                    (hubConfig.getSelectedGesture())
                                                                                     .type),
                                                                  hubConfig, commandManager);
    addAndMakeVisible (*gestureSettings);

    initialiseGestureSlots();

    startTimerHz (freq);
}

GesturePanel::~GesturePanel()
{
    TRACE_IN;

    stopTimer();
    unselectCurrentGesture();
    newGesturePanel.hidePanel (true);
}

//==============================================================================
void GesturePanel::update()
{
    if (newGesturePanel.isVisible())
    {
        int lastSlot = newGesturePanel.getLastSelectedSlot();

        updateSlotIfNeeded (lastSlot);
        newGesturePanel.hidePanel();

        //renameGestureInSlot (lastSlot); OLD, no rename anymore
    }

    stopTimer();
    initialiseGestureSlots();

    gestureSettings.reset (new GestureSettingsComponent (hubConfig.getSelectedGesture(), hubConfig, commandManager));
    addAndMakeVisible (*gestureSettings);
    
    resized();
    startTimerHz (freq);
}

//==============================================================================
void GesturePanel::paint (Graphics&)
{
}

void GesturePanel::paintOverChildren (Graphics&)
{
    /* TODO paint the component snapshot during a drag.
       Get a snapshot of the component being dragged (might wanna cache it so it is not 
       recalculated for every repaint), and display it centred at the current mouse position.
    if (dragMode)
    {
        Image gestureComponentImage;
        // here get the image

        //g.drawImage (gestureComponentImage, Rectangle_that_is_the_size_of_the_image);
    }
    */
}

void GesturePanel::resized()
{
    using namespace neova_dash::ui;

    auto area = getLocalBounds();
    resizeSlotsAndTrimAreaAccordingly (area, 4*MARGIN, 2*MARGIN);
    
    gestureSettings->setBounds (area.reduced (MARGIN, 2*MARGIN));
}

void GesturePanel::timerCallback()
{
    if (gestureSettings != nullptr)
    {
        gestureSettings->updateDisplay();
    }
}

void GesturePanel::mouseDown (const MouseEvent &event)
{
    if (auto* gestureComponent = dynamic_cast<GestureComponent*> (event.eventComponent))
    {
        if (!gestureComponent->isSelected())
        {
            selectGestureExclusive (*gestureComponent);
        }
    }
}

void GesturePanel::mouseUp (const MouseEvent &event)
{
    if (event.eventComponent != this)
    {
        if (event.mods.isLeftButtonDown())
        {
            if (dragMode) endDragMode();

			handleLeftClickUp (event);
        }

        else if (event.mods.isPopupMenu())
        {
            if (auto* gestureComponent = dynamic_cast<GestureComponent*> (event.eventComponent))
            {
                createMenuForGestureId (gestureComponent->id);
            }
        }
    }
}

void GesturePanel::mouseDrag (const MouseEvent& event)
{
    auto relativeEvent = event.getEventRelativeTo (this);

    if (relativeEvent.mods.isLeftButtonDown())
    {
        if (auto* gestureComponent = dynamic_cast<GestureComponent*> (relativeEvent.originalComponent))
        {
            if (!dragMode)
            {
                startDragMode (gestureComponent->id);
            }

			int formerDraggedOverId = draggedOverSlotId;

            if (Component* componentUnderMouse = getComponentAt (relativeEvent.getPosition()))
            {
                if (auto* otherGesture = dynamic_cast<GestureComponent*> (componentUnderMouse))
                {
					if (otherGesture->id != draggedGestureComponentId)
					{
						draggedOverSlotId = otherGesture->id;
						otherGesture->repaint();
					}
				}
                else if (auto* emptySlot = dynamic_cast<EmptyGestureSlotComponent*> (componentUnderMouse))
                {
                    draggedOverSlotId = emptySlot->id;
					emptySlot->repaint();
                }
				else
				{
					draggedOverSlotId = -1;
				}
            }
            else
            {
                draggedOverSlotId = -1;
            }

			if (formerDraggedOverId != -1 && formerDraggedOverId != draggedOverSlotId)
				gestureSlots[formerDraggedOverId]->repaint();

            /* TODO repaint here for the component snapshot.
               For optimal smoothness, get the last coordinates here, and repaint around
               both the current and last mouse position, in a square that is the size
               of the snapshot.
            //repaint();
            */
        }
    }
}

void GesturePanel::handleLeftClickUp (const MouseEvent& event)
{
    if (auto* gestureComponent = dynamic_cast<GestureComponent*> (event.eventComponent))
    {
        /*
        // Mouse didn't leave the component it clicked in the first place
        if (getComponentAt (event.getEventRelativeTo (this).getPosition()) == event.eventComponent)
        {
            if (!gestureComponent->isSelected())
            {
                selectGestureExclusive (*gestureComponent);
            }
            }
        }
        else // Mouse was dragged to another component*/
        {
            if (Component* componentUnderMouse = getComponentAt (event.getEventRelativeTo (this).getPosition()))
            {
                if (auto* slotUnderMouse = dynamic_cast<EmptyGestureSlotComponent*> (componentUnderMouse))
                {
                    moveGestureToId (gestureComponent->id, slotUnderMouse->id);
                }
                else if (auto* gestureComponentUnderMouse = dynamic_cast<GestureComponent*> (componentUnderMouse))
                {
                    swapGestures (gestureComponent->id, gestureComponentUnderMouse->id);
                }
            }
        }
    }

    else if (auto* emptySlot = dynamic_cast<EmptyGestureSlotComponent*> (event.eventComponent))
    {
        newGesturePanel.showPanelForGestureID (emptySlot->id);
    }
}

void GesturePanel::handleLeftClickDrag (const MouseEvent&)
{
}

bool GesturePanel::keyPressed (const KeyPress &key)
{
    if (hasSelectedGesture() && key.isValid())
    {
        if (key.getKeyCode() == KeyPress::deleteKey || key.getKeyCode() == KeyPress::backspaceKey)
        {
            removeGestureAndGestureComponent (hubConfig.getSelectedGesture());
        }
        /*
        else if (key.getTextCharacter() == 'r')
        {
			if (key.getModifiers().isAltDown())
			{
				renameGestureInSlot (hubConfig.getSelectedGesture());
			}
        }*/
    }

	return false;
}

void GesturePanel::initialiseGestureSlots()
{
    if (!gestureSlots.isEmpty()) gestureSlots.clear();

    for (int i=0; i<neova_dash::gesture::NUM_GEST; i++)
    {
        if (hubConfig.getGestureData (i).type != uint8 (neova_dash::gesture::none))
        {
            gestureSlots.add (new GestureComponent (hubConfig, commandManager, i,
                                                    dragMode, draggedGestureComponentId, draggedOverSlotId));

            if (hubConfig.getSelectedGesture() != -1 && i == hubConfig.getSelectedGesture())
            {
                dynamic_cast<GestureComponent*> (gestureSlots.getLast())->setSelected (true);
            }
        }
        else
        {
            gestureSlots.add (new EmptyGestureSlotComponent (hubConfig, i,
                                                             dragMode, draggedGestureComponentId,
                                                                       draggedOverSlotId));
        }

        addAndMakeVisible (gestureSlots.getLast());
        gestureSlots.getLast()->addMouseListener (this, false);
    }
}

void GesturePanel::updateGestureSlots()
{
    for (int i=0; i<neova_dash::gesture::NUM_GEST; i++)
    {
		gestureSlots[i]->removeMouseListener(this);

        if (hubConfig.getGestureData (i).type != uint8 (neova_dash::gesture::none))
        {
            gestureSlots.set (i, new GestureComponent (hubConfig, commandManager, i,
                                                       dragMode, draggedGestureComponentId, draggedOverSlotId));
        }
        else
        {
            gestureSlots.set (i, new EmptyGestureSlotComponent (hubConfig, i,
                                                                dragMode, draggedGestureComponentId,
                                                                          draggedOverSlotId));
        }

        addAndMakeVisible (gestureSlots[i]);
        gestureSlots[i]->addMouseListener (this, false);
    }
}

void GesturePanel::resizeSlotsAndTrimAreaAccordingly (juce::Rectangle<int>& area, int marginX, int marginY)
{
    using namespace neova_dash::ui;
    if (neova_dash::gesture::NUM_GEST == 0 || gestureSlots.size() == 0) return;

    // There should be an even number of gestures for the grid of gesture slots to make sense...
    jassert (neova_dash::gesture::NUM_GEST%2 == 0);
    int numRows = neova_dash::gesture::NUM_GEST/2;

    int tempWidth = area.getWidth()/4;
    area.reduce (tempWidth/4, 0);

    auto column1 = area.removeFromLeft (tempWidth);
    auto column2 = area.removeFromRight (tempWidth);

    int slotHeight = area.getHeight()/numRows;

    // sets bounds depending on the value in the array
    for (int i=0; i<gestureSlots.size(); i++)
    {
        gestureSlots[i]->setBounds (i < numRows ? column1.removeFromTop (slotHeight)
														 .reduced (marginX, marginY)
                                                : column2.removeFromBottom (slotHeight)
														 .reduced (marginX, marginY));
    }
}

void GesturePanel::updateSlotIfNeeded (int slotToCheck)
{
    // 1st check, if a gesture was deleted (slot is GestureComponent but should be empty)
    if (auto* gestureComponent = dynamic_cast<GestureComponent*> (gestureSlots[slotToCheck]))
    {
        if (hubConfig.getGestureData (slotToCheck).type == uint8 (neova_dash::gesture::none))
        {
            gestureSlots.set (slotToCheck, new EmptyGestureSlotComponent (hubConfig,
                                                                          slotToCheck,
                                                                          dragMode,
                                                                          draggedGestureComponentId,
                                                                          draggedOverSlotId),
                                           true);

            addAndMakeVisible (gestureSlots[slotToCheck]);
            gestureSlots[slotToCheck]->addMouseListener (this, false);
            resized();
            //repaint();
        }
    }
    // 2nd check, if a gesture was created (slot is empty but should be a gestureComponent)
    else if (auto* emptySlot = dynamic_cast<EmptyGestureSlotComponent*> (gestureSlots[slotToCheck]))
    {
        if (hubConfig.getGestureData (slotToCheck).type != uint8 (neova_dash::gesture::none))
        {
            gestureSlots.set (slotToCheck, new GestureComponent (hubConfig,
                                                                 commandManager,
                                                                 slotToCheck,
                                                                 dragMode,
                                                                 draggedGestureComponentId,
                                                                 draggedOverSlotId),
                                           true);

            addAndMakeVisible (gestureSlots[slotToCheck]);
            gestureSlots[slotToCheck]->addMouseListener (this, false);

			if (newGesturePanel.getLastSelectedSlot() == slotToCheck)
			{
				newGesturePanel.hidePanel(true);
				selectGestureExclusive (*dynamic_cast<GestureComponent*> (gestureSlots[slotToCheck]));
			}

            resized();
            //repaint();
        }
    }
}


void GesturePanel::moveGestureToId (int idToMoveFrom, int idToMoveTo)
{
    bool mustChangeSelection = (hubConfig.getSelectedGesture() == idToMoveFrom);
    
    if (mustChangeSelection) unselectCurrentGesture();

    hubConfig.moveGestureToId (idToMoveFrom, idToMoveTo);
    commandManager.invokeDirectly (neova_dash::commands::updateInterfaceLEDs, true);
	update();

    if (mustChangeSelection)
    {
        selectGestureExclusive (idToMoveTo);
    }
}

void GesturePanel::swapGestures (int firstId, int secondId)
{
    bool mustChangeSelection = (hubConfig.getSelectedGesture() == firstId
                                || hubConfig.getSelectedGesture() == secondId);
    int idToSelect;

    if (mustChangeSelection)
    {
        idToSelect = (hubConfig.getSelectedGesture() == firstId ? secondId : firstId);
        unselectCurrentGesture();
    }

    // Deletes GestureComponents for the 2 slots
    gestureSlots.set (firstId, new EmptyGestureSlotComponent (hubConfig, firstId,
                                                                         dragMode,
                                                                         draggedGestureComponentId,
                                                                         draggedOverSlotId), true);
    gestureSlots.set (secondId, new EmptyGestureSlotComponent (hubConfig, secondId,
                                                                          dragMode,
                                                                          draggedGestureComponentId,
                                                                          draggedOverSlotId), true);

    hubConfig.swapGestures (firstId, secondId);
    update();
    
    if (mustChangeSelection) selectGestureExclusive (idToSelect);
}

void GesturePanel::renameGestureInSlot (int slotNumber)
{
    if (auto* gestureComponent = dynamic_cast<GestureComponent*> (gestureSlots[slotNumber]))
    {
        DBG ("Renaming?? We don't do that here..");
        //gestureComponent->startNameEntry();
    }
    else
    {
        /* Tried to rename a gesture, but the selected dlot was empty...
        */
        jassertfalse;
    }
}

void GesturePanel::removeGestureAndGestureComponent (int gestureId)
{
    if (gestureId < 0 || gestureId > neova_dash::gesture::NUM_GEST) return;
    stopTimer();

    if (gestureId == hubConfig.getSelectedGesture())
    {
        unselectCurrentGesture();
        gestureSettings.reset (new GestureSettingsComponent (neova_dash::gesture::NUM_GEST + 1,
                                                             hubConfig, commandManager));
        addAndMakeVisible (*gestureSettings);
        resized();
    }

    hubConfig.setDefaultGestureValues (gestureId, neova_dash::gesture::none);
    commandManager.invokeDirectly (neova_dash::commands::updateInterfaceLEDs, true);
    updateSlotIfNeeded (gestureId);

    if (!isTimerRunning()) startTimerHz (freq);
}

bool GesturePanel::hasSelectedGesture()
{
    return hubConfig.getSelectedGesture() != -1;
}

void GesturePanel::switchGestureSelectionState (GestureComponent& gestureComponentToSwitch)
{
    if (gestureComponentToSwitch.isSelected())
    {
        gestureComponentToSwitch.setSelected (false);
        hubConfig.setSelectedGesture (-1);
    }
    else
    {
        selectGestureExclusive (gestureComponentToSwitch);
    }
}

void GesturePanel::selectGestureExclusive (GestureComponent& gestureComponentToSelect)
{
    gestureComponentToSelect.setSelected (true);

    for (auto* slot : gestureSlots)
    {
        if (auto* gestureComponent = dynamic_cast<GestureComponent*> (slot))
        {
            if (gestureComponent != &gestureComponentToSelect && gestureComponent->isSelected())
            {
                gestureComponent->setSelected (false);
            }
        }
    }

    gestureSettings.reset (new GestureSettingsComponent (gestureComponentToSelect.id, hubConfig,
			                                             commandManager));
    addAndMakeVisible (*gestureSettings);
    hubConfig.setSelectedGesture (gestureComponentToSelect.id);
    resized();
}

void GesturePanel::selectGestureExclusive (const int idToSelect)
{
    if (idToSelect < 0 || idToSelect >= gestureSlots.size()) return;

    if (auto* gestureComponent = dynamic_cast<GestureComponent*> (gestureSlots[idToSelect]))
    {
        selectGestureExclusive (*gestureComponent);
    }
}

void GesturePanel::unselectCurrentGesture()
{
	// Can't unselect if nothing is selected..
	if (hubConfig.getSelectedGesture() == -1) return;

    if (auto* gestureComponentToUnselect = dynamic_cast<GestureComponent*> (gestureSlots[hubConfig.getSelectedGesture()]))
    {
        if (!gestureComponentToUnselect->isSelected())
        {
            // Plume tried to unselect an already unselected gesture...
            jassertfalse;
            return;
        }

        gestureComponentToUnselect->setSelected (false);

        hubConfig.setSelectedGesture (-1);
        gestureSettings.reset (new GestureSettingsComponent (neova_dash::gesture::NUM_GEST + 1,
			                                                 hubConfig, commandManager));
        addAndMakeVisible (*gestureSettings);
        resized();

        return;
    }

    /* Hitting this assert means that Plume tried to unselect a gesture, but the slot
       it tried to access is an empty slot. That might either mean that an emptySlot failed
       to be updated to a GestureComponent, or that Plume failed to keep track of the actual
       selected Gesture.
    */
    jassertfalse;
}

void GesturePanel::createMenuForGestureId (int id)
{
    PopupMenu gestureMenu;

    gestureMenu.addItem (1, "Duplicate", hubConfig.canDuplicate());
    gestureMenu.addItem (2, "Delete", true);
    
    handleMenuResult (id,
                      gestureMenu.showMenu (PopupMenu::Options().withParentComponent (getParentComponent())
                                                                .withMaximumNumColumns (3)
                                                                .withPreferredPopupDirection (PopupMenu::Options::PopupDirection::downwards)
                                                                .withStandardItemHeight (20)));
}

void GesturePanel::handleMenuResult (int gestureId, const int menuResult)
{
    switch (menuResult)
    {
        case 0: // No choice
            break;
            
        case 1: // Duplicate
            hubConfig.duplicateGesture (gestureId);
            update();
            commandManager.invokeDirectly (neova_dash::commands::updateInterfaceLEDs, true);
            selectGestureExclusive (gestureId);
            break;

        case 2: // Delete gesture
            removeGestureAndGestureComponent (gestureId);
            update();
    }
}

void GesturePanel::startDragMode (int slotBeingDragged)
{
    dragMode = true;
    draggedGestureComponentId = slotBeingDragged;
    draggedOverSlotId = -1;

    for (auto* slot : gestureSlots)
    {
        slot->repaint();
    }
}

void GesturePanel::endDragMode()
{
    dragMode = false;
    draggedGestureComponentId = -1;
    draggedOverSlotId = -1;

    for (auto* slot : gestureSlots)
    {
        slot->repaint();
    }
}