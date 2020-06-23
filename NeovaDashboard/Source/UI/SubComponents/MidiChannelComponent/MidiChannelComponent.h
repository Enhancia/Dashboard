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
    void comboBoxChanged (ComboBox *comboBoxThatHasChanged) override;

    //==========================================================================
    void update();

private:
    //==========================================================================
    void createComboBox();
    void setHUBToFirstActiveChannelOrChannel1();
    
    //==========================================================================
    HubConfiguration& hubConfig;
    ComboBox midiChannelBox;

    //==========================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MidiChannelComponent)
};
