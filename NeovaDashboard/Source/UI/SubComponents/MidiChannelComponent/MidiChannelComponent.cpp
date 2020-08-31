/*
  ==============================================================================

    MidiChannelComponent.cpp
    Created: 4 Dec 2019 2:22:39pm
    Author:  Enhancia Dev

  ==============================================================================
*/

#include "../../../../JuceLibraryCode/JuceHeader.h"
#include "../../../Common/DashCommon.h"
#include "../../../Common/HubConfiguration.h"
#include "MidiChannelComponent.h"

//==============================================================================
MidiChannelComponent::MidiChannelComponent (HubConfiguration& data)
	: hubConfig (data)
{
	createComboBox();
}

MidiChannelComponent::~MidiChannelComponent()
{
	midiChannelBox.removeListener (this);
}

void MidiChannelComponent::paint (Graphics& g)
{
	g.setFont (neova_dash::font::dashFont.withHeight (15.0f));
	g.setColour (neova_dash::colour::mainText);

	//g.drawRect (getLocalBounds());
	g.drawText ("MIDI Channel:", getLocalBounds().withRight (midiChannelBox.getX()), Justification::left);
	g.drawText (midiChannelBox.getText(), getLocalBounds().withTrimmedLeft (g.getCurrentFont().getStringWidth ("MIDI Channel:"))
														  .withTrimmedRight (15)
										, Justification::centred);
}

void MidiChannelComponent::resized()
{
	auto area = getLocalBounds();

	midiChannelBox.setBounds (area.removeFromRight (getWidth()*3/8));
}

void MidiChannelComponent::comboBoxChanged (ComboBox* comboBoxThatHasChanged)
{
	if (auto* comboBoxThatHasChanged = &midiChannelBox)
	{
		hubConfig.setMidiChannelExclusive (midiChannelBox.getSelectedId() - 1);
		repaint();
	}
}

void MidiChannelComponent::update()
{
	uint16_t midiChannels = hubConfig.getMidiChannels();
	double channelToSet = std::log2 (midiChannels);

	// Check for legacy HUBs with multiple channels (-> multiple bytes at 1), and wrong channel values in general
	if (channelToSet != static_cast<int>(channelToSet) || channelToSet < 0 || channelToSet > 15)
	{
		setHUBToFirstActiveChannelOrChannel1();
	}
	else
	{
		midiChannelBox.setSelectedId (int (channelToSet) + 1, dontSendNotification);
	}

}

void MidiChannelComponent::createComboBox()
{
	addAndMakeVisible (midiChannelBox);

	for (int channel=1; channel<=16; channel++)
	{
		midiChannelBox.addItem (String (channel), channel);
	}

	midiChannelBox.setJustificationType (Justification::centred);
	midiChannelBox.setColour (ComboBox::textColourId, Colour (0x000000));
	midiChannelBox.addListener (this);

	//update();
}

void MidiChannelComponent::setHUBToFirstActiveChannelOrChannel1()
{
	uint16_t midiChannels = hubConfig.getMidiChannels();

	for (int channel = 0; channel<16; channel++)
	{
		if (midiChannels << channel & 1 == 1)
		{
			hubConfig.setMidiChannelExclusive (channel);
			midiChannelBox.setSelectedId (channel + 1);
			return;
		}
	}

	hubConfig.setMidiChannelExclusive (0);
	midiChannelBox.setSelectedId (1);
}