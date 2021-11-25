/*
  ==============================================================================

    MidiChannelComponent.h
    Created: 4 Dec 2019 2:22:39pm
    Author:  Enhancia Dev

  ==============================================================================
*/

#pragma once

#include "../../../../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/*
*/
class MidiChannelComponent : public Component
{
public:
    //==========================================================================
    MidiChannelComponent (HubConfiguration& data, const bool controlsInputMidi);
    ~MidiChannelComponent();

    //==========================================================================
    void paint (Graphics&) override;
    void resized() override;

    //==========================================================================
    void mouseEnter (const MouseEvent& event) override;
    void mouseExit (const MouseEvent& event) override;
    void mouseUp (const MouseEvent& event) override;

    //==========================================================================
    void update();

private:
    //==========================================================================
    bool highlighted = false;

    //==========================================================================
    void createPopupMenu();
    void drawArrowPath (Graphics& g, juce::Rectangle<float> area);
    static void menuCallback (int result, MidiChannelComponent* mcComp);
    void handleMenuResult (const int menuResult);
    
    //==========================================================================
    HubConfiguration& hubConfig;
    const bool isInput;

    //==========================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MidiChannelComponent)
};


class MidiThroughComponent : public Component,
                             public Button::Listener
{
public:
    //==========================================================================
    MidiThroughComponent (HubConfiguration& data);
    ~MidiThroughComponent();

    //==========================================================================
    void paint (Graphics&) override;
    void resized() override;

    //==========================================================================
    void update();

    //==========================================================================
    void buttonClicked (Button* bttn) override;

    //==========================================================================
    //void mouseEnter (const MouseEvent& event) override;
    //void mouseExit (const MouseEvent& event) override;
    //void mouseUp (const MouseEvent& event) override;

private:
    //==========================================================================
    HubConfiguration& hubConfig;
    ToggleButton throughButton;

    //==========================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MidiThroughComponent)
};