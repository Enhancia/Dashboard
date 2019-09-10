/*
  ==============================================================================

    GestureSettingsComponent.cpp
    Created: 6 Sep 2019 8:05:32pm
    Author:  aleva

  ==============================================================================
*/

#include "GestureSettingsComponent.h"

GestureSettingsComponent::GestureSettingsComponent (const int gestId, HubConfiguration& config)
	: gestureId (gestId), hubConfig (config) 
{
	addAndMakeVisible (midiPanel = new Component ("Midi Panel"));
}

GestureSettingsComponent::~GestureSettingsComponent()
{
	midiPanel = nullptr;
}

void GestureSettingsComponent::paint (Graphics& g)
{
    using namespace neova_dash::ui;

    // Enhancia Text

    // Background
    paintBackground (g);

    // HeaderText
    auto headerArea = getLocalBounds().removeFromTop (this->HEADER_HEIGHT);

    //Advanced Settings text
    g.setColour (neova_dash::colour::subText);
    g.setFont (Font().withHeight (10.0f));
    g.drawText ("ADVANCED PANEL",
                headerArea.removeFromLeft (getWidth()/3).reduced (MARGIN, MARGIN_SMALL),
                Justification::bottomLeft, false);

    // Gesture Name text
    g.setColour (neova_dash::colour::mainText);                    
    g.setFont (Font().boldened().withHeight (15.0f));
    g.drawText (neova_dash::gesture::getTypeString (hubConfig.getGestureData (gestureId)
		                                                     .type, true).toUpperCase(),
                headerArea.removeFromLeft (getWidth()/3).reduced (MARGIN, 0),
                Justification::centred, false);
}

void GestureSettingsComponent::paintBackground (Graphics& g)
{
    using namespace neova_dash::ui;
    auto area = getLocalBounds().withBottom (midiPanel->getBottom());

    // MidiMap Background
    g.setColour (neova_dash::colour::gestureBackground2);
    g.fillRoundedRectangle (area.withTop (midiPanel->getY()).toFloat(), 10.0f);

    // Tuner backGround
    g.setColour (neova_dash::colour::gestureBackground);
    g.fillRoundedRectangle (area.withBottom (midiPanel->getY() + HEADER_HEIGHT)
                                .toFloat(),
                            10.0f);

    Path tile;
    tile.addEllipse (0, 0, 2, 2);

    neova_dash::ui::paintTiledPath (g, tile,
                                    area.withBottom (midiPanel->getY() + HEADER_HEIGHT)
                                    .toFloat(),
                                    15.0f, 15.0f, neova_dash::colour::gestureBackground2, Colour (0), 0.0f);

    g.saveState();

    //Header Fill
    g.reduceClipRegion (0, 0, getWidth(), 30);
    g.setColour (neova_dash::colour::gestureHeader);
    g.fillRoundedRectangle (area.withBottom (midiPanel->getY() + HEADER_HEIGHT)
                                .toFloat(),
                            10.0f);

    g.restoreState();
}

void GestureSettingsComponent::resized()
{
    using namespace neova_dash::ui;

    auto area = getLocalBounds();
    auto headerArea = area.removeFromTop (HEADER_HEIGHT).reduced (MARGIN_SMALL);
    headerArea.removeFromRight (25);

    //muteButton->setBounds (headerArea.removeFromRight (30).withSizeKeepingCentre (18, 18));

    midiPanel->setBounds (area.removeFromBottom (getHeight()/2 - HEADER_HEIGHT));
    area.removeFromBottom (MARGIN);
    //gestTuner->setBounds (area);
}

void GestureSettingsComponent::updateDisplay()
{
}

void GestureSettingsComponent::updateComponents()
{
}