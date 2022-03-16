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
#include "../../../DataReader/DataReader.h"

#include "Tuner/GesturesTuner.h"
#include "MidiPanel.h"
#include "../DashShapeButton.h"

class GestureSettingsComponent : public Component
{
public:
    //==============================================================================
    GestureSettingsComponent (int gestId, HubConfiguration& config,
                              ApplicationCommandManager& manager, DataReader& reader);
    ~GestureSettingsComponent() override;
    
    //==============================================================================
    // Component
    void paint (Graphics&) override;
    void resized() override;
    
    //==============================================================================
    Tuner& getTuner() const;
    void updateDisplay();
    void updateComponents() const;
    
    const int gestureId;

private:
    //==============================================================================
    void paintBackground (Graphics&) const;
    
    //==============================================================================
    void createTuner();
    void createToggles();
    void createMidiPanel();
	void disableIfGestureWasDeleted();
    
    //==============================================================================
    bool disabled = false;

    //==============================================================================
    const int HEADER_HEIGHT = 30;
	
    std::unique_ptr<Tuner> gestTuner;
    std::unique_ptr<DashShapeButton> muteButton;
    std::unique_ptr<MidiPanel> midiPanel;

    //==============================================================================
    HubConfiguration& hubConfig;
    ApplicationCommandManager& commandManager;
    DataReader& dataReader;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GestureSettingsComponent)
};