/*
  ==============================================================================

    GesturePanel.h
    Created: 3 Sep 2019 11:38:56am
    Author:  Enhancia Dev

  ==============================================================================
*/

#pragma once

#include "../../../../JuceLibraryCode/JuceHeader.h"
#include "../../../Common/DashCommon.h"
#include "../../../DataReader/DataReader.h"

#include "../../../Common/HubConfiguration.h"
#include "GestureSlots.h"
#include "../Top/NewGesturePanel.h"
#include "GestureSettingsComponent.h"

//==============================================================================
/*
*/
class GesturePanel    : public Component,
                        public Timer
{
public:
    //==============================================================================
    GesturePanel (HubConfiguration& data, DataReader& reader,
                  NewGesturePanel& newGest, ApplicationCommandManager& manager,
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

    void mouseDown (const MouseEvent &event) override;
    void mouseUp (const MouseEvent &event) override;
    void mouseDrag (const MouseEvent &event) override;
    bool keyPressed (const KeyPress &key) override;

    //==============================================================================
    void initialiseGestureSlots();
    void updateGestureSlots();
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
    void startDragMode (int slotBeingDragged);
    void endDragMode();

    //==============================================================================
    void paintShadows (Graphics& g);
    
    //==============================================================================
    OwnedArray<Component> gestureSlots;
    std::unique_ptr<GestureSettingsComponent> gestureSettings;

    Image vibratoImage = ImageFileFormat::loadFrom (DashData::VIBRATOicon_png,
                                                    DashData::VIBRATOicon_pngSize);
    Image pitchBendImage = ImageFileFormat::loadFrom (DashData::PITCHicon_png,
                                                      DashData::PITCHicon_pngSize);
    Image tiltImage = ImageFileFormat::loadFrom (DashData::TILTicon_png,
                                                 DashData::TILTicon_pngSize);
    Image rollImage = ImageFileFormat::loadFrom (DashData::ROLLicon_png,
                                                 DashData::ROLLicon_pngSize);

    //==============================================================================
    int selectedGesture = 0;
    int freq;

    //==============================================================================
    bool dragMode = false;
    int draggedGestureComponentId = -1;
    int draggedOverSlotId = -1;

    //==============================================================================
    HubConfiguration& hubConfig;
    NewGesturePanel& newGesturePanel;
    ApplicationCommandManager& commandManager;
    DataReader& dataReader;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GesturePanel)
};