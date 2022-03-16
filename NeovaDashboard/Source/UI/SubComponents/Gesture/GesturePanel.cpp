/*
  ==============================================================================

    GesturePanel.cpp
    Created: 3 Sep 2019 11:38:56am
    Author:  Enhancia Dev

  ==============================================================================
*/

#include "GesturePanel.h"

//==============================================================================
GesturePanel::GesturePanel (HubConfiguration& data, DataReader& reader,
                            NewGesturePanel& newGest, ApplicationCommandManager& manager,
                            int freqHz)
                            : freq (freqHz), hubConfig (data),
                              newGesturePanel (newGest),commandManager (manager),
                              dataReader (reader)
{
    setComponentID ("gesturePanel");

    gestureSettings = std::make_unique<GestureSettingsComponent> (static_cast<int>(hubConfig.getGestureData
                                                                      (hubConfig.getSelectedGesture())
                                                                      .type),
                                                                  hubConfig, commandManager, dataReader);
    addAndMakeVisible (*gestureSettings);

    initialiseGestureSlots();

    startTimerHz (freq);
}

GesturePanel::~GesturePanel()
{
    
    stopTimer();
    unselectCurrentGesture();
    newGesturePanel.hidePanel (true);
}

//==============================================================================
void GesturePanel::update()
{
    if (newGesturePanel.isVisible())
    {
        const int lastSlot = newGesturePanel.getLastSelectedSlot();

        updateSlotIfNeeded (lastSlot);
        newGesturePanel.hidePanel();

        //renameGestureInSlot (lastSlot); OLD, no rename anymore
    }

    stopTimer();
    updateGestureSlots();

    gestureSettings.reset (new GestureSettingsComponent (hubConfig.getSelectedGesture(), hubConfig,
                                                                                         commandManager,
                                                                                         dataReader));
    addAndMakeVisible (*gestureSettings);
    
    resized();
    /*
    if (draggedGestureComponentId != -1 && draggedOverSlotId != draggedGestureComponentId)
    {
        repaint (gestureSlots[draggedGestureComponentId]->getBounds().expanded (13));
        repaint (gestureSlots[draggedOverSlotId]->getBounds().expanded (13));
    }*/
    startTimerHz (freq);
}

//==============================================================================
void GesturePanel::paint (Graphics& g)
{
    paintShadows (g);
}


void GesturePanel::paintShadows (Graphics& g) const
{
    Path shadowPath;

    {
        // Gesture Settings
        const auto gestureSettingsArea = gestureSettings->getBounds();

        shadowPath.addRoundedRectangle (gestureSettingsArea.toFloat(), 10.0f);
    }
    {
        // Gesture Components
        auto gestureSettingsArea = gestureSettings->getBounds();

        for (int slot=0; slot < neova_dash::gesture::NUM_GEST; slot++)
        {
            if (const auto* gestureComp = dynamic_cast<GestureComponent*> (gestureSlots[slot]))
            {
                shadowPath.addRoundedRectangle (gestureComp->getBounds().toFloat(), 10.0f);
            }
        }
    }

    const DropShadow shadow (Colour (0x40000000), 10, {2, 3});
    shadow.drawForPath (g, shadowPath);
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
            if (const auto* gestureComponent = dynamic_cast<GestureComponent*> (event.eventComponent))
            {
                createMenuForGestureId (gestureComponent->id);
            }
        }
    }
}

void GesturePanel::mouseDrag (const MouseEvent& event)
{
    const auto relativeEvent = event.getEventRelativeTo (this);

    if (relativeEvent.mods.isLeftButtonDown())
    {
        if (const auto* gestureComponent = dynamic_cast<GestureComponent*> (relativeEvent.originalComponent))
        {
            if (!dragMode)
            {
                startDragMode (gestureComponent->id);
            }

            const int formerDraggedOverId = draggedOverSlotId;

            if (Component* componentUnderMouse = getComponentAt (relativeEvent.getPosition()))
            {
                if (auto* otherGesture = dynamic_cast<GestureComponent*> (componentUnderMouse))
                {
					if (otherGesture->id != draggedOverSlotId && otherGesture->id != draggedGestureComponentId)
					{
						draggedOverSlotId = otherGesture->id;
						otherGesture->repaint();
					}
				}
                else if (auto* emptySlot = dynamic_cast<EmptyGestureSlotComponent*> (componentUnderMouse))
                {
                    if (emptySlot->id != draggedOverSlotId)
                    {
                        draggedOverSlotId = emptySlot->id;
    					emptySlot->repaint();
                    }
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
    if (const auto* gestureComponent = dynamic_cast<GestureComponent*> (event.eventComponent))
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
                if (const auto* slotUnderMouse = dynamic_cast<EmptyGestureSlotComponent*> (componentUnderMouse))
                {
                    moveGestureToId (gestureComponent->id, slotUnderMouse->id);
                }
                else if (const auto* gestureComponentUnderMouse = dynamic_cast<GestureComponent*> (componentUnderMouse))
                {
                    swapGestures (gestureComponent->id, gestureComponentUnderMouse->id);
                }
            }
        }
    }

    else if (const auto* emptySlot = dynamic_cast<EmptyGestureSlotComponent*> (event.eventComponent))
    {
        newGesturePanel.showPanelForGestureID (emptySlot->id);
    }
}

void GesturePanel::handleLeftClickDrag (const MouseEvent&)
{
}

void GesturePanel::handleKeyPress (const KeyPress &key)
{
    if (key == neova_dash::keyboard_shortcut::selectGestureRight)
    {
        selectGestureExclusive (hubConfig.selectGestureRight());
    }
    else if (key == neova_dash::keyboard_shortcut::selectGestureLeft)
    {
        selectGestureExclusive (hubConfig.selectGestureLeft());
    }
    else if (key == neova_dash::keyboard_shortcut::selectGestureUp)
    {
        selectGestureExclusive (hubConfig.selectGestureUp());
    }
    else if (key == neova_dash::keyboard_shortcut::selectGestureDown)
    {
        selectGestureExclusive (hubConfig.selectGestureDown());
    }
}

void GesturePanel::initialiseGestureSlots()
{
    if (!gestureSlots.isEmpty()) gestureSlots.clear();

    for (int i=0; i<neova_dash::gesture::NUM_GEST; i++)
    {
        if (hubConfig.getGestureData (i).type != static_cast<uint8>(neova_dash::gesture::none))
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

        if (hubConfig.getGestureData (i).type != static_cast<uint8>(neova_dash::gesture::none))
        {
            if (auto* emptySlot = dynamic_cast<EmptyGestureSlotComponent*> (gestureSlots[i]))
            {
                repaint (gestureSlots[i]->getBounds().expanded (13));
            }

            gestureSlots.set (i, new GestureComponent (hubConfig, commandManager, i,
                                                       dragMode, draggedGestureComponentId, draggedOverSlotId));
            
        }
        else
        {
            if (auto* gestureComp = dynamic_cast<GestureComponent*> (gestureSlots[i]))
            {
                repaint (gestureSlots[i]->getBounds().expanded (13));
            }

            gestureSlots.set (i, new EmptyGestureSlotComponent (hubConfig, i,
                                                                dragMode, draggedGestureComponentId,
                                                                          draggedOverSlotId));
        }

        addAndMakeVisible (gestureSlots[i]);
        gestureSlots[i]->addMouseListener (this, false);
    }
}

void GesturePanel::resizeSlotsAndTrimAreaAccordingly (juce::Rectangle<int>& area, int marginX, int marginY) const
{
    using namespace neova_dash::ui;
    if (gestureSlots.size() == 0) return;

    // There should be an even number of gestures for the grid of gesture slots to make sense...
    jassert (neova_dash::gesture::NUM_GEST%2 == 0);
    const int numRows = neova_dash::gesture::NUM_GEST/2;

    const int tempWidth = area.getWidth()/4;
    area.reduce (tempWidth/4, 0);

    auto column1 = area.removeFromLeft (tempWidth);
    auto column2 = area.removeFromRight (tempWidth);

    const int slotHeight = area.getHeight()/numRows;

    // sets bounds depending on the value in the array
    for (int i=0; i<gestureSlots.size(); i++)
    {
        gestureSlots[i]->setBounds (i < numRows ? column1.removeFromTop (slotHeight)
														 .reduced (marginX, marginY)
                                                : column2.removeFromTop (slotHeight)
														 .reduced (marginX, marginY));
    }
}

void GesturePanel::updateSlotIfNeeded (int slotToCheck)
{
    // 1st check, if a gesture was deleted (slot is GestureComponent but should be empty)
    if (auto* gestureComponent = dynamic_cast<GestureComponent*> (gestureSlots[slotToCheck]))
    {
        if (hubConfig.getGestureData (slotToCheck).type == static_cast<uint8>(neova_dash::gesture::none))
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
        if (hubConfig.getGestureData (slotToCheck).type != static_cast<uint8>(neova_dash::gesture::none))
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
    const bool mustChangeSelection = hubConfig.getSelectedGesture() == idToMoveFrom;
    
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
    const bool mustChangeSelection = hubConfig.getSelectedGesture() == firstId
        || hubConfig.getSelectedGesture() == secondId;
    int idToSelect = 0;

    if (mustChangeSelection)
    {
        idToSelect = hubConfig.getSelectedGesture() == firstId ? secondId : firstId;
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

void GesturePanel::renameGestureInSlot (int slotNumber) const
{
    if (auto* gestureComponent = dynamic_cast<GestureComponent*> (gestureSlots[slotNumber]))
    {
        DBG ("Renaming?? We don't do that here..");
    }
    else
    {
        /* Tried to rename a gesture, but the selected slot was empty...
        */
        jassertfalse;
    }
}

void GesturePanel::removeGestureAndGestureComponent (int gestureId)
{
    if (gestureId < 0 || gestureId > neova_dash::gesture::NUM_GEST) return;
    stopTimer();

    hubConfig.setDefaultGestureValues (gestureId, neova_dash::gesture::none);

    if (gestureId == hubConfig.getSelectedGesture())
    {
        if (dragMode) endDragMode();

        hubConfig.selectFirstExistingGesture();
        gestureSettings.reset (new GestureSettingsComponent (hubConfig.getSelectedGesture(),
                                                             hubConfig, commandManager, dataReader));
        addAndMakeVisible (*gestureSettings);
        resized();
    }

    commandManager.invokeDirectly (neova_dash::commands::updateInterfaceLEDs, true);
    updateGestureSlots();

    if (!isTimerRunning()) startTimerHz (freq);
    update();
}

bool GesturePanel::hasSelectedGesture() const
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
			                                             commandManager, dataReader));
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
            // Dashboard tried to unselect an already unselected gesture...
            jassertfalse;
            return;
        }

        gestureComponentToUnselect->setSelected (false);

        hubConfig.setSelectedGesture (-1);
        gestureSettings.reset (new GestureSettingsComponent (neova_dash::gesture::NUM_GEST + 1,
			                                                 hubConfig, commandManager, dataReader));
        addAndMakeVisible (*gestureSettings);
        resized();

        return;
    }

    /* Hitting this assert means that the Dashboard tried to unselect a gesture, but the slot
       it tried to access is an empty slot. That might either mean that an emptySlot failed
       to be updated to a GestureComponent, or that the Dashboard failed to keep track of the actual
       selected Gesture.
    */
    jassertfalse;
}

void GesturePanel::createMenuForGestureId (int id)
{
    PopupMenu gestureMenu;

    gestureMenu.addItem (1, "Duplicate", hubConfig.canDuplicate());
    gestureMenu.addItem (2, "Delete", true);
    gestureMenu.addItem (3, hubConfig.getGestureData (id).on == 1 ? "Mute" : "Unmute", true);
    
    gestureMenu.showMenuAsync (PopupMenu::Options().withParentComponent (getParentComponent())
                                                                .withMaximumNumColumns (3)
                                                                .withPreferredPopupDirection (PopupMenu::Options::PopupDirection::downwards)
                                                                .withStandardItemHeight (20),
                               ModalCallbackFunction::forComponent (menuCallback, this, id));
}

void GesturePanel::menuCallback (int result, GesturePanel* gPanel, int id)
{
    if (gPanel != nullptr)
    {
        gPanel->handleMenuResult (id, result);
    }
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

        case 3: // Mute gesture
            hubConfig.setUint8Value (gestureId, HubConfiguration::on, hubConfig.getGestureData (gestureId).on == 1 ? 0 : 1);
            commandManager.invokeDirectly (neova_dash::commands::updateDashInterface, true);
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
