/*
  ==============================================================================

    GestureSettingsComponent.cpp
    Created: 6 Sep 2019 8:05:32pm
    Author:  aleva

  ==============================================================================
*/

#include "GestureSettingsComponent.h"

GestureSettingsComponent::GestureSettingsComponent (const int gestId, HubConfiguration& config,
                                                    ApplicationCommandManager& manager, DataReader& reader)
	: gestureId (gestId), hubConfig (config), commandManager (manager), dataReader (reader)
{
    TRACE_IN;

	createTuner();
	createToggles();
    createMidiPanel();
}

GestureSettingsComponent::~GestureSettingsComponent()
{
    TRACE_IN;

    gestTuner = nullptr;
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
    const int advTextAreaWidth = getHeight()/3 - MARGIN;
    const int stringWidth = neova_dash::font::dashFontLight.withHeight (11.0f)
                                                           .getStringWidth ("ADVANCED PANEL");

    float advTextHeight = advTextAreaWidth > stringWidth ? 11.0f
                                                         : 11.0f*advTextAreaWidth/stringWidth;

    g.setFont (neova_dash::font::dashFontLight.withHeight (advTextHeight));
    g.drawText ("ADVANCED PANEL",
                headerArea.removeFromLeft (getWidth()/3)
                          .reduced (0, MARGIN_SMALL)
                          .withTrimmedLeft (MARGIN),
                Justification::bottomLeft, false);

    // Gesture Name text
    g.setColour (neova_dash::colour::mainText);                    
    g.setFont (neova_dash::font::dashFontBold.withHeight (15.0f));
    g.drawText (neova_dash::gesture::getTypeString (hubConfig.getGestureData (gestureId)
		                                                     .type, true).toUpperCase(),
                headerArea.removeFromLeft (getWidth()/3),
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

    muteButton->setBounds (headerArea.removeFromRight (30).withSizeKeepingCentre (18, 18));
    midiPanel->setBounds (area.removeFromBottom (jmax (getHeight()/4, 80)));

    if (gestTuner != nullptr) gestTuner->setBounds (area);

	repaint();
}

void GestureSettingsComponent::updateDisplay()
{
    if (!disabled) disableIfGestureWasDeleted();

    if (!disabled)
    {
        if (gestTuner != nullptr)
        {
            gestTuner->updateDisplay();
        }
        
        midiPanel->updateDisplay();
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
	if (gestTuner != nullptr) gestTuner->updateColour();
}

void GestureSettingsComponent::createTuner()
{
	if (hubConfig.getGestureData (gestureId).type == uint8 (neova_dash::gesture::vibrato))
    {
        gestTuner = std::make_unique <VibratoTuner> (hubConfig, dataReader, gestureId);
        addAndMakeVisible (*gestTuner);
    }
    else if (hubConfig.getGestureData (gestureId).type == uint8 (neova_dash::gesture::pitchBend))
    {
        gestTuner = std::make_unique <PitchBendTuner> (hubConfig, dataReader, gestureId);
        addAndMakeVisible (*gestTuner);
    }
    
    else if (hubConfig.getGestureData (gestureId).type == uint8 (neova_dash::gesture::tilt))
    {
        gestTuner = std::make_unique <TiltTuner> (hubConfig, dataReader, gestureId);
        addAndMakeVisible (*gestTuner);
    }
    /*  Un-comment when the wave gesture is implemented
    else if (hubConfig.getGestureData (gestureId).type == uint8 (neova_dash::gesture::wave))
    {
        gestTuner = std::make_unique <WaveTuner> (hubConfig, dataReader, gestureId);
        addAndMakeVisible (*gestTuner);
    }
    */
    else if (hubConfig.getGestureData (gestureId).type == uint8 (neova_dash::gesture::roll))
    {
        gestTuner = std::make_unique <RollTuner> (hubConfig, dataReader, gestureId);
        addAndMakeVisible (*gestTuner);
    }
    else
    {
        DBG ("Unknown Gesture type. No tuner was created.");
        return;
    }

    gestTuner->updateColour();
}

void GestureSettingsComponent::createToggles()
{
    muteButton = std::make_unique<DashShapeButton> ("Mute Button",
                                                    neova_dash::colour::dashboardBackground,
                                                    neova_dash::gesture::getHighlightColour (hubConfig.getGestureData (gestureId)
                                                                                                      .type),
                                                    neova_dash::gesture::getHighlightColour (hubConfig.getGestureData (gestureId)
                                                                                                      .type, false));
    addAndMakeVisible (*muteButton);

    muteButton->setShape (neova_dash::path::createPath (neova_dash::path::onOff), false, true, false);
    muteButton->setToggleState (!hubConfig.isGestureActive (gestureId), dontSendNotification);
    muteButton->setClickingTogglesState (true);
    muteButton->onClick = [this] ()
    {
        if (hubConfig.getGestureData (gestureId).type != uint8 (neova_dash::gesture::none))
        {
            hubConfig.setUint8Value (gestureId, HubConfiguration::on, uint8 (muteButton->getToggleState() ? 0 : 1));
            commandManager.invokeDirectly (neova_dash::commands::updateDashInterface, true);
        }
    };
}

void GestureSettingsComponent::createMidiPanel()
{
    midiPanel = std::make_unique<MidiPanel> (hubConfig, dataReader, gestureId);
    addAndMakeVisible (*midiPanel);
}

Tuner& GestureSettingsComponent::getTuner()
{
	return *gestTuner;
}