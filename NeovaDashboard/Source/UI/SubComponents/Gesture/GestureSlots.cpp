/*
  ==============================================================================

    GestureSlots.cpp
    Created: 6 Sep 2019 8:05:19pm
    Author:  aleva

  ==============================================================================
*/

#include "GestureSlots.h"

//==============================================================================
// Gesture Component

GestureComponent::GestureComponent (HubConfiguration& hubCfg, ApplicationCommandManager& manager,
                                                              const int gestNum,
                                                              const bool& dragModeReference,
                                                              const int& draggedGestureReference,
                                                              const int& draggedOverSlotReference)
    : hubConfig (hubCfg), commandManager (manager),
      id (gestNum), type (hubCfg.getGestureData (id).type),
      dragMode (dragModeReference),
      draggedGesture (draggedGestureReference),
      draggedOverSlot (draggedOverSlotReference)
{
    createLabel();
    createButton();
	selected = hubConfig.getSelectedGesture() == id;
}

GestureComponent::~GestureComponent()
{
    gestureNameLabel = nullptr;
    muteButton = nullptr;
}

void GestureComponent::paint (Graphics& g)
{
    // BackGround + Header Fill
    g.setColour (neova_dash::colour::gestureBackground);
    g.fillRoundedRectangle (getLocalBounds().toFloat(), 10.0f);

    g.saveState();
    g.reduceClipRegion (0, 0, getWidth(), 30);
    g.setColour (neova_dash::colour::gestureHeader);
    g.fillRoundedRectangle (getLocalBounds().toFloat(), 10.0f);
    g.restoreState();

    // Outline
    if (dragMode && draggedGesture != id && draggedOverSlot == id)
    {
        g.setColour (neova_dash::gesture::getHighlightColour
		                (hubConfig.getGestureData (draggedGesture).type,
                         hubConfig.isGestureActive (draggedGesture)));

        g.drawRoundedRectangle (getLocalBounds().reduced (1.0f).toFloat(), 10.0f, 3.0f);
    }
    else if (selected)
    {
        g.setColour (neova_dash::gesture::getHighlightColour (type, hubConfig.isGestureActive (id)));
        g.drawRoundedRectangle (getLocalBounds().reduced (1.0f).toFloat(), 10.0f, 1.0f);
    }

    auto area = getLocalBounds().withTrimmedTop (30);

    // Bottom display
    auto stateArea = area.removeFromBottom (25)
                         .reduced (neova_dash::ui::MARGIN*3, neova_dash::ui::MARGIN_SMALL);

    g.setFont (neova_dash::font::dashFont.withHeight (13.0f));
    g.setColour (neova_dash::colour::subText);
    
    g.drawText (hubConfig.getGestureData (id).midiType == neova_dash::gesture::pitchMidi
                    ? "Pitch MIDI"
                    : hubConfig.getGestureData (id).midiType == neova_dash::gesture::ccMidi
                        ? "CC " + String (hubConfig.getGestureData (id).cc) + " MIDI"
                        : hubConfig.getGestureData (id).midiType == neova_dash::gesture::afterTouchMidi
                            ? "Aftertouch Midi" : "Unknown MIDI",
                stateArea, Justification::centred, true);
    
    // Gesture Image
    drawGesturePath (g, area);

    // Highlight
    if (!selected && highlighted)
    {
        g.setColour (Colours::white.withAlpha (0.05f));
        g.fillRoundedRectangle (getLocalBounds().toFloat(), 10.0f);
    }
}

void GestureComponent::resized()
{
    auto headerArea = getLocalBounds().removeFromTop (30);

    gestureNameLabel->setBounds (headerArea.withSizeKeepingCentre (getWidth()*2/3, 25));
    const int stringWidth = gestureNameLabel->getFont().getStringWidth (gestureNameLabel->getText());

    if (stringWidth/2 > getWidth()/2-30)
    {
        gestureNameLabel->setBounds (gestureNameLabel->getBounds()
                                                      .translated (-(gestureNameLabel->getRight()-getWidth()+30), 0));
    }

    if (gestureNameLabel->getWidth() < stringWidth)
    {
        gestureNameLabel->setFont (gestureNameLabel->getFont().getHeight()
                                        *gestureNameLabel->getWidth()/stringWidth);
    }

    muteButton->setBounds (headerArea.removeFromRight (30 + neova_dash::ui::MARGIN)
                                     .withSizeKeepingCentre (18, 18));
}

void GestureComponent::mouseEnter (const MouseEvent &event)
{
    setHighlighted (true);
}
void GestureComponent::mouseExit (const MouseEvent &event)
{
    setHighlighted (false);
}
void GestureComponent::mouseDrag (const MouseEvent &event)
{
}

bool GestureComponent::isSelected() const
{
    return selected;
}
void GestureComponent::setSelected (bool shouldBeSelected)
{
    selected = shouldBeSelected;
    repaint();
}
void GestureComponent::setHighlighted (bool shouldBeHighlighted)
{
    highlighted = shouldBeHighlighted;
    repaint();
}
void GestureComponent::setSolo (bool shouldBeSolo)
{
    solo = shouldBeSolo;
    repaint();
}
void GestureComponent::startNameEntry()
{
    gestureNameLabel->showEditor();
}

void GestureComponent::createLabel()
{
	gestureNameLabel = std::make_unique<Label> ("gestureNameLabel",
                                                neova_dash::gesture::getTypeString
                                                   (neova_dash::gesture::intToGestureType(type),
                                                    true).toUpperCase());

    addAndMakeVisible (*gestureNameLabel);

    gestureNameLabel->setEditable (false, false, false);
    gestureNameLabel->setColour (Label::backgroundColourId, Colour (0x00000000));
    gestureNameLabel->setColour (Label::textColourId, neova_dash::colour::mainText);
    gestureNameLabel->setFont (neova_dash::font::dashFont.withHeight (15.0f));
    gestureNameLabel->setJustificationType (Justification::centred);
    gestureNameLabel->setInterceptsMouseClicks (false, false);
}

void GestureComponent::createButton()
{
    muteButton = std::make_unique<DashShapeButton> ("Mute Button",
                                                    neova_dash::colour::dashboardBackground,
                                                    neova_dash::gesture::getHighlightColour (hubConfig.getGestureData (id)
                                                                                                      .type),
                                                    neova_dash::colour::inactiveGesture);
    addAndMakeVisible (*muteButton);

	using namespace neova_dash;

    muteButton->setShape (path::createPath (path::onOff), false, true, false);
    muteButton->setOutline (gesture::getHighlightColour (type, hubConfig.isGestureActive (id)),
                            1.5f);
    muteButton->setToggleState (!hubConfig.isGestureActive (id), dontSendNotification);
    muteButton->setClickingTogglesState (true);
    muteButton->onClick = [this] ()
    {
        hubConfig.setUint8Value (id, HubConfiguration::on, uint8 (muteButton->getToggleState() ? 0 : 1));
        commandManager.invokeDirectly (neova_dash::commands::updateDashInterface, true);
    };
}

void GestureComponent::drawGesturePath (Graphics& g, juce::Rectangle<int> area)
{
    g.setColour (Colour (0xfff3f3f3));

    // Icon Fill
    Path iconFill;

    /*
    if (type == Gesture::tilt) iconFill = PLUME::path::createPath (PLUME::path::handTilt);
    else if (type == Gesture::roll) iconFill = PLUME::path::createPath (PLUME::path::handRoll);
    else iconFill = PLUME::path::createPath (PLUME::path::handFingerDown);

    auto areaFloat = (type == Gesture::tilt || type == Gesture::roll)
                          ? area.reduced (area.getWidth()/8, area.getHeight()/4).toFloat()
                          : area.reduced (area.getWidth()/4, area.getHeight()/8).toFloat();

	iconFill.scaleToFit (areaFloat.getX(), areaFloat.getY(),
                         areaFloat.getWidth(), areaFloat.getHeight(), true);

    g.fillPath (iconFill);
    */
    // Icon stroke
    /*
    Path iconStroke;

    if (gesture.type == Gesture::tilt)
    {
        iconStroke = PLUME::path::createPath (PLUME::path::tiltArrow);
        areaFloat = areaFloat.withTrimmedLeft (areaFloat.getWidth()*2/3)
                             .withTrimmedBottom (areaFloat.getHeight()/2);
    }
    else if (gesture.type == Gesture::roll)
    {
        iconStroke = PLUME::path::createPath (PLUME::path::rollArrow);
        areaFloat = areaFloat.withTrimmedRight (areaFloat.getWidth()/2)
                             .withTrimmedBottom (areaFloat.getHeight()/2);
    }
    else if (gesture.type == Gesture::pitchBend)
    {
        iconStroke = PLUME::path::createPath (PLUME::path::pitchBendArrow);
        areaFloat = areaFloat.withTrimmedTop (areaFloat.getHeight()*2/3)
                             .translated (areaFloat.getWidth()/12, 0);
    }
    else if (gesture.type == Gesture::vibrato)
    {
        iconStroke = PLUME::path::createPath (PLUME::path::vibratoRipple);
        areaFloat = areaFloat.withTrimmedTop (areaFloat.getHeight()*2/3)
                             .translated (areaFloat.getWidth()/12, 0);
    }
    else return;

    iconStroke.scaleToFit (areaFloat.getX(), areaFloat.getY(),
                           areaFloat.getWidth(), areaFloat.getHeight(), true);
    g.strokePath (iconStroke, PathStrokeType (1.0f));*/
}

//==============================================================================
// Gesture Slot 

EmptyGestureSlotComponent::EmptyGestureSlotComponent (HubConfiguration& hubCfg, const int slotId,
                                                                                const bool& dragModeReference,
                                                                                const int& draggedGestureReference,
                                                                                const int& draggedOverSlotReference)
    : id (slotId), hubConfig (hubCfg),
                   dragMode (dragModeReference),
                   draggedGesture (draggedGestureReference),
                   draggedOverSlot (draggedOverSlotReference)
{
}
EmptyGestureSlotComponent::~EmptyGestureSlotComponent()
{
}

void EmptyGestureSlotComponent::paint (Graphics& g)
{
    Path outline, plusIcon;

    outline.addRoundedRectangle (getLocalBounds().reduced (1.0f), 10.0f);

    plusIcon.startNewSubPath ({getWidth()/2.0f - 10.0f, getHeight()/2.0f});
    plusIcon.lineTo          ({getWidth()/2.0f + 10.0f, getHeight()/2.0f});
    plusIcon.startNewSubPath ({getWidth()/2.0f, getHeight()/2.0f - 10.0f});
    plusIcon.lineTo          ({getWidth()/2.0f, getHeight()/2.0f + 10.0f});

    if (highlighted)
    {
        g.setColour (neova_dash::colour::emptySlotBackground);
        g.fillRoundedRectangle (getLocalBounds().reduced (1).toFloat(), 10.0f);
    }

    if (dragMode && draggedGesture != id && draggedOverSlot == id)
    {
        g.setColour (neova_dash::gesture::getHighlightColour
                        (hubConfig.getGestureData (draggedGesture).type,
                         hubConfig.isGestureActive (draggedGesture)));
    }
    else
    {
        g.setColour (neova_dash::colour::emptySlotOutline);
    }

    // Plus Icon
    g.strokePath (plusIcon, {2.0f, PathStrokeType::mitered, PathStrokeType::rounded});

    // Shadow
    Path shadowPath;
    shadowPath.addRectangle (getLocalBounds().expanded (3));
    shadowPath.addRoundedRectangle (getLocalBounds().reduced (5).toFloat(), 6.0f);
    shadowPath.setUsingNonZeroWinding (false);

    g.saveState();
    g.reduceClipRegion (outline);
    DropShadow (Colour (0x40000000), 15, {0, 0}).drawForPath (g, shadowPath);
    g.restoreState();
    
    // Outline
    if (dragMode && draggedGesture != id && draggedOverSlot == id)
    {
        g.setColour (neova_dash::gesture::getHighlightColour
                        (hubConfig.getGestureData (draggedGesture).type,
                         hubConfig.isGestureActive (draggedGesture)));
		g.strokePath(outline, PathStrokeType(3.0f));
    }
    else
    {
        g.setColour (neova_dash::colour::emptySlotOutline);
        PathStrokeType outlineStroke (1.0f, PathStrokeType::mitered, PathStrokeType::butt);
        Path dashedOutline;
		const float dashLengths[] = {5.0f, 5.0f};

        outlineStroke.createDashedStroke (dashedOutline, outline, dashLengths, 2);

        g.fillPath (dashedOutline);
    }
}

void EmptyGestureSlotComponent::resized()
{
}

void EmptyGestureSlotComponent::mouseEnter (const MouseEvent &event)
{
    highlighted = true;
    repaint();
}
void EmptyGestureSlotComponent::mouseExit (const MouseEvent &event)
{
    highlighted = false;
    repaint();
}
void EmptyGestureSlotComponent::mouseDrag (const MouseEvent &event)
{
}