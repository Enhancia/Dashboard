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
MidiChannelComponent::MidiChannelComponent (HubConfiguration& data, const bool controlsInputMidi)
    : hubConfig (data), isInput (controlsInputMidi)
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
    g.drawText (String (isInput ? "MIDI IN"
                                : "MIDI OUT"),
                getLocalBounds(), Justification::centred);
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

void MidiChannelComponent::mouseUp (const MouseEvent&)
{
    createPopupMenu();
}

void MidiChannelComponent::mouseEnter (const MouseEvent&)
{
    highlighted = true;
    repaint();
}

void MidiChannelComponent::mouseExit (const MouseEvent&)
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
    const auto midiChannels = static_cast<uint16_t>(hubConfig.getMidiChannels (isInput));

    if(firstInit && !isInput)
		listMidiOut.clear();

    for (int channelNum =0; channelNum < 16; channelNum++)
    {
		// prevent old configuration filling
		if(firstInit && listMidiOut.size() >= 5)
		{
			channelsMenu.addItem (channelNum + 1, String (channelNum + 1), true, false);
		} else
		{
			channelsMenu.addItem (channelNum + 1, String (channelNum + 1), true, midiChannels >> channelNum & true);
		}

		// fill saved configuration
        if(midiChannels >> channelNum & true)
        {
            if(firstInit && !isInput)
            {
				if(listMidiOut.size() >= 5)
				{
					hubConfig.toggleMidiChannel (channelNum, isInput);
				} else
				{
					listMidiOut.add(channelNum + 1);
				}
            }
        }
    }

    if(!isInput)
		firstInit = false;

    channelsMenu.showMenuAsync (PopupMenu::Options().withParentComponent (getParentComponent())
                                                                 .withTargetComponent (this)
                                                                 .withMinimumWidth (getWidth())
                                                                 .withMinimumNumColumns (2)
                                                                 .withPreferredPopupDirection (PopupMenu::Options::PopupDirection::downwards)
                                                                 .withStandardItemHeight (20),
                                ModalCallbackFunction::forComponent (menuCallback, this));
}

void MidiChannelComponent::menuCallback (int result, MidiChannelComponent* mcComp)
{
    if (mcComp != nullptr)
    {
        mcComp->handleMenuResult (result);
    }
}

void MidiChannelComponent::handleMenuResult (const int menuResult)
{
	auto itr = std::find (std::begin (listMidiOut), std::end (listMidiOut), menuResult);
    bool exist = itr != std::end (listMidiOut);
    auto index = std::distance( std::begin( listMidiOut ), itr );

	switch (menuResult)
	{
	case 0: // no choice
		highlighted = false;
		repaint ();
		break;
	default:
		if (isInput) // midi in
		{
			hubConfig.toggleMidiChannel (menuResult - 1, isInput);
		}
		else // midi out
		{
			// limit the selection to 5
			if (listMidiOut.size () >= 5)
			{
				//check if selection already exist in listMidiOut array
				if (exist) // deselect
				{
					hubConfig.toggleMidiChannel (menuResult - 1, isInput);
					listMidiOut.remove (static_cast<int>(index));
				}
				else // select and remove the oldest
				{
					hubConfig.toggleMidiChannel (listMidiOut.getLast () - 1, isInput);
					listMidiOut.removeLast ();
					//listMidiOut.remove (0);

					hubConfig.toggleMidiChannel (menuResult - 1, isInput);
					listMidiOut.add (menuResult);
				}
			}
			else //limit to 5 not reached
			{
				if (exist) // deselect
				{
					if (listMidiOut.size () == 1) // prevent remove in listMidiOut if there is only one left
					{
						hubConfig.toggleMidiChannel (menuResult - 1, isInput);
					}
					else
					{
						hubConfig.toggleMidiChannel (menuResult - 1, isInput);
						listMidiOut.remove (static_cast<int>(index));
					}
				}
				else // select
				{
					hubConfig.toggleMidiChannel (menuResult - 1, isInput);
					listMidiOut.add (menuResult);
				}
			}
		}
		createPopupMenu ();
		break;
	}
}
