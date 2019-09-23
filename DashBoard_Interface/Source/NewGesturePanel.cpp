/*
  ==============================================================================

    NewGesturePanel.cpp
    Created: 9 Sep 2019 11:47:26am
    Author:  Enhancia Dev

  ==============================================================================
*/

#include "NewGesturePanel.h"

NewGesturePanel::NewGesturePanel (HubConfiguration& config, ApplicationCommandManager& manager)
    : hubConfig (config), commandManager (manager)
{
  	createCloseButton();
  	createGestureSelectorButtons();
  	createAndAddTextEditor();
}

NewGesturePanel::~NewGesturePanel()
{
    closeButton = nullptr;
	  descriptionTextEditor = nullptr;
}

//==============================================================================
// Component Methods
void NewGesturePanel::paint (Graphics& g)
{
    using namespace neova_dash::ui;

    // transparent area
    auto gradTransp = ColourGradient::vertical (Colour (0x00000000),
                                                float (getX()), 
                                                Colour (neova_dash::colour::topPanelTransparentArea),
                                                float(getLocalBounds().getBottom()));
    gradTransp.addColour (0.05f, neova_dash::colour::topPanelTransparentArea);

    g.setGradientFill (gradTransp);
    
    //g.setColour (Colours::black); // TO DELETE
    g.fillRect (getLocalBounds());
    
    // panel area
    ColourGradient gradFill (neova_dash::colour::topPanelBackground
                                         .overlaidWith (Colour (0x10000000)),
                             float (panelArea.getCentreX()),
                             float (panelArea.getBottom()),
                             neova_dash::colour::topPanelBackground,
                             float (panelArea.getCentreX()),
                             float (panelArea.getY()),
                             true);
    gradFill.addColour (0.7, neova_dash::colour::topPanelBackground
                                         .overlaidWith (Colour (0x10000000)));

    g.setGradientFill (gradFill);
    g.fillRoundedRectangle (panelArea.toFloat(), 10.0f);
    
    // panel outline
    auto gradOut = ColourGradient::horizontal (Colour (0x10ffffff),
                                               float(panelArea.getX()), 
                                               Colour (0x10ffffff),
                                               float(panelArea.getRight()));
    gradOut.addColour (0.5, Colour (0x50ffffff));

    g.setGradientFill (gradOut);
    g.drawRoundedRectangle (panelArea.toFloat(), 10.0f, 1.0f);
}

void NewGesturePanel::resized()
{
	  using namespace neova_dash::ui;

    // Panel Area
    panelArea = getLocalBounds().reduced (getWidth()/4, getHeight()/6);

    closeButton->setBounds (juce::Rectangle<int> (25, 25).withRightX (panelArea.getRight() - MARGIN_SMALL)
                                                         .withY (panelArea.getY() + MARGIN_SMALL)
                                                         .reduced (3));

	  auto area = panelArea.reduced (2*MARGIN);

    resizeGestureSelectorButtons (area.removeFromTop (area.getHeight()/2)
    								  .reduced (MARGIN, 2*MARGIN));

    descriptionTextEditor->setBounds (area.reduced (2*MARGIN, MARGIN));
    if (area.getHeight() > 80)
    descriptionTextEditor->setBounds (area.withSizeKeepingCentre (area.reduced (2*MARGIN, MARGIN).getWidth(),
    																  80));
}

//==============================================================================
// Callbacks
void NewGesturePanel::visibilityChanged()
{
}

void NewGesturePanel::mouseEnter (const MouseEvent &event)
{
    using namespace neova_dash::gesture;

  	if (auto* gestureSelector = dynamic_cast<GestureTypeSelector*> (event.eventComponent))
  	{
    		descriptionTextEditor->setText ("", false);

    		// Gesture Type
    		descriptionTextEditor->setFont (neova_dash::font::dashFont.withHeight (15.0f).boldened());
    		descriptionTextEditor->insertTextAtCaret (getTypeString (intToGestureType (gestureSelector->gestureType), true)
    												  	                  + " :\n");
    		// Gesture Description
    		descriptionTextEditor->setFont (neova_dash::font::dashFont.withHeight (14.0f));
    		descriptionTextEditor->insertTextAtCaret (getDescriptionString (intToGestureType (gestureSelector->gestureType)));
    	
    		descriptionTextEditor->setCaretPosition (0);
  	}
}
void NewGesturePanel::mouseUp (const MouseEvent &event)
{
  	if (auto* gestureSelector = dynamic_cast<GestureTypeSelector*> (event.eventComponent))
  	{
    		selectGestureType (gestureSelector);
    		createNewGesture();
    		gestureSelector->setHighlighted (false);
  	}
    else
    {
        if (!panelArea.contains (event.getPosition()))
        {
            hidePanel (true);
        }
    }
}
void NewGesturePanel::buttonClicked (Button* bttn)
{
  	if (bttn == closeButton)
  	{
  		  hidePanel (true);
  	}
}

void NewGesturePanel::createNewGesture()
{
    using namespace neova_dash::gesture;
    hubConfig.setDefaultGestureValues (selectedGestureSlot, intToGestureType (selectedGestureType));

  	updateInterface();
}

void NewGesturePanel::labelTextChanged (Label* lbl)
{
}
void NewGesturePanel::editorShown (Label* lbl, TextEditor& ed)
{
}

//==============================================================================
void NewGesturePanel::showPanelForGestureID (const int gestureID)
{
  	if (gestureID < 0 || gestureID >= neova_dash::gesture::NUM_GEST)
  	{
    		// Plume tries to create a gesture for an id that can't exist
    		jassertfalse;
    		return;
  	}
  	else if (hubConfig.getGestureData (gestureID).type != int (neova_dash::gesture::none))
  	{
    		// Plume tries to create a gesture for an id that already has a gesture!!
    		jassertfalse;
    		return;
  	}

    if (isVisible())
    {
        unselectGestureType();
        descriptionTextEditor->setText ("");
    }

    selectedGestureSlot = gestureID;

    setVisible (true);
}

void NewGesturePanel::hidePanel (const bool resetSelectedSlot)
{
    if (resetSelectedSlot) selectedGestureSlot = -1;

    unselectGestureType();
    setVisible (false);
}

void NewGesturePanel::updateInterface()
{
    commandManager.invokeDirectly (neova_dash::commands::updateDashInterface, true);
}

const int NewGesturePanel::getLastSelectedSlot()
{
	  return selectedGestureSlot;
}

void NewGesturePanel::setVisible (bool shouldBeVisible)
{
	 Component::setVisible (shouldBeVisible);
}

void NewGesturePanel::selectGestureType (const GestureTypeSelector* gestureTypeToSelect)
{
	 selectedGestureType = gestureTypeToSelect->gestureType;
}
void NewGesturePanel::unselectGestureType()
{
	 selectedGestureType = -1;
}

//==============================================================================
void NewGesturePanel::createCloseButton()
{
    addAndMakeVisible (closeButton = new DashShapeButton ("Close Settings Button", Colour(0),
                                                                                   neova_dash::colour::mainText));

    Path p;
    p.startNewSubPath (0, 0);
    p.lineTo (3*neova_dash::ui::MARGIN, 3*neova_dash::ui::MARGIN);
    p.startNewSubPath (0, 3*neova_dash::ui::MARGIN);
    p.lineTo (3*neova_dash::ui::MARGIN, 0);

    closeButton->setShape (p, false, true, false);
    closeButton->addMouseListener (this, false);
    closeButton->addListener (this);
}

void NewGesturePanel::createGestureSelectorButtons()
{
  	jassert (gestureSelectors.isEmpty());

  	for (int i=0; i < neova_dash::gesture::numTypes; i++)
  	{
  		gestureSelectors.add (new GestureTypeSelector (i));
  		addAndMakeVisible (gestureSelectors.getLast());
  		gestureSelectors.getLast()->addMouseListener (this, false);
  	}
}
void NewGesturePanel::resizeGestureSelectorButtons (juce::Rectangle<int> buttonsArea)
{
    using namespace neova_dash::ui;

  	int N = neova_dash::gesture::numTypes;

  	int selectorWidth = (buttonsArea.getWidth() - (N - 1 - 1)*MARGIN) / (N - 1); // TODO WAVE remove -1 when wave is implemented

  	for (int i=0; i < N; i++)
  	{
    		if (i == (int) neova_dash::gesture::wave) continue; // TODO WAVE remove when wave is implemented

    		gestureSelectors[i]->setBounds (buttonsArea.removeFromLeft (selectorWidth));
    		buttonsArea.removeFromLeft (MARGIN);
  	}
}

void NewGesturePanel::createAndAddTextEditor()
{
	  addAndMakeVisible (descriptionTextEditor = new TextEditor ("Gesture Description Text Editor"));
    descriptionTextEditor->setMultiLine (true, true);
    descriptionTextEditor->setReturnKeyStartsNewLine (true);
    descriptionTextEditor->setReadOnly (true);
    descriptionTextEditor->setScrollbarsShown (true);
    descriptionTextEditor->setPopupMenuEnabled (false);
    descriptionTextEditor->applyColourToAllText (neova_dash::colour::mainText, true);
    descriptionTextEditor->setJustification (Justification::centred);
    descriptionTextEditor->setFont (neova_dash::font::dashFont.withHeight (13.0f));
    descriptionTextEditor->setMouseCursor (MouseCursor (MouseCursor::NormalCursor));
    descriptionTextEditor->setInterceptsMouseClicks (false, false);
}

//==============================================================================
// GestureTypeSelector

NewGesturePanel::GestureTypeSelector::GestureTypeSelector (int gestType) : gestureType (gestType)
{
}
NewGesturePanel::GestureTypeSelector::~GestureTypeSelector()
{
}

void NewGesturePanel::GestureTypeSelector::paint (Graphics& g)
{
  	if (highlighted)
  	{
    		// Fill
    		g.setColour (neova_dash::gesture::getHighlightColour (gestureType).withAlpha (0.15f));
    		g.fillRoundedRectangle (getLocalBounds().reduced (2).toFloat(), 10.0f);

  /*
    		// Outline
    		g.setColour (Gesture::getHighlightColour (gestureType));
    		g.drawRoundedRectangle (getLocalBounds().reduced (2).toFloat(), 10.0f, 1.0f);*/
  	}
  	else
  	{
    		g.setColour (neova_dash::colour::gestureBackground);
    		g.fillRoundedRectangle (getLocalBounds().reduced (2).toFloat(), 10.0f);
  	}

  	g.setColour (neova_dash::colour::mainText);
  	drawGesturePath (g, getLocalBounds().reduced (neova_dash::ui::MARGIN));
}
void NewGesturePanel::GestureTypeSelector::resized()
{
}

void NewGesturePanel::GestureTypeSelector::mouseEnter (const MouseEvent &event)
{
  	setHighlighted (true);
  	repaint();
}
void NewGesturePanel::GestureTypeSelector::mouseExit (const MouseEvent &event)
{
  	setHighlighted (false);
  	repaint();
}

void NewGesturePanel::GestureTypeSelector::setHighlighted (bool shouldBeHighlighted)
{
  	if (shouldBeHighlighted != highlighted)
  	{
    		highlighted = shouldBeHighlighted;
    		repaint();
  	}
}

void NewGesturePanel::GestureTypeSelector::drawGesturePath (Graphics& g, juce::Rectangle<int> area)
{
    using namespace neova_dash::gesture;

    g.setColour (Colour (0xfff3f3f3));

	 g.drawText (getTypeString(intToGestureType(gestureType), true), getLocalBounds(),
		           Justification::centred, true);
    /*
    // Icon Fill
    Path iconFill;

    if (gestureType == (int) Gesture::tilt) iconFill = PLUME::path::createPath (PLUME::path::handTilt);
    else if (gestureType == (int) Gesture::roll) iconFill = PLUME::path::createPath (PLUME::path::handRoll);
    else iconFill = PLUME::path::createPath (PLUME::path::handFingerDown);

    auto areaFloat = (gestureType == (int) Gesture::tilt || gestureType == (int) Gesture::roll)
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