/*
  ==============================================================================

    GestureSettingsComponent.h
    Created: 6 Sep 2019 8:05:32pm
    Author:  aleva

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "DashCommon.h"
#include "HubConfiguration.h"
#include "Tuner.h"
#include "VibratoTuner.h"

class GestureSettingsComponent : public Component
{
public:
    //==============================================================================
    GestureSettingsComponent (const int gestureId, HubConfiguration& config);
    ~GestureSettingsComponent();
    
    //==============================================================================
    // Component
    void paint (Graphics&) override;
    void resized() override;
    
    //==============================================================================
    Tuner& getTuner();
    void updateDisplay();
    void updateComponents();
    
    const int gestureId;

private:
    //==============================================================================
    void paintBackground (Graphics&);
    
    //==============================================================================
    void createTuner();
    void createToggles();
	void disableIfGestureWasDeleted();
    
    //==============================================================================
    bool disabled = false;

    //==============================================================================
    HubConfiguration& hubConfig;
    const int HEADER_HEIGHT = 30;
	
    std::unique_ptr<Tuner> gestTuner;
    //ScopedPointer<PlumeShapeButton> muteButton;

    std::unique_ptr<Component> midiPanel;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GestureSettingsComponent)
};