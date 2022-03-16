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
#include "../../../../DataReader/DataReader.h"
#include "Tuner.h"

class VibratoTuner:    public Tuner,
                       private Slider::Listener,
                       private Label::Listener
{
public:
    //==============================================================================
    VibratoTuner (HubConfiguration& config, DataReader& reader, int gestureId);
    ~VibratoTuner() override;
    
    //==============================================================================
    void paint (Graphics& g) override;
    void resized() override;
    void updateComponents() override;
    void updateDisplay() override;
    void updateColour() override;
    
    //==============================================================================
    void labelTextChanged (Label* lbl) override;
    void editorShown (Label* lbl, TextEditor& ted) override;
    void editorHidden (Label* lbl, TextEditor& ted) override;
    void sliderValueChanged (Slider* sldr) override;

    //==============================================================================
    void mouseDown (const MouseEvent& e) override;
    void mouseUp (const MouseEvent& e) override;
    void setColour (Colour newColour) override;

    //==============================================================================
    const int id;
    
private:
    //==============================================================================
	VibratoTuner (HubConfiguration& config, const float& val,
	                                        NormalisableRange<float> gestRange, int gestureId,
					                        const float& vibratoIntensity, float maxIntens,
                                            Range<float> gainMax, Range<float> threshMax);

    //==============================================================================
    void createSliders();
    void createLabels();
    void updateLabelBounds (Label* labelToUpdate) const;

    //==============================================================================
    void drawValueCursor (Graphics& g);
    void drawIntensityCursor (Graphics& g);
    void setThresholdSliderColour();
    void computeSmoothIntensity (float smoothnessRamp);

    //==============================================================================
    void setGain (float value) const;
    void setThreshold (float value) const;

    float getGain() const;
    float getThreshold() const;
    
    //==============================================================================
    const float& value;
    float lastValue = -1.0f;
    juce::Rectangle<int> tunerArea;

    const NormalisableRange<float> gestureRange;
    const float& intensity;
    const float maxIntensity;
    float lastIntensity = -1.0f;
    float smoothIntensity = intensity;
    float incrementalSmoothFactor = 1.0f;

    const Range<float> parameterMaxGain;
    const Range<float> parameterMaxThreshold;
    
    HubConfiguration& hubConfig;

    std::unique_ptr<Slider> gainSlider;
    std::unique_ptr<Slider> thresholdSlider;
    std::unique_ptr<Label> gainLabel;
    std::unique_ptr<Label> thresholdLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VibratoTuner)
};