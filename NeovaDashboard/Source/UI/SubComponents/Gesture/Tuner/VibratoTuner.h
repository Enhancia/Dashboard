/*
  ==============================================================================

    VibratoTuner.h
    Created: 12 Sep 2019 12:05:15pm
    Author:  Enhancia Dev

  ==============================================================================
*/

#pragma once

#include "../../../../../JuceLibraryCode/JuceHeader.h"
#include "../../../../Common/DashCommon.h"
#include "../../../../Common/HubConfiguration.h"
#include "Tuner.h"

class VibratoTuner:    public Tuner,
                       private Slider::Listener,
                       private Label::Listener
{
public:
    //==============================================================================
    VibratoTuner (HubConfiguration& config, const int gestureId);
    ~VibratoTuner();
    
    //==============================================================================
    void paint (Graphics& g) override;
    void resized() override;
    void updateComponents();
    void updateDisplay();
    void updateColour() override;
    
    //==============================================================================
    void labelTextChanged (Label* lbl) override;
    void editorHidden (Label* lbl, TextEditor& ted) override;
    void sliderValueChanged (Slider* sldr) override;

    //==============================================================================
    void mouseDown (const MouseEvent& e) override;
    void mouseUp (const MouseEvent& e) override;
    void setColour (const Colour newColour) override;

    //==============================================================================
    const int id;
    
private:
    //==============================================================================
	VibratoTuner (HubConfiguration& config, const float& val,
	                                        NormalisableRange<float> gestRange, const int gestureId,
					                        const float& vibratoIntensity, float maxIntens,
    				                        const Range<float> gainMax, const Range<float> threshMax);

    //==============================================================================
    void createSliders();
    void createLabels();
    void updateLabelBounds (Label* labelToUpdate);

    //==============================================================================
    void drawValueCursor (Graphics& g);
    void drawIntensityCursor (Graphics& g);
    void setThresholdSliderColour();
    void computeSmoothIntensity (float smoothnessRamp);

    //==============================================================================
    void setGain (float value);
    void setThreshold (float value);

    float getGain();
    float getThreshold();
    
    //==============================================================================
    const float& value;
    float lastValue = -1.0f;

    const NormalisableRange<float> gestureRange;
    const float& intensity;
    const float maxIntensity;
	float lastIntensity = -1.0f;
    float smoothIntensity = intensity;
    float incrementalSmoothFactor = 1.0f;

    const Range<float> parameterMaxGain;
    const Range<float> parameterMaxThreshold;
    
    HubConfiguration& hubConfig;

    ScopedPointer<Slider> gainSlider;
    ScopedPointer<Slider> thresholdSlider;
    ScopedPointer<Label> gainLabel;
    ScopedPointer<Label> thresholdLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VibratoTuner)
};