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
    //createComboBox();
}

MidiChannelComponent::~MidiChannelComponent()
{
    midiChannelBox.removeListener (this);
}

void MidiChannelComponent::paint (Graphics& g)
{
    g.setColour (highlighted ? neova_dash::colour::subText : neova_dash::colour::mainText);
    drawArrowPath (g, getLocalBounds().removeFromRight (20)
                          .reduced (neova_dash::ui::MARGIN_SMALL)
                          .toFloat());

    g.setColour (neova_dash::colour::mainText);
    g.setFont (neova_dash::font::dashFont.withHeight (15.0f));
    g.drawText ("MIDI Channels", getLocalBounds(), Justification::centred);
}

void MidiChannelComponent::drawArrowPath (Graphics& g, juce::Rectangle<float> area)
{
    Path p;
    p.startNewSubPath (0,0);
    p.lineTo (1, 1);
    p.lineTo (2, 0);

    p.scaleToFit (area.getX(),
                  area.getY(),
                  area.getWidth(),
                  area.getHeight(),
                  true);

    g.strokePath (p, PathStrokeType (1.0f));
}

void MidiChannelComponent::resized()
{
    auto area = getLocalBounds();

    midiChannelBox.setBounds (area.removeFromRight (getWidth()*3/8));
}

void MidiChannelComponent::mouseUp (const MouseEvent& event)
{
    createPopupMenu();
}

void MidiChannelComponent::mouseEnter (const MouseEvent& event)
{
    highlighted = true;
    repaint();
}

void MidiChannelComponent::mouseExit (const MouseEvent& event)
{
    highlighted = false;
    repaint();
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

void MidiChannelComponent::createPopupMenu()
{
    PopupMenu channelsMenu;
    const uint16_t midiChannels = hubConfig.getMidiChannels();

    for (int channelNum =0; channelNum < 16; channelNum++)
    {
        channelsMenu.addItem (channelNum + 1, String (channelNum + 1), true, midiChannels >> channelNum & 1 == 1);
    }
    
    handleMenuResult (channelsMenu.showMenu (PopupMenu::Options().withParentComponent (getParentComponent())
                                                                 .withTargetComponent (this)
                                                                 .withMinimumWidth (getWidth())
                                                                 .withMinimumNumColumns (2)
                                                                 .withPreferredPopupDirection (PopupMenu::Options::PopupDirection::downwards)
                                                                 .withStandardItemHeight (20)));
}

void MidiChannelComponent::handleMenuResult (const int menuResult)
{
    switch (menuResult)
    {
        case 0: // no choice
            highlighted = false;
            repaint();
            break;
        default:
            hubConfig.toggleMidiChannel (menuResult-1);
            createPopupMenu();
            break;
    }
}