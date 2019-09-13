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
	createTuner();
	createToggles();
	midiPanel = std::make_unique<Component>("Midi Panel");
	addAndMakeVisible (*midiPanel);

}

GestureSettingsComponent::~GestureSettingsComponent()
{
	//midiPanel = nullptr;
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

    // Tuner backGround
    g.setColour (neova_dash::colour::gestureBackground);
    g.fillRoundedRectangle (area.toFloat(), 10.0f);

    // MidiMap Background
    g.saveState();
    g.reduceClipRegion (0, midiPanel->getY(), getWidth(), getLocalBounds().getBottom());
    g.setColour (neova_dash::colour::gestureBackground2);
    g.fillRoundedRectangle (area.toFloat(), 10.0f);
    g.restoreState();

    Path tile;
    tile.addEllipse (0, 0, 2, 2);

    neova_dash::ui::paintTiledPath (g, tile,
                                    area.withBottom (midiPanel->getY())
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
    gestTuner->setBounds (area);

	repaint();
}

void GestureSettingsComponent::updateDisplay()
{
    if (!disabled) disableIfGestureWasDeleted();

    if (!disabled)
    {
        if (hubConfig.getGestureData (gestureId).on != 0) gestTuner->updateDisplay();
        
        //midiPanel->updateDisplay();
    }
}

void GestureSettingsComponent::disableIfGestureWasDeleted()
{
    if (hubConfig.getGestureData (gestureId).type == uint8 (neova_dash::gesture::none))
    {
        disabled = true;
    }
}

void GestureSettingsComponent::updateComponents()
{
}

void GestureSettingsComponent::createTuner()
{
	if (hubConfig.getGestureData (gestureId).type == uint8 (neova_dash::gesture::vibrato))
    {
        gestTuner = std::make_unique <VibratoTuner> (hubConfig, gestureId);
        addAndMakeVisible (*gestTuner);
    }
    /*
    else if (hubConfig.getGestureData (gestureId).type == uint8 (neova_dash::gesture::pitchBend))
    {
        gestTuner = std::make_unique <PitchBendTuner> (hubConfig, gestureId);
        addAndMakeVisible (*gestTuner);
    }
    
    else if (hubConfig.getGestureData (gestureId).type == uint8 (neova_dash::gesture::tilt))
    {
        gestTuner = std::make_unique <TiltTuner> (hubConfig, gestureId);
        addAndMakeVisible (*gestTuner);
    }
    /*  Un-comment when the wave gesture is implemented
    else if (hubConfig.getGestureData (gestureId).type == uint8 (neova_dash::gesture::wave))
    {
        gestTuner = std::make_unique <WaveTuner> (hubConfig, gestureId);
        addAndMakeVisible (*gestTuner);
    }
    */
    /*
    else if (hubConfig.getGestureData (gestureId).type == uint8 (neova_dash::gesture::roll))
    {
        gestTuner = std::make_unique <RollTuner> (hubConfig, gestureId);
        addAndMakeVisible (*gestTuner);
    }
    */
    else
    {
        DBG ("Unknown Gesture type. No tuner was created.");
        return;
    }

    gestTuner->updateColour();
}

void GestureSettingsComponent::createToggles()
{
	/*
	addAndMakeVisible (muteButton = new PlumeShapeButton ("Mute Button",
                                                          getPlumeColour (plumeBackground),
                                                          getPlumeColour (mutedHighlight),
                                                          Gesture::getHighlightColour (gesture.type)));

    muteButton->setShape (PLUME::path::createPath (PLUME::path::onOff), false, true, false);
    muteButton->setToggleState (gesture.isActive(), dontSendNotification);
    muteButton->setClickingTogglesState (true);
    muteButton->onClick = [this] ()
    {
        gesture.setActive (muteButton->getToggleState());
        closeButton.setToggleState (gesture.isActive(), dontSendNotification);
    };*/
}

Tuner& GestureSettingsComponent::getTuner()
{
	return *gestTuner;
}