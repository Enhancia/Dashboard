/*
  ==============================================================================

    OneRangeTuner.h
    Created: 13 Sep 2019 3:57:33pm
    Author:  Enhancia Dev

  ==============================================================================
*/

#pragma once

#include "../../../../../JuceLibraryCode/JuceHeader.h"
#include "../../../../Common/DashCommon.h"
#include "../../../../Common/HubConfiguration.h"
#include "../../../LookAndFeel/DashBoardLookAndFeel.h"
#include "Tuner.h"

class OneRangeTuner:  public Tuner,
                      private Slider::Listener,
                      private Label::Listener,
                      private Button::Listener
{
public:
    enum TunerStyle
    {
        tilt =0,
        roll,
        wave
    };

    enum DraggableObject
    {
        none = -1,
        lowThumb,
        highThumb,
        middleArea
    };

    //==============================================================================
    OneRangeTuner (HubConfiguration& config, int gestureId,
				   const float& val, NormalisableRange<float> gestureRange,
                   Range<float> paramMax, String unit = "", TunerStyle style = tilt);
    ~OneRangeTuner() override;
    
    //==============================================================================
    void paint (Graphics& g) override;
    void resized() override;
    
    void updateComponents() override;
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
    void setStyle (TunerStyle newStyle);
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
    void setRangeLow (float value, bool uploadToHub = true) const;
    void setRangeHigh (float value, bool uploadToHub = true) const;
    
    float getRangeLow() const;
    float getRangeHigh() const;

    double getAngleFromMouseEventRadians (const MouseEvent& e) const;
    double getThumbAngleRadians (DraggableObject thumb) const;

    DraggableObject getObjectToDrag (const MouseEvent& e) const;
    void handleSingleClick (const MouseEvent& e);
    void handleDoubleClick (const MouseEvent& e) const;

    void drawTunerSliderBackground (Graphics& g) const;
    void updateLabelBounds (Label* labelToUpdate) const;

    float getValueAngle() const;
    void drawValueCursor (Graphics& g);
    void drawLineFromSliderCentre (Graphics&, float angleRadian) const;
    void drawThumbsAndToleranceLines (Graphics& g);
    
    //==============================================================================
    const float& value;
    const NormalisableRange<float> gestureRange;

    //==============================================================================
    const Range<float> parameterMax;
    
    std::unique_ptr<Slider> lowSlider;
    std::unique_ptr<Slider> highSlider;
    std::unique_ptr<Label> rangeLabelMin;
    std::unique_ptr<Label> rangeLabelMax;
    std::unique_ptr<TextButton> minAngleButton;
    std::unique_ptr<TextButton> maxAngleButton;
    
    //==============================================================================
    TunerStyle tunerStyle;

    DraggableObject objectBeingDragged = none;
    float previousCursorAngle = value;

    juce::Rectangle<int> sliderBounds;
    float sliderRadius;
    Point<int> sliderCentre;
    float startAngle;
    float endAngle;
    bool maxAngleBtnIsHovered = false;
    bool minAngleBtnIsHovered = false;

    //==============================================================================
    HubConfiguration& hubConfig;
    RangeTunerLookAndFeel rangeTunerLookAndFeel;

    CriticalSection rangeUpdateLock;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OneRangeTuner)
};