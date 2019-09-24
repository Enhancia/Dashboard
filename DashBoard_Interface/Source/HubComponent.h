/*
  ==============================================================================

    HubComponent.h
    Created: 3 Sep 2019 11:37:45am
    Author:  Enhancia Dev

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "DashCommon.h"
#include "HubConfiguration.h"
#include "NewGesturePanel.h"

//==============================================================================
/*
*/
class HubComponent    : public Component,
                        private Button::Listener
{
public:
    enum HubMode
    {
        gestureMute =0,
        presetSelection
    };

    //==============================================================================
    HubComponent (HubConfiguration& data, NewGesturePanel& newGest, ApplicationCommandManager& manager);
    ~HubComponent();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

    //==============================================================================
    void buttonClicked (Button* bttn) override;
    void handleHubButtonClick (const int buttonId);

    //==========================================================================
    void update();
    void repaintLEDs();
    void switchHubMode();
    HubMode getCurrentMode();
    bool getControlButtonDown();
    void setControlButtonDown (const bool shouldBeDown);

private:
    //==============================================================================
    class HubButton : public Button
    {
    public:
        //==========================================================================
        HubButton (const int buttonNum);
        ~HubButton();

        //==========================================================================
        void resized() override;
   
        //==========================================================================
        const int id;

    private:
        //==========================================================================
        void paintButton (Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;

        //==========================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HubButton)
    };

    class GestureLED : public Component
    {
    public:
        //==========================================================================
        GestureLED (const int ledNum, HubConfiguration& config, HubComponent::HubMode& hubMode);
        ~GestureLED();

        //==========================================================================
        void paint (Graphics&) override;
        void resized() override;

        //==========================================================================
        const int id;
        HubConfiguration& hubConfig;
        HubComponent::HubMode& mode;
    
    private:
        //==========================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GestureLED)
    };

    //==============================================================================
    int currentPreset = 0;
    HubMode mode = gestureMute;
    bool ctrlButtonDown = false;
    
    HubConfiguration& hubConfig;
    ApplicationCommandManager& commandManager;
    NewGesturePanel& newGesturePanel;

    //==============================================================================
    Image hubImage = ImageFileFormat::loadFrom (DashData::DashboardHUBonlycropped_png,
                                                DashData::DashboardHUBonlycropped_pngSize);

    //==============================================================================
    OwnedArray<HubButton> buttons;
    OwnedArray<GestureLED> leds;

    const int CTRL_ID = neova_dash::gesture::NUM_GEST;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HubComponent)
};