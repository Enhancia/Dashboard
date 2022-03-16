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
    TwoRangeTuner (HubConfiguration& config, int gestureId,
				   const float& val, NormalisableRange<float> gestureRange,
                   Range<float> paramMax, String unit = "");
    ~TwoRangeTuner() override;
    
    //==============================================================================
    void paint (Graphics& g) override;
    void resized() override;
    
    void updateComponents() override;
    void updateComponents (DraggableObject thumbThatShouldUpdate);
    void updateDisplay() override;

    void setColour (Colour newColour) override;
    void updateColour() override;
    
    //==============================================================================
    void labelTextChanged (Label* lbl) override;
    void editorShown (Label* lbl, TextEditor& ted) override;
    void editorHidden (Label* lbl, TextEditor& ted) override;
    void sliderValueChanged (Slider* sldr) override;
    void buttonClicked (Button* bttn) override;
    void buttonStateChanged (Button* btn) override;
    
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
    void resizeButtons() const;
    
    //==============================================================================
    void setRangeLeftLow (float value, bool uploadToHub = true) const;
    void setRangeLeftHigh (float value, bool uploadToHub = true) const;
    void setRangeRightLow (float value, bool uploadToHub = true) const;
    void setRangeRightHigh (float value, bool uploadToHub = true) const;
    
    float getRangeLeftLow() const;
    float getRangeLeftHigh() const;
    float getRangeRightLow() const;
    float getRangeRightHigh() const;
    
    //==============================================================================
    double getAngleFromMouseEventRadians (const MouseEvent& e) const;
    double getThumbAngleRadians (DraggableObject thumb) const;

    DraggableObject getObjectToDrag (const MouseEvent& e) const;
    void handleSingleClick (const MouseEvent& e);
    void handleDoubleClick (const MouseEvent& e) const;

    void drawTunerSliderBackground (Graphics& g) const;
    void updateLabelBounds (Label* labelToUpdate) const;

    float getValueAngle() const;
    void drawValueCursor (Graphics& g);
    void drawLineFromSliderCentre (Graphics& g, float angleRadian) const;
    void drawThumbsAndToleranceLines (Graphics& g);
    
    //==============================================================================
    const float& value;
    const NormalisableRange<float> gestureRange;
    
    //==============================================================================
    const Range<float> parameterMax;
    
    //==============================================================================
    std::unique_ptr<Slider> leftLowSlider;
    std::unique_ptr<Slider> leftHighSlider;
    std::unique_ptr<Slider> rightLowSlider;
    std::unique_ptr<Slider> rightHighSlider;
    
    std::unique_ptr<Label> rangeLabelMinLeft;
    std::unique_ptr<Label> rangeLabelMaxLeft;
    std::unique_ptr<Label> rangeLabelMinRight;
    std::unique_ptr<Label> rangeLabelMaxRight;

    std::unique_ptr<TextButton> minLeftAngleButton;
    std::unique_ptr<TextButton> maxLeftAngleButton;
    std::unique_ptr<TextButton> minRightAngleButton;
    std::unique_ptr<TextButton> maxRightAngleButton;
    
    //==============================================================================
    DraggableObject objectBeingDragged = none;
    float previousCursorAngle = value;

    juce::Rectangle<int> sliderBounds;
    float sliderRadius;
    Point<int> sliderCentre;
    float startAngle;
    float endAngle;
    bool maxLeftAngleBtnIsHovered = false;
    bool minLeftAngleBtnIsHovered = false;
    bool maxRightAngleBtnIsHovered = false;
    bool minRightAngleBtnIsHovered = false;

    //==============================================================================
    HubConfiguration& hubConfig;
    RangeTunerLookAndFeel rangeTunerLookAndFeel;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TwoRangeTuner)
};