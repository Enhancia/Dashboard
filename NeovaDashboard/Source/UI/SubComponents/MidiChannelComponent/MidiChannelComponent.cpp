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
	initializeButtons();
}

MidiChannelComponent::~MidiChannelComponent()
{
}

void MidiChannelComponent::paint (Graphics& g)
{
	g.setFont (neova_dash::font::dashFont.withHeight (15.0f));
	g.setColour (Colour (0xffffffff));

	g.drawText ("MIDI Channels :", getLocalBounds ().removeFromLeft (90), Justification::centred);
}

void MidiChannelComponent::resized()
{
	auto area = getLocalBounds();
	area.removeFromLeft (90 + 2*neova_dash::ui::MARGIN);

	const int buttonW = area.getWidth()/buttons.size();

	for (auto button : buttons)
	{
		button->setBounds (area.removeFromLeft (buttonW)
							   .reduced (0, neova_dash::ui::MARGIN_SMALL));
	}
}

void MidiChannelComponent::buttonClicked (Button* bttn)
{
	if (auto* channelButton = dynamic_cast<ChannelButton*> (bttn))
	{
		if (channelButton->getToggleState())
		{
			unselectChannel (channelButton->channel);
		}
		else
		{
			selectChannelFifo (channelButton->channel);
		}
	}
}

void MidiChannelComponent::update()
{
	uint16_t midiChannels = hubConfig.getMidiChannels();

	for (int channel =0; channel < 16; channel++)
	{
		if ((midiChannels & (1 << channel)) != 0)
		{
			selectChannelFifo (channel);
		}
		else
		{
			unselectChannel (channel);
		}
	}
}

void MidiChannelComponent::initializeButtons()
{
	uint16_t midiChannels = hubConfig.getMidiChannels();
	
	for (int channel =0; channel < 16; channel++)
	{
		buttons.add (new ChannelButton (String ("channelButton") + String (channel), channel));
		addAndMakeVisible (buttons.getLast());
		buttons.getLast()->addListener (this);

		if ((midiChannels & (1 << channel)) != 0)
		{
			selectChannelFifo (channel);
		}
	}
}

void MidiChannelComponent::selectChannelFifo (const int channelToSelect)
{
	if (buttons[channelToSelect]->getToggleState() || channelToSelect < 0
												   || channelToSelect > 15 ) return;

	if (selectedButtons.size() == MAX_CHANNELS)
	{
		unselectChannel (selectedButtons.getLast()->channel);
	}

	selectedButtons.insert (0, buttons[channelToSelect]);
	buttons[channelToSelect]->setToggleState (true, dontSendNotification);
	hubConfig.setMidiChannel (channelToSelect, true);
}

void MidiChannelComponent::selectChannelLifo (const int channelToSelect)
{
	if (buttons[channelToSelect]->getToggleState() || channelToSelect < 0
												   || channelToSelect > 15 ) return;

	if (selectedButtons.size() == MAX_CHANNELS)
	{
		unselectChannel (selectedButtons.getLast()->channel);
	}

	selectedButtons.add (buttons[channelToSelect]);
	buttons[channelToSelect]->setToggleState (true, dontSendNotification);
	hubConfig.setMidiChannel (channelToSelect, true);
}

void MidiChannelComponent::unselectChannel (const int channelToSelect)
{ 
	auto* buttonToUnselect = buttons[channelToSelect];
	int idInSelectedArray = selectedButtons.indexOf(buttonToUnselect);

	if (idInSelectedArray != -1)
	{
		selectedButtons.remove (idInSelectedArray);
		buttonToUnselect->setToggleState (false, dontSendNotification);
		hubConfig.setMidiChannel (channelToSelect, false);
	}
	else { DBG ("Channel wasn't selected to begin with..... "); }
}

//==============================================================================
MidiChannelComponent::ChannelButton::ChannelButton (const String& name, const int channelNumber)
	: Button (name), channel (channelNumber)
{
	//setClickingTogglesState (true);
}

MidiChannelComponent::ChannelButton::~ChannelButton()
{
}

void MidiChannelComponent::ChannelButton::paintButton (Graphics& g, bool shouldDrawButtonAsHighlighted,
																	bool shouldDrawButtonAsDown)
{
	if (shouldDrawButtonAsHighlighted)
	{
		g.setColour (Colour (0x30ffffff));
		g.fillRoundedRectangle (getLocalBounds().toFloat(), 2.0f);
	}

	if (getToggleState())
	{
		g.setColour (Colour (0xd0ffffff));
		g.fillRect (getLocalBounds().removeFromBottom (2));
	}
	
	g.setColour (Colour (getToggleState() ? 0xd0ffffff : 0x40ffffff));

	g.setFont (neova_dash::font::dashFont.withHeight (13.0f));
	g.drawText (String (channel+1), getLocalBounds(), Justification::centred, false);
}