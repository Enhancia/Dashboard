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

class GestureSettingsComponent : public Component
                                //private Button::Listener
{
public:
    //==============================================================================
    GestureSettingsComponent (const int gestureId, HubConfiguration& config);
    ~GestureSettingsComponent();
    
    //==============================================================================
    // Component
    void paint (Graphics&) override;
    void resized() override;
    
    /*
    //==============================================================================
    //callbacks
    void buttonClicked (Button*) override;
    
    //==============================================================================
    Tuner& getTuner();
    */
    void updateDisplay();
    void updateComponents();
    
    const int gestureId;

private:
    //==============================================================================
    void paintBackground (Graphics&);
    
    /*
    //==============================================================================
    void createTuner();
    void createToggles();
    void createPanels();
    void showAppropriatePanel();
	void disableIfGestureWasDeleted();
    
    //==============================================================================
    bool disabled = false;
	*/
    //==============================================================================
    HubConfiguration& hubConfig;
    const int HEADER_HEIGHT = 30;
	/*
    ScopedPointer<Tuner> gestTuner;
    ScopedPointer<PlumeShapeButton> muteButton;
    */
    ScopedPointer<Component> midiPanel;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GestureSettingsComponent)
};