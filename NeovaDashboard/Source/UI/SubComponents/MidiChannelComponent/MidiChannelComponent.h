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
                             public Button::Listener
{
public:
    //==========================================================================
    MidiChannelComponent (HubConfiguration& data);
    ~MidiChannelComponent();

    //==========================================================================
    void paint (Graphics&) override;
    void resized() override;

    //==========================================================================
    void buttonClicked (Button* bttn) override;

    //==========================================================================
    void update();

    //==========================================================================
    const int MAX_CHANNELS = 4;

private:
    //==========================================================================
	class ChannelButton : public Button
	{
	public:
		//======================================================================
		ChannelButton(const String& name, const int channelNumber);
		~ChannelButton();

		//======================================================================
		const int channel;

	protected:
		//======================================================================
		void paintButton (Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;

	private:
		//======================================================================
		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ChannelButton)
	};

    //==========================================================================
    void initializeButtons();

    //==========================================================================
    void selectChannelFifo (const int channelToSelect);
    void selectChannelLifo (const int channelToSelect);
    void unselectChannel (const int channelNumber);
    
    //==========================================================================
    HubConfiguration& hubConfig;
    OwnedArray<ChannelButton> buttons;
    Array<ChannelButton*> selectedButtons;

    //==========================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MidiChannelComponent)
};
