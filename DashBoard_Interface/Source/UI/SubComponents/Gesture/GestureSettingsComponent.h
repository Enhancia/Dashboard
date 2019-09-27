/*
  ==============================================================================

    GestureSettingsComponent.h
    Created: 6 Sep 2019 8:05:32pm
    Author:  aleva

  ==============================================================================
*/

#pragma once

#include "../../../../JuceLibraryCode/JuceHeader.h"
#include "../../../Common/DashCommon.h"
#include "../../../Common/HubConfiguration.h"
#include "Tuner/GesturesTuner.h"
#include "MidiPanel.h"
#include "../DashShapeButton.h"

class GestureSettingsComponent : public Component
{
public:
    //==============================================================================
    GestureSettingsComponent (const int gestureId, HubConfiguration& config, ApplicationCommandManager& manager);
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
    void createMidiPanel();
	void disableIfGestureWasDeleted();
    
    //==============================================================================
    bool disabled = false;

    //==============================================================================
    HubConfiguration& hubConfig;
    const int HEADER_HEIGHT = 30;
	
    std::unique_ptr<Tuner> gestTuner;
    std::unique_ptr<DashShapeButton> muteButton;
    std::unique_ptr<MidiPanel> midiPanel;
    ApplicationCommandManager& commandManager;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GestureSettingsComponent)
};