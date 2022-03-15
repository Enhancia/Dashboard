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
    createButton();
	selected = hubConfig.getSelectedGesture() == id;
}

GestureComponent::~GestureComponent()
{
    muteButton = nullptr;
}

void GestureComponent::paint (Graphics& g)
{

    // BackGround + Header Fill
    g.setColour (neova_dash::colour::gestureBackground.brighter (0.1f));
    g.fillRoundedRectangle (getLocalBounds().toFloat(), 10.0f);

	/*
    g.saveState();
    g.reduceClipRegion (0, 0, getWidth(), 30);
    g.setColour (neova_dash::colour::gestureHeader);
    g.fillRoundedRectangle (getLocalBounds().toFloat(), 10.0f);
    g.restoreState();*/

	// Gesture Image
    Path roundedRectangleBackground;
    roundedRectangleBackground.addRoundedRectangle (selected ? getLocalBounds().toFloat().reduced (1.0f)
                                                             : getLocalBounds().toFloat(),
                                                    10.0f);

    g.saveState();
    g.reduceClipRegion (roundedRectangleBackground);
	drawGesturePath (g, getLocalBounds());
    g.restoreState();

    // Gesture Name
    g.setColour (neova_dash::colour::mainText);
    g.setFont (neova_dash::font::dashFont.withHeight (17.0f).withExtraKerningFactor (0.06f));
    g.drawText (neova_dash::gesture::getTypeString (neova_dash::gesture::intToGestureType(type), true).toUpperCase(),
                getLocalBounds(), Justification::centred);

    // Outline
    if (dragMode && draggedGesture != id && draggedOverSlot == id)
    {
        g.setColour (neova_dash::gesture::getHighlightColour
		                (hubConfig.getGestureData (draggedGesture).type,
                         hubConfig.isGestureActive (draggedGesture)));

        g.drawRoundedRectangle (getLocalBounds().reduced (1).toFloat(), 10.0f, 3.0f);
    }
    else if (selected)
    {
        g.setColour (neova_dash::gesture::getHighlightColour (type, hubConfig.isGestureActive (id)));
        g.drawRoundedRectangle (getLocalBounds().reduced (1).toFloat(), 10.0f, 1.0f);
    }

    auto area = getLocalBounds().withTrimmedTop (30);

    // Bottom display
    auto stateArea = area.removeFromBottom (25)
                         .reduced (neova_dash::ui::MARGIN*3, neova_dash::ui::MARGIN_SMALL);

    g.setFont (neova_dash::font::dashFont.withHeight (13.0f));
    g.setColour (neova_dash::colour::subText);
    
    g.drawText (hubConfig.getGestureData (id).midiType == neova_dash::gesture::pitchMidi
                    ? "Pitch"
                    : hubConfig.getGestureData (id).midiType == neova_dash::gesture::ccMidi
                        ? "CC " + String (hubConfig.getGestureData (id).cc)
                        : hubConfig.getGestureData (id).midiType == neova_dash::gesture::afterTouchMidi
                            ? "Aftertouch" : "Unknown MIDI",
                stateArea, Justification::centred, true);

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

    muteButton->setBounds (headerArea.removeFromRight (30 + neova_dash::ui::MARGIN)
                                     .withSizeKeepingCentre (18, 18));
}

void GestureComponent::mouseEnter (const MouseEvent&)
{
    setHighlighted (true);
}
void GestureComponent::mouseExit (const MouseEvent&)
{
    setHighlighted (false);
}
void GestureComponent::mouseDrag (const MouseEvent&)
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

void GestureComponent::createButton()
{
    muteButton = std::make_unique<DashShapeButton> ("Mute Button",
                                                    neova_dash::colour::dashboardBackground,
                                                    neova_dash::gesture::getHighlightColour (hubConfig.getGestureData (id)
                                                                                                      .type),
                                                    neova_dash::gesture::getHighlightColour (hubConfig.getGestureData (id)
                                                                                                      .type, false));
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

        if (hubConfig.getSelectedGesture() != id)
        {
            hubConfig.setSelectedGesture (id);
        }

        commandManager.invokeDirectly (neova_dash::commands::updateDashInterface, true);
    };

    muteButton->setVisible (gesture::isValidGestureType (type));
}

void GestureComponent::drawGesturePath (Graphics& g, juce::Rectangle<int> area)
{
    /*
    Image gestureImage;

    switch (type)
    {
        case (neova_dash::gesture::vibrato):   gestureImage = vibratoImage; break;
        case (neova_dash::gesture::pitchBend): gestureImage = pitchBendImage; break;
        case (neova_dash::gesture::tilt):      gestureImage = tiltImage; break;
        case (neova_dash::gesture::roll):      gestureImage = rollImage; break;
        default: return;

    g.drawImage (gestureImage, area.toFloat(), RectanglePlacement::centred);*/

    Path gesturePath;

    switch (type)
    {
        case (neova_dash::gesture::tilt):
            gesturePath = neova_dash::path::createPath (neova_dash::path::tilt);
            break;

        case (neova_dash::gesture::vibrato):
            gesturePath = neova_dash::path::createPath (neova_dash::path::vibrato);
            break;

        case (neova_dash::gesture::pitchBend):
            gesturePath = neova_dash::path::createPath (neova_dash::path::pitchBend);
            break;

        case (neova_dash::gesture::roll):
            gesturePath = neova_dash::path::createPath (neova_dash::path::roll);
            break;

        default:
            return;
    }

	gesturePath.scaleToFit (area.toFloat().getX(),
                            area.toFloat().getY(),
                            area.toFloat().getWidth(),
                            area.toFloat().getHeight(),
		                    false);

    Colour pathColour (0xff808080);
    ColourGradient gesturePathGradient (pathColour.withAlpha (0.4f),
                                        {area.toFloat().getX(),
                                         area.toFloat().getY() + area.toFloat().getHeight() },
                                        pathColour.withAlpha (0.4f),
                                        {area.toFloat().getX() + area.toFloat().getWidth(),
                                         area.toFloat().getY()},
                                        false);

    gesturePathGradient.addColour (0.35, pathColour.withAlpha (0.0f));
    gesturePathGradient.addColour (0.65, pathColour.withAlpha (0.0f));

    g.setGradientFill (gesturePathGradient);
	g.strokePath (gesturePath, PathStrokeType (2.0f));
}

//==============================================================================
// Empty Gesture Slot 

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

    outline.addRoundedRectangle (getLocalBounds().reduced (1), 10.0f);

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

void EmptyGestureSlotComponent::mouseEnter (const MouseEvent&)
{
    highlighted = true;
    repaint();
}
void EmptyGestureSlotComponent::mouseExit (const MouseEvent&)
{
    highlighted = false;
    repaint();
}
void EmptyGestureSlotComponent::mouseDrag (const MouseEvent&)
{
}