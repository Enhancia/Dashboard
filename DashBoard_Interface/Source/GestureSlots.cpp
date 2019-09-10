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

GestureComponent::GestureComponent (HubConfiguration& hubCfg, const int gestNum,
                                                              const bool& dragModeReference,
                                                              const int& draggedGestureReference,
                                                              const int& draggedOverSlotReference)
    : hubConfig (hubCfg),
      id (gestNum), type (hubCfg.getGestureData (id).type),
      dragMode (dragModeReference),
      draggedGesture (draggedGestureReference),
      draggedOverSlot (draggedOverSlotReference)
{
    createLabel();
    createButton();
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
		                (static_cast <neova_dash::gesture::GestureType>
							(hubConfig.getGestureData (draggedGesture).type)));

        g.drawRoundedRectangle (getLocalBounds().reduced (1.0f).toFloat(), 10.0f, 3.0f);
    }
    else if (selected)
    {
        g.setColour (neova_dash::gesture::getHighlightColour (static_cast <neova_dash::gesture::GestureType> (type)));
        g.drawRoundedRectangle (getLocalBounds().reduced (1.0f).toFloat(), 10.0f, 1.0f);
    }

    auto area = getLocalBounds().withTrimmedTop (30);

    // Bottom display
    auto stateArea = area.removeFromBottom (25)
                         .reduced (neova_dash::ui::MARGIN*3, neova_dash::ui::MARGIN_SMALL);

    g.setFont (Font().withHeight (12.0f));
    g.setColour (neova_dash::colour::subText);
    
    g.drawText ((   hubConfig.getGestureData (id).type == neova_dash::gesture::pitchBend
                 || hubConfig.getGestureData (id).type == neova_dash::gesture::vibrato)
                    ?  "Pitch MIDI" : "CC " + String (hubConfig.getGestureData (id).cc) + " MIDI",
                    
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
    addAndMakeVisible (gestureNameLabel = new Label ("gestureNameLabel",
		                                             neova_dash::gesture::getTypeString
													     (neova_dash::gesture::intToGestureType(type),
														  true).toUpperCase()));

    gestureNameLabel->setEditable (false, false, false);
    gestureNameLabel->setColour (Label::backgroundColourId, Colour (0x00000000));
    gestureNameLabel->setColour (Label::textColourId, neova_dash::colour::mainText);
    gestureNameLabel->setFont (Font().withHeight (15.0f));
    gestureNameLabel->setJustificationType (Justification::centred);
    gestureNameLabel->setInterceptsMouseClicks (false, false);
}

void GestureComponent::createButton()
{
    addAndMakeVisible (muteButton = new ShapeButton ("Mute Button",
                                                     Colour (0),
                                                     Colour (0),
                                                     Colour (0)));

    // TO DELETE
    Path muteButtonPath;
    muteButtonPath.addRectangle (Rectangle<float> (10.0f, 10.0f));

	using namespace neova_dash;

    muteButton->setShape (muteButtonPath, false, true, false);
    muteButton->setOutline (gesture::getHighlightColour (gesture::intToGestureType (type)), 1.5f);
    muteButton->setToggleState (hubConfig.getGestureData (id).on, dontSendNotification);
    muteButton->setClickingTogglesState (true);
    muteButton->onClick = [this] ()
    {
        hubConfig.setUint8ValueAndUpload (id, HubConfiguration::on, uint8 (muteButton->getToggleState() ? 1 : 0));
    
        if (selected)
        {
            if (auto* closeButton = dynamic_cast<Button*> (getParentComponent()
    														  ->findChildWithID ("Close Button")))
    			closeButton->setToggleState (hubConfig.getGestureData (id).on, dontSendNotification);
        }

        repaint();
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
        g.fillRoundedRectangle (getLocalBounds().toFloat(), 10.0f);
    }

    if (dragMode && draggedGesture != id && draggedOverSlot == id)
    {
        g.setColour (neova_dash::gesture::getHighlightColour
                        (neova_dash::gesture::intToGestureType (hubConfig.getGestureData (draggedGesture).type)));
    }
    else
    {
        g.setColour (neova_dash::colour::emptySlotOutline);
    }

    // Plus Icon
    g.strokePath (plusIcon, {2.0f, PathStrokeType::mitered, PathStrokeType::rounded});

    
    // Outline
    if (dragMode && draggedGesture != id && draggedOverSlot == id)
    {
        g.setColour (neova_dash::gesture::getHighlightColour
                        (neova_dash::gesture::intToGestureType (hubConfig.getGestureData (draggedGesture).type)));
		g.drawRoundedRectangle(getLocalBounds().reduced (1.0f).toFloat(), 10.0f, 3.0f);
    }

    else //if (highlighted)
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