/*
  ==============================================================================

    PresetSelectorComponent.h
    Created: 3 Sep 2019 11:37:45am
    Author:  Enhancia Dev

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "DashCommon.h"
#include "HubConfiguration.h"

//==============================================================================
/*
*/
class PresetSelectorComponent    : public Component,
                                   private Button::Listener
{
public:
    //==============================================================================
    PresetSelectorComponent (HubConfiguration& data);
    ~PresetSelectorComponent();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

    //==============================================================================
    void buttonClicked (Button* bttn) override;

private:
    //==============================================================================
    class PresetToggle : public Button
    {
    public:
        //==========================================================================
        PresetToggle (const int toggleNum);
        ~PresetToggle();

        //==========================================================================
        void resized() override;
   
        //==========================================================================
        const int id;

    private:
        //==========================================================================
        void paintButton (Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;

        //==========================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PresetToggle)
    };

    class GestureLED : public Component
    {
    public:
        //==========================================================================
        GestureLED (const int ledNum, HubConfiguration& config);
        ~GestureLED();

        //==========================================================================
        void paint (Graphics&) override;
        void resized() override;

        //==========================================================================
        const int id;
        HubConfiguration& hubConfig;
    
    private:
        //==========================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GestureLED)
    };

    //==============================================================================
    int currentPreset = -1;
    
    HubConfiguration& hubConfig;

    //==============================================================================
    OwnedArray<PresetToggle> toggles;
    OwnedArray<GestureLED> leds;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PresetSelectorComponent)
};
