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

GestureComponent::GestureComponent (Gesture& gest, GestureArray& gestArray,
                                    const bool& dragModeReference,
                                    const int& draggedGestureReference,
                                    const int& draggedOverSlotReference)
    : gesture (gest), gestureArray (gestArray),
      id (gest.id),
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
    return gesture.getName() + " | " + gesture.getTypeString (true) + "\n\n" +
           "State : " + (gesture.isActive() ? "Enabled" : "Disabled") +
           " | Mode : " + (gesture.generatesMidi() ? "MIDI\n" : "Parameters\n")
           + "\n" + gesture.getDescription();
}

void GestureComponent::update()
{
    gestureNameLabel->setText (gesture.getName().toUpperCase(), sendNotification);
    muteButton->setToggleState (gesture.isActive(), sendNotification);
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
        g.setColour (gestureArray.getGesture (draggedGesture)->getHighlightColour());
        g.drawRoundedRectangle (getLocalBounds().reduced (1.0f).toFloat(), 10.0f, 3.0f);
    }
    else if (selected)
    {
        g.setColour (gesture.getHighlightColour());
        g.drawRoundedRectangle (getLocalBounds().reduced (1.0f).toFloat(), 10.0f, 1.0f);
    }

    auto area = getLocalBounds().withTrimmedTop (30);

    // Bottom display
    auto stateArea = area.removeFromBottom (25)
                         .reduced (PLUME::UI::MARGIN*3, PLUME::UI::MARGIN_SMALL);

    g.setFont (PLUME::font::plumeFontLight.withHeight (12.0f));
    g.setColour (getPlumeColour (basePanelSubText));
    
    if (gesture.generatesMidi())
    {
        g.drawText (gesture.midiType == Gesture::pitch ? "Pitch MIDI"
                                                       : "CC " + String (gesture.getCc()) + " MIDI",
                    stateArea, Justification::centred, true);
    }
    else
    {
        paintParameterSlotDisplay (g, stateArea, 1, 6, PLUME::UI::MARGIN);
    }
    
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
    muteButton->setBounds (headerArea.removeFromRight (30 + PLUME::UI::MARGIN)
                                     .withSizeKeepingCentre (18, 18));
}
void GestureComponent::editorShown (Label* lbl, TextEditor& ted)
{
    ted.setColour (TextEditor::highlightColourId, Colour (0xff101010));
    ted.setColour (TextEditor::textColourId, Colour (0xff959595));
    ted.setJustification (Justification::centred);
}

void GestureComponent::labelTextChanged (Label* lbl)
{
    gesture.setName (gestureNameLabel->getText());
    gestureNameLabel->setText (gesture.getName(), dontSendNotification);
    
    dynamic_cast<PlumeComponent*> (getParentComponent())->update();
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

Gesture& GestureComponent::getGesture()
{
    return gesture;
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
    addAndMakeVisible (gestureNameLabel = new Label ("gestureNameLabel", gesture.getName().toUpperCase()));
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
                                                          getPlumeColour (plumeBackground),
                                                          getPlumeColour (mutedHighlight),
                                                          Gesture::getHighlightColour (gesture.type)));

    muteButton->setShape (PLUME::path::createPath (PLUME::path::onOff), false, true, false);
    muteButton->setToggleState (gesture.isActive(), dontSendNotification);
    muteButton->setClickingTogglesState (true);
    muteButton->onClick = [this] ()
    { 
        gesture.setActive (muteButton->getToggleState());
        
        if (selected)
        {
            if (auto* closeButton = dynamic_cast<Button*> (getParentComponent()
    														  ->findChildWithID ("Close Button")))
    			closeButton->setToggleState (gesture.isActive(), dontSendNotification);
        }

        repaint();
    };
}

void GestureComponent::paintParameterSlotDisplay  (Graphics& g, juce::Rectangle<int> area,
                                                            const int numRows,
                                                            const int numColumns,
                                                            const int sideLength)
{
    /*  Hitting this assert means you're trying to paint this object with a number of
        parameters that doesn't match the actual maximum number of parameters allowed
        for a gesture.
    */
    jassert (numRows * numColumns == PLUME::MAX_PARAMETER);

    int rowHeight = area.getHeight()/numRows;
    int columnWidth = area.getWidth()/numColumns;

    for (int row=0; row < numRows; row++)
    {
        auto columnArea = area.removeFromTop (rowHeight);

        for (int column=0; column < numColumns; column++)
        {
            int slotSide = jmin (sideLength, rowHeight - 8, columnWidth - 8);
            auto slotArea = columnArea.removeFromLeft (columnWidth)
                                      .withSizeKeepingCentre (slotSide, slotSide);

            g.setColour ((row*numColumns) + column < gesture.getParameterArray().size() ?
                            gesture.getHighlightColour() :
                            getPlumeColour (plumeBackground));
            g.fillRoundedRectangle (slotArea.toFloat(), sideLength / 3.5f);
        }
    }
}

void GestureComponent::drawGesturePath (Graphics& g, juce::Rectangle<int> area)
{
    g.setColour (Colour (0xfff3f3f3));

    // Icon Fill
    Path iconFill;

    if (gesture.type == Gesture::tilt) iconFill = PLUME::path::createPath (PLUME::path::handTilt);
    else if (gesture.type == Gesture::roll) iconFill = PLUME::path::createPath (PLUME::path::handRoll);
    else iconFill = PLUME::path::createPath (PLUME::path::handFingerDown);

    auto areaFloat = (gesture.type == Gesture::tilt || gesture.type == Gesture::roll)
                          ? area.reduced (area.getWidth()/8, area.getHeight()/4).toFloat()
                          : area.reduced (area.getWidth()/4, area.getHeight()/8).toFloat();

	iconFill.scaleToFit (areaFloat.getX(), areaFloat.getY(),
                         areaFloat.getWidth(), areaFloat.getHeight(), true);

    g.fillPath (iconFill);

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

EmptyGestureSlotComponent::EmptyGestureSlotComponent (const int slotId,
                                                      GestureArray& gestArray,
                                                      const bool& dragModeReference,
                                                      const int& draggedGestureReference,
                                                      const int& draggedOverSlotReference)
    : id (slotId), gestureArray (gestArray),
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
        g.setColour (gestureArray.getGesture (draggedGesture)->getHighlightColour());
    }
    else
    {
        g.setColour (getPlumeColour (emptySlotOutline));
    }

    // Plus Icon
    g.strokePath (plusIcon, {2.0f, PathStrokeType::mitered, PathStrokeType::rounded});

    
    // Outline
    if (dragMode && draggedGesture != id && draggedOverSlot == id)
    {
        g.setColour (gestureArray.getGesture (draggedGesture)->getHighlightColour());
		g.drawRoundedRectangle(getLocalBounds().reduced (1.0f).toFloat(), 10.0f, 3.0f);
    }

    else //if (highlighted)
    {
        g.setColour (getPlumeColour (emptySlotOutline));
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