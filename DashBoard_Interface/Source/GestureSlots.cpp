/*
  ==============================================================================

    GestureSlots.cpp
    Created: 6 Sep 2019 8:05:19pm
    Author:  aleva

  ==============================================================================
*/

#include "GestureSlots.h"


#include "GesturePanelSlots.h"

//==============================================================================
// Gesture Component

GestureComponent::GestureComponent (HubConfiguration& hubCfg, const int gestNum,
                                                              const bool& dragModeReference,
                                                              const int& draggedGestureReference,
                                                              const int& draggedOverSlotReference)
    : hubConfig (hubCfg),
      id (gestNum), type (hubConfig.getGestureData (id).type)
      dragMode (dragModeReference),
      draggedGesture (draggedGestureReference),
      draggedOverSlot (draggedOverSlotReference)
{
    createLabel();
    createButton();
}

GestureComponent::~GestureComponent()
{
    gestureNameLabel->removeListener (this);
    gestureNameLabel = nullptr;
    muteButton = nullptr;
}

const String GestureComponent::getInfoString()
{
    return "Type : " + neova_dash::gesture::getTypeString (type, true) + "\n\n" +
           "State : " + (hubConfig.getGestureData (id).on ? "Enabled" : "Disabled")
}

void GestureComponent::update()
{
    gestureNameLabel->setText (neova_dash::gesture::getTypeString (type, true).toUpperCase(), sendNotification);
    muteButton->setToggleState (bool (hubConfig.getGestureData (id)), sendNotification);
    repaint();
}

void GestureComponent::paint (Graphics& g)
{
    // BackGround + Header Fill
    g.setColour (getPlumeColour (basePanelBackground));
    g.fillRoundedRectangle (getLocalBounds().toFloat(), 10.0f);

    g.saveState();
    g.reduceClipRegion (0, 0, getWidth(), 30);
    g.setColour (getPlumeColour (basePanelHeaderFill));
    g.fillRoundedRectangle (getLocalBounds().toFloat(), 10.0f);
    g.restoreState();

    // Outline
    if (dragMode && draggedGesture != id && draggedOverSlot == id)
    {
        g.setColour (neova_dash::gesture::getGestureColour (hubConfig.getGestureData (draggedGesture).type));
        g.drawRoundedRectangle (getLocalBounds().reduced (1.0f).toFloat(), 10.0f, 3.0f);
    }
    else if (selected)
    {
        g.setColour (neova_dash::gesture::getGestureColour (type));
        g.drawRoundedRectangle (getLocalBounds().reduced (1.0f).toFloat(), 10.0f, 1.0f);
    }

    auto area = getLocalBounds().withTrimmedTop (30);

    // Bottom display
    auto stateArea = area.removeFromBottom (25)
                         .reduced (neova_dash::ui::MARGIN*3, neova_dash::ui::MARGIN_SMALL);

    g.setFont (neova_dash::font::dashFont.withHeight (12.0f));
    g.setColour (neova_dash::colour::subText);
    
    g.drawText (hubConfig.getGestureData (id).type == neova_dash::gesture::pitchBend || neova_dash::gesture::vibrato
                                                        ?  "Pitch MIDI"
                                                        : "CC " + String (hubConfig.getGestureData (id).cc) + " MIDI",
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
    addAndMakeVisible (gestureNameLabel = new Label ("gestureNameLabel", hubConfig.getTypeString (type).toUpperCase()));
    gestureNameLabel->setEditable (false, false, false);
    gestureNameLabel->setColour (Label::backgroundColourId, Colour (0x00000000));
    gestureNameLabel->setColour (Label::textColourId, getPlumeColour (basePanelMainText));
    gestureNameLabel->setFont (PLUME::font::plumeFontBold.withHeight (15.0f));
    gestureNameLabel->setJustificationType (Justification::centred);
    gestureNameLabel->setInterceptsMouseClicks (false, false);
    gestureNameLabel->addListener (this);
}

void GestureComponent::createButton()
{
    addAndMakeVisible (muteButton = new PlumeShapeButton ("Mute Button",
                                                          neova_dash::colour::dashboardBackground,
                                                          neova_dash::colour::inactiveGesture,
                                                          neova_dash::gesture::getHighlightColour (type)));

    muteButton->setShape (PLUME::path::createPath (PLUME::path::onOff), false, true, false);
    muteButton->setToggleState (hubConfig.getGestureData (id).on, dontSendNotification);
    muteButton->setClickingTogglesState (true);
    muteButton->onClick = [this] ()
    {
        hubConfig.setUint8ValueandUpload (id, HubConfiguration::on, uint8 (muteButton->getToggleState() ? 1 : 0));
    
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
                                                                                const int& draggedOverSlotReference);
    : id (slotId), hubConfig (hubCfg),
                   dragMode (dragModeReference),
                   draggedGesture (draggedGestureReference),
                   draggedOverSlot (draggedOverSlotReference)
{
}
EmptyGestureSlotComponent::~EmptyGestureSlotComponent()
{
}

const String EmptyGestureSlotComponent::getInfoString()
{
    return String();
}
void EmptyGestureSlotComponent::update()
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
        g.setColour (getPlumeColour (emptySlotBackground));
        g.fillRoundedRectangle (getLocalBounds().toFloat(), 10.0f);
    }


    if (dragMode && draggedGesture != id && draggedOverSlot == id)
    {
        g.setColour (neova_dash::gesture::getHighlightColour (hubConfig.getGestureData (draggedGesture).type));
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
        g.setColour (getHighlightColour (hubConfig.getGestureData (draggedGesture).type));
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