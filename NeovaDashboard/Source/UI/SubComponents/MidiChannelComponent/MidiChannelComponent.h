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
class MidiChannelComponent : public Component,
                             public ComboBox::Listener
{
public:
    //==========================================================================
    MidiChannelComponent (HubConfiguration& data);
    ~MidiChannelComponent();

    //==========================================================================
    void paint (Graphics&) override;
    void resized() override;

    //==========================================================================
    void mouseEnter (const MouseEvent& event) override;
    void mouseExit (const MouseEvent& event) override;
    void mouseUp (const MouseEvent& event) override;
    void comboBoxChanged (ComboBox *comboBoxThatHasChanged) override;

    //==========================================================================
    void update();

private:
    //==========================================================================
    bool highlighted = false;

    //==========================================================================
    void createComboBox();
    void createPopupMenu();
    void drawArrowPath (Graphics& g, juce::Rectangle<float> area);
    void handleMenuResult (const int menuResult);
    void setHUBToFirstActiveChannelOrChannel1();
    
    //==========================================================================
    HubConfiguration& hubConfig;
    ComboBox midiChannelBox;

    //==========================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MidiChannelComponent)
};
