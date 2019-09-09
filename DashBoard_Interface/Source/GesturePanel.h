/*
  ==============================================================================

    GesturePanel.h
    Created: 3 Sep 2019 11:38:56am
    Author:  Enhancia Dev

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "DashCommon.h"
#include "HubConfiguration.h"
#include "GestureSlots.h"
//#include "NewGesturePanel"

//==============================================================================
/*
*/
class GesturePanel    : public Component,
                        public Timer,
                        private Button::Listener,
                        private AudioProcessorValueTreeState::Listener
{
public:
    //==============================================================================
    GesturePanel (HubConfiguration& data, /*NewGesturePanel& newGest,*/
                  int freqHz = 60);
    ~GesturePanel();

    //==============================================================================
    void update();

    //==============================================================================
    // Component
    void paint (Graphics&) override;
    void paintOverChildren (Graphics&) override;
    void resized() override;
    
    //==============================================================================
    // Callbacks
    void timerCallback() override;
    void buttonClicked (Button*) override;
    void parameterChanged (const String &parameterID, float newValue) override;

    void mouseUp (const MouseEvent &event) override;
    void mouseDrag (const MouseEvent &event) override;
    bool keyPressed (const KeyPress &key) override;

    //==============================================================================
    void removeListenerForAllParameters();
    void addParameterListenerForGestureId (const int id);
    void removeParameterListenerForGestureId (const int id);

    //==============================================================================
    void initialiseGestureSlots();
    void resizeSlotsAndTrimAreaAccordingly (juce::Rectangle<int>& area, int marginX = neova_dash::ui::MARGIN,
                                                                        int marginY = neova_dash::ui::MARGIN);
    void removeGestureAndGestureComponent (int gestureId);
    bool hasSelectedGesture();
    void renameGestureInSlot (int slotNumber);

private:
    //==============================================================================
    void switchGestureSelectionState (GestureComponent& gestureComponentToSwitch);
    void selectGestureExclusive (const int idToSelect);
    void selectGestureExclusive (GestureComponent& gestureComponentToSelect);
    void unselectCurrentGesture ();
    void updateSlotIfNeeded (int slotToCheck);
    void moveGestureToId (int idToMoveFrom, int idToMoveTo);
    void swapGestures (int firstId, int secondId);

    //==============================================================================
    void createMenuForGestureId (int id);
    void handleMenuResult (int gestureId, const int menuResult);
    void handleLeftClickUp (const MouseEvent &event);
    void handleLeftClickDrag (const MouseEvent &event);

    //==============================================================================
    void setSettingsVisible (bool shouldBeVisible);

    //==============================================================================
    void startDragMode (int slotBeingDragged);
    void endDragMode();

    //==============================================================================
    OwnedArray<Component> gestureSlots;
    //ScopedPointer<GestureSettingsComponent> gestureSettings;

    //==============================================================================
    int selectedGesture = -1;
    int freq;
    bool settingsVisible = true;

    //==============================================================================
    bool dragMode = false;
    int draggedGestureComponentId = -1;
    int draggedOverSlotId = -1;

    //==============================================================================
    HubConfiguration& hubConfig;
    NewGesturePanel& newGesturePanel;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GesturePanel)
};