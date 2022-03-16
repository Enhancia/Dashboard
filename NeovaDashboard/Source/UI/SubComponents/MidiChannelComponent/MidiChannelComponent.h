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
    MidiChannelComponent (HubConfiguration& data, bool controlsInputMidi);
    ~MidiChannelComponent() override;

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
    void handleMenuResult (int menuResult);
    
    //==========================================================================
    HubConfiguration& hubConfig;
    const bool isInput;
    bool firstInit = true;
    Array<int> listMidiOut;

    //==========================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MidiChannelComponent)
};