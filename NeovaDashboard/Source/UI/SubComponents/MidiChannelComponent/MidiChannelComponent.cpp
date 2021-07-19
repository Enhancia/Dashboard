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
}

MidiChannelComponent::~MidiChannelComponent()
{
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

void MidiChannelComponent::update()
{
    repaint();
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