/*
  ==============================================================================

    TwoRangeTuner.h
    Created: 13 Sep 2019 3:57:41pm
    Author:  Enhancia Dev

  ==============================================================================
*/

#pragma once

#include "../../../../../JuceLibraryCode/JuceHeader.h"
#include "../../../../Common/DashCommon.h"
#include "../../../../Common/HubConfiguration.h"
#include "../../../LookAndFeel/DashBoardLookAndFeel.h"
#include "Tuner.h"

class TwoRangeTuner:    public Tuner,
                        private Slider::Listener,
                        private Label::Listener,
                        private Button::Listener
{
public:
    enum DraggableObject
    {
        none = -1,
        leftLowThumb,
        leftHighThumb,
        rightLowThumb,
        rightHighThumb,
        middleAreaLeft,
        middleAreaRight
    };

    //==============================================================================
    TwoRangeTuner (HubConfiguration& config, const int gestureId,
				   const float& val, const NormalisableRange<float> gestureRange,
                   const Range<float> paramMax, const String unit = "");
    ~TwoRangeTuner();
    
    //==============================================================================
    void paint (Graphics& g) override;
    void resized() override;
    
    void updateComponents() override;
    void updateComponents (DraggableObject thumbThatShouldUpdate);
    void updateDisplay() override;

    void setColour (const Colour newColour) override;
    void updateColour() override;
    
    //==============================================================================
    void labelTextChanged (Label* lbl) override;
    void editorShown (Label* lbl, TextEditor& ted) override;
    void editorHidden (Label* lbl, TextEditor& ted) override;
    void sliderValueChanged (Slider* sldr) override;
    void buttonClicked (Button* bttn) override;
    
    //==============================================================================
    void mouseDown (const MouseEvent& e) override;
    void mouseDrag (const MouseEvent& e) override;
    void mouseUp (const MouseEvent& e) override;
    MouseCursor getMouseCursor() override;

    //==============================================================================
    void setAngles (float startAngle, float endAngle);

    //==============================================================================
    const int id;

private:
    //==============================================================================
    void createSliders();
    void resizeSliders();
    void createLabels();
    void createButtons();
    void resizeButtons();
    
    //==============================================================================
    void setRangeLeftLow (float value, bool uploadToHub = true);
    void setRangeLeftHigh (float value, bool uploadToHub = true);
    void setRangeRightLow (float value, bool uploadToHub = true);
    void setRangeRightHigh (float value, bool uploadToHub = true);
    
    float getRangeLeftLow();
    float getRangeLeftHigh();
    float getRangeRightLow();
    float getRangeRightHigh();
    
    //==============================================================================
    double getAngleFromMouseEventRadians (const MouseEvent& e);
    double getThumbAngleRadians (const DraggableObject thumb);

    DraggableObject getObjectToDrag (const MouseEvent& e);
    void handleSingleClick (const MouseEvent& e);
    void handleDoubleClick (const MouseEvent& e);

    void drawTunerSliderBackground (Graphics& g);
    void updateLabelBounds (Label* labelToUpdate);

    float getValueAngle();
    void drawValueCursor (Graphics& g);
    void drawLineFromSliderCentre (Graphics& g, float angleRadian);
    void drawThumbsAndToleranceLines (Graphics& g);
    
    //==============================================================================
    const float& value;
    const NormalisableRange<float> gestureRange;
    
    //==============================================================================
    const Range<float> parameterMax;
    
    //==============================================================================
    ScopedPointer<Slider> leftLowSlider;
    ScopedPointer<Slider> leftHighSlider;
    ScopedPointer<Slider> rightLowSlider;
    ScopedPointer<Slider> rightHighSlider;
    
    ScopedPointer<Label> rangeLabelMinLeft;
    ScopedPointer<Label> rangeLabelMaxLeft;
    ScopedPointer<Label> rangeLabelMinRight;
    ScopedPointer<Label> rangeLabelMaxRight;

    ScopedPointer<TextButton> minLeftAngleButton;
    ScopedPointer<TextButton> maxLeftAngleButton;
    ScopedPointer<TextButton> minRightAngleButton;
    ScopedPointer<TextButton> maxRightAngleButton;
    
    //==============================================================================
    DraggableObject objectBeingDragged = none;
    float previousCursorAngle = value;

    juce::Rectangle<int> sliderBounds;
    float sliderRadius;
    Point<int> sliderCentre;
    float startAngle;
    float endAngle;

    //==============================================================================
    HubConfiguration& hubConfig;
    RangeTunerLookAndFeel rangeTunerLookAndFeel;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TwoRangeTuner)
};