/*
  ==============================================================================

    HubComponent.h
    Created: 3 Sep 2019 11:37:45am
    Author:  Enhancia Dev

  ==============================================================================
*/

#pragma once

#include "../../../../JuceLibraryCode/JuceHeader.h"
#include "../../../Common/DashCommon.h"
#include "../../../Common/HubConfiguration.h"
#include "../Top/NewGesturePanel.h"

//==============================================================================
/*
*/
class HubComponent    : public Component,
                        private Button::Listener
{
public:
    enum PresetModeState
    {
        normalState =0,
        presetState,
        slaveState
    };

    //==============================================================================
    HubComponent (HubConfiguration& data, NewGesturePanel& newGest,
                  ApplicationCommandManager& manager, int& presetState, int& dashState);
    ~HubComponent() override;

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

    //==============================================================================
    void buttonClicked (Button* bttn) override;
    void handleHubButtonClick (int buttonId);

    //==========================================================================
    void update() const;
    void repaintLEDs() const;

    //==============================================================================
    void lockHubToPresetMode (bool shouldLockHub);
    void setPresetStateToPresetMode (bool notifyChange = true) const;
    void setPresetStateToNormalMode (bool notifyChange = true) const;

    //==========================================================================
    bool getControlButtonDown() const;
    void setControlButtonDown (bool shouldBeDown);

private:
    //==============================================================================
    class HubButton : public Button
    {
    public:
        //==========================================================================
        HubButton (int buttonNum);
        ~HubButton() override;

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
        GestureLED (int ledNum, HubConfiguration& config, const int& presetState);
        ~GestureLED() override;

        //==========================================================================
        void paint (Graphics&) override;
        void resized() override;

        //==========================================================================
        const int id;
        bool forceOff = false;

        HubConfiguration& hubConfig;
        const int& presetModeState;
    
    private:
        //==========================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GestureLED)
    };

    //==============================================================================
    int currentPreset = 0;
    int& presetModeState;

    bool ctrlButtonDown = false;
    bool locked = false;
    
    HubConfiguration& hubConfig;
    ApplicationCommandManager& commandManager;
    NewGesturePanel& newGesturePanel;
    int& dashboardState;

    //==============================================================================
    Image hubImage = ImageFileFormat::loadFrom (DashData::DashboardHUBonlycropped_png,
                                                DashData::DashboardHUBonlycropped_pngSize);

    Image hubRingImage = ImageFileFormat::loadFrom (DashData::DashboardHUBALLcropped_png,
                                                    DashData::DashboardHUBALLcropped_pngSize);

    //==============================================================================
    OwnedArray<HubButton> buttons;
    OwnedArray<GestureLED> leds;

    const int CTRL_ID = neova_dash::gesture::NUM_GEST;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HubComponent)
};