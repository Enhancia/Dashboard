/*
  ==============================================================================

    GestureSlots.h
    Created: 6 Sep 2019 8:05:19pm
    Author:  aleva

  ==============================================================================
*/

#pragma once

#include "../../../JuceLibraryCode/JuceHeader.h"
#include "DashCommon.h"

class GestureComponent : public PlumeComponent,
                         private Label::Listener
{
public:
    //==============================================================================
    GestureComponent (HubConfiguration& hubCfg, const int gestNum,
                                                const bool& dragModeReference,
                                                const int& draggedGestureReference,
                                                const int& draggedOverSlotReference);
    ~GestureComponent();

    //==============================================================================
    const String getInfoString() override;
    void update() override;

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

    //==============================================================================
    void editorShown (Label* lbl, TextEditor& ted) override;
    void labelTextChanged (Label* lbl) override;

    //==============================================================================
    void mouseEnter (const MouseEvent &event) override;
    void mouseExit (const MouseEvent &event) override;
    void mouseDrag (const MouseEvent &event) override;

    //==============================================================================
    Gesture& getGesture();
    bool isSelected() const;
    void setSelected (bool);
    void setHighlighted (bool);
    void setSolo (bool);
    void startNameEntry();

    //==============================================================================
    const int id;

private:
    //==============================================================================
    void createLabel();
    void createButton();
    void paintParameterSlotDisplay (Graphics& g, juce::Rectangle<int> area,
                                                 const int numRows,
                                                 const int numColumns,
                                                 const int margin = 0);
    void drawGesturePath (Graphics& g, juce::Rectangle<int> area);

    //==============================================================================
    HubConfiguration& hubConfig;
    ScopedPointer<Label> gestureNameLabel;
    ScopedPointer<PlumeShapeButton> muteButton;
    
    bool on = bool (hubConfig.getGestureData (id).on);
    bool selected = false, highlighted = false, solo = false;

    const bool& dragMode;
    const int& draggedGesture;
    const int& draggedOverSlot;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GestureComponent)
};


class EmptyGestureSlotComponent : public PlumeComponent
{
public:
    //==============================================================================
    EmptyGestureSlotComponent (HubConfiguration& hubCfg, const int slotId,
                                                         const bool& dragModeReference,
                                                         const int& draggedGestureReference,
                                                         const int& draggedOverSlotReference);
    ~EmptyGestureSlotComponent();

    //==============================================================================
    const String getInfoString() override;
    void update() override;

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

    //==============================================================================
    void mouseEnter (const MouseEvent &event) override;
    void mouseExit (const MouseEvent &event) override;
    void mouseDrag (const MouseEvent &event) override;

    const int id;

private:
    //==============================================================================
    bool highlighted = false;
    
    GestureArray& gestureArray;
    const bool& dragMode;
    const int& draggedGesture;
    const int& draggedOverSlot;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EmptyGestureSlotComponent)
};