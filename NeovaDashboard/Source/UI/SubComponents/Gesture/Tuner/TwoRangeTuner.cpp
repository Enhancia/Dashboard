/*
  ==============================================================================

    TwoRangeTuner.cpp
    Created: 13 Sep 2019 3:57:41pm
    Author:  Enhancia Dev

  ==============================================================================
*/

#include "TwoRangeTuner.h"

//==============================================================================
TwoRangeTuner::TwoRangeTuner (HubConfiguration& config, const int gestureId,
				   			  const float& val, const NormalisableRange<float> gestRange,
                   			  const Range<float> paramMax, const String unit)
    :   Tuner (unit, neova_dash::gesture::getHighlightColour (config.getGestureData (gestureId).type,
    														  config.isGestureActive (gestureId))),
        hubConfig (config), id (gestureId),
        value (val), gestureRange (gestRange),
        parameterMax (paramMax)
{
    setLookAndFeel (&rangeTunerLookAndFeel);

    setAngles (MathConstants<float>::pi*5/3, MathConstants<float>::pi*7/3);
    createSliders();
    createLabels();
    createButtons();
}

TwoRangeTuner::~TwoRangeTuner()
{
    leftLowSlider = nullptr;
    leftHighSlider = nullptr;
    rightLowSlider = nullptr;
    rightHighSlider = nullptr;
	rangeLabelMinLeft = nullptr;
	rangeLabelMaxLeft = nullptr;
	rangeLabelMinRight = nullptr;
	rangeLabelMaxRight = nullptr;

    setLookAndFeel (nullptr);
}
    
//==============================================================================
void TwoRangeTuner::paint (Graphics& g)
{   
    drawTunerSliderBackground (g);
    drawValueCursor (g);

    g.setColour (neova_dash::colour::subText);
    g.setFont (neova_dash::font::dashFontLight.withHeight (13.0f).withExtraKerningFactor (0.08f));
    juce::Rectangle<int> textRect (100, 50);
    textRect.setCentre ({getLocalBounds().getCentreX(),
                         sliderBounds.getCentreY() + int (jmin (sliderBounds.getWidth()/4.0f,
                                                                sliderBounds.getHeight()/4.0f))});

    g.drawText ("RANGE", textRect, Justification::centredTop);
}

void TwoRangeTuner::resized()
{
    // Sets bounds and changes the slider and labels position
    sliderBounds = getLocalBounds().reduced (neova_dash::ui::MARGIN).translated (0, jmax (20, getHeight()/8));
    resizeSliders();
    resizeButtons();

    updateLabelBounds (rangeLabelMinLeft);
    updateLabelBounds (rangeLabelMaxLeft);
    updateLabelBounds (rangeLabelMinRight);
    updateLabelBounds (rangeLabelMaxRight);

    repaint();
}


void TwoRangeTuner::resizeSliders()
{
    sliderRadius = jmin (sliderBounds.getWidth()/2.0f, sliderBounds.getHeight()/2.0f);
    sliderCentre = {getLocalBounds().getCentreX(),
                    sliderBounds.getCentreY() + (int) sliderRadius/2};

    juce::Rectangle<int> adjustedBounds = sliderBounds.withWidth (sliderRadius*2)
                                                      .withHeight (sliderRadius*2)
                                                      .expanded (10)
                                                      .withCentre (sliderCentre);

    leftLowSlider->setBounds (adjustedBounds);
    leftHighSlider->setBounds (adjustedBounds);
    rightLowSlider->setBounds (adjustedBounds);
    rightHighSlider->setBounds (adjustedBounds);
}

void TwoRangeTuner::resizeButtons()
{
    using namespace neova_dash::ui;

    auto buttonsAreaLeft = getLocalBounds().withRight (getLocalBounds().getX() + 80)
                                           .withHeight (70)
                                           .reduced (MARGIN);

    auto buttonsAreaRight = getLocalBounds().withLeft (getLocalBounds().getRight() - 80)
                                            .withHeight (70)
                                            .reduced (MARGIN);

    maxLeftAngleButton->setBounds (buttonsAreaLeft.removeFromTop (buttonsAreaLeft.getHeight()/2).withTrimmedBottom (MARGIN/2));
    minLeftAngleButton->setBounds (buttonsAreaLeft.withTrimmedTop (MARGIN/2));
    maxRightAngleButton->setBounds (buttonsAreaRight.removeFromTop (buttonsAreaRight.getHeight()/2).withTrimmedBottom (MARGIN/2));
    minRightAngleButton->setBounds (buttonsAreaRight.withTrimmedTop (MARGIN/2));
}

void TwoRangeTuner::updateComponents()
{
    if (getRangeLeftLow() < getRangeLeftHigh()
            && leftLowSlider->getThumbBeingDragged() == -1
            && leftHighSlider->getThumbBeingDragged() == -1)
    {
        // Sets slider value
        leftLowSlider->setValue (double (getRangeLeftLow()), dontSendNotification);
        leftHighSlider->setValue (double (getRangeLeftHigh()), dontSendNotification);
        
        // Sets label text
        if (!(rangeLabelMinLeft->isBeingEdited()))
		{
		    rangeLabelMinLeft->setText (String (int (getRangeLeftLow())) + valueUnit, dontSendNotification);
		}
		if (!(rangeLabelMaxLeft->isBeingEdited()))
		{
            rangeLabelMaxLeft->setText (String (int (getRangeLeftHigh())) + valueUnit, dontSendNotification);
		}
    }
        
    if (getRangeRightLow() < getRangeRightHigh()
            && rightLowSlider->getThumbBeingDragged() == -1
            && rightHighSlider->getThumbBeingDragged() == -1)
    {
        // Sets slider value
        rightHighSlider->setValue (double (getRangeRightHigh()), dontSendNotification);
        rightLowSlider->setValue (double (getRangeRightLow()), dontSendNotification);
        
        // Sets label text
        if (!(rangeLabelMinRight->isBeingEdited()))
		{
            rangeLabelMinRight->setText (String (int (getRangeRightLow())) + valueUnit, dontSendNotification);
		}
        if (!(rangeLabelMaxRight->isBeingEdited()))
		{
            rangeLabelMaxRight->setText (String (int (getRangeRightHigh())) + valueUnit, dontSendNotification);
		}
    }
}

void TwoRangeTuner::updateDisplay()
{
    if (getValueAngle() != previousCursorAngle)
    {
        repaint (sliderBounds);
    }
}

void TwoRangeTuner::setColour (const Colour newColour)
{
    Tuner::setColour (newColour);

    auto setLabelColours = [this] (Label& label)
    {
        label.setColour (Label::textColourId, tunerColour);
        label.setColour (Label::textWhenEditingColourId, tunerColour);
        label.setColour (TextEditor::textColourId, tunerColour);
        label.setColour (TextEditor::highlightColourId, tunerColour.withAlpha (0.2f));
        label.setColour (TextEditor::highlightedTextColourId, tunerColour);
        label.setColour (CaretComponent::caretColourId, tunerColour.withAlpha (0.2f));
    };

    setLabelColours (*rangeLabelMinLeft);
    setLabelColours (*rangeLabelMaxLeft);
    setLabelColours (*rangeLabelMinRight);
    setLabelColours (*rangeLabelMaxRight);

    repaint();
}

void TwoRangeTuner::updateColour()
{
	setColour (neova_dash::gesture::getHighlightColour (hubConfig.getGestureData (id).type,
    												    hubConfig.isGestureActive (id)));
}
    
//==============================================================================
void TwoRangeTuner::labelTextChanged (Label* lbl)
{
    // checks that the string is numbers only (and unit)
    if (lbl->getText().containsOnly ("-0123456789"+valueUnit) == false)
    {
        if      (lbl == rangeLabelMinLeft)   lbl->setText (String (int (getRangeLeftLow())) + valueUnit, dontSendNotification);
        else if (lbl == rangeLabelMaxLeft)   lbl->setText (String (int (getRangeLeftHigh())) + valueUnit, dontSendNotification);
        else if (lbl == rangeLabelMinRight)  lbl->setText (String (int (getRangeRightLow())) + valueUnit, dontSendNotification);
        else if (lbl == rangeLabelMaxRight)  lbl->setText (String (int (getRangeRightHigh())) + valueUnit, dontSendNotification);

        return;
    }
        
    float val;
        
    // Gets the float value of the text 
    if (valueUnit != "" &&
        lbl->getText().endsWith(valueUnit)) val = lbl->getText().upToFirstOccurrenceOf(valueUnit, false, false).getFloatValue();
    else                                    val = lbl->getText().getFloatValue();
        
    if (val < parameterMax.getStart())    val = parameterMax.getStart();
    else if (val > parameterMax.getEnd()) val = parameterMax.getEnd();
        
    // Sets slider and labels accordingly
    if (lbl == rangeLabelMinLeft)   // Min left
    {
        if (val > getRangeLeftHigh()) val = getRangeLeftHigh();
            
        leftLowSlider->setValue (val, sendNotification);
    	setRangeLeftLow (float (leftLowSlider->getValue()), false);
    	setRangeLeftHigh (float (leftHighSlider->getValue()));
        lbl->setText (String (int (getRangeLeftLow())) + valueUnit, dontSendNotification);
    }
    else if (lbl == rangeLabelMaxLeft)  // Max left
    {
        if (val < getRangeLeftLow()) val = getRangeLeftLow();
        else if (val > getRangeRightLow()) val = getRangeRightLow();

        leftHighSlider->setValue (val, sendNotification);
    	setRangeLeftLow (float (leftLowSlider->getValue()), false);
    	setRangeLeftHigh (float (leftHighSlider->getValue()));
        lbl->setText (String (int (getRangeLeftHigh())) + valueUnit, dontSendNotification);
    }
    else if (lbl == rangeLabelMinRight)   // Min right
    {
        if ( val > getRangeRightHigh()) val = getRangeRightHigh();
        else if (val < getRangeLeftHigh()) val = getRangeLeftHigh();
            
        rightLowSlider->setValue (val, sendNotification);
    	setRangeRightLow (float (rightLowSlider->getValue()), false);
    	setRangeRightHigh (float (rightHighSlider->getValue()));
        lbl->setText (String (int (getRangeRightLow())) + valueUnit, dontSendNotification);
    }
    else if (lbl == rangeLabelMaxRight)  // Max right
    {
        if ( val < getRangeRightLow()) val = getRangeRightLow();
        
        rightHighSlider->setValue (val, sendNotification);
    	setRangeRightLow (float (rightLowSlider->getValue()), false);
    	setRangeRightHigh (float (rightHighSlider->getValue()));
        lbl->setText (String (int (getRangeRightHigh())) + valueUnit, dontSendNotification);
    }
}

void TwoRangeTuner::editorShown (Label*, TextEditor& ted)
{
    ted.setJustification (Justification::centred);
}

void TwoRangeTuner::editorHidden (Label* lbl, TextEditor&)
{
    lbl->setVisible (false);
}

void TwoRangeTuner::sliderValueChanged (Slider* sldr)
{
	// min left value changed
	if (sldr == leftLowSlider)
	{
        // Illegal attempt to get left range over right range
        if (sldr->getValue() > rightLowSlider->getValue())
        {
            sldr->setValue (double (rightLowSlider->getValue()), dontSendNotification);
            updateLabelBounds (rangeLabelMinLeft);
            rangeLabelMinLeft->setText (String (int (getRangeLeftHigh())) + valueUnit, dontSendNotification);
            //return;
        }
        else // Normal drag
        {
            updateLabelBounds (rangeLabelMinLeft);
            rangeLabelMinLeft->setText (String (int (sldr->getValue())) + valueUnit, dontSendNotification);
	    }

	    // In case the other thumb is dragged along..
		if (leftHighSlider->getThumbBeingDragged() == -1 && leftLowSlider->getValue() > leftHighSlider->getValue())
		{
            leftHighSlider->setValue (sldr->getValue(), dontSendNotification);
            updateLabelBounds (rangeLabelMaxLeft);
			rangeLabelMaxLeft->setText(String(int(sldr->getValue())) + valueUnit, dontSendNotification);
		}
				
	}

	// max left value changed
	else if (sldr == leftHighSlider)
	{
        // Illegal attempt to get left range over right range
        if (leftHighSlider->getValue() > rightLowSlider->getValue())
        {
            sldr->setValue (double (rightLowSlider->getValue()), dontSendNotification);
            updateLabelBounds (rangeLabelMaxLeft);
            rangeLabelMaxLeft->setText (String (int (getRangeRightLow())) + valueUnit, dontSendNotification);
            //return;
        }

        // normal case
        updateLabelBounds (rangeLabelMaxLeft);
	    rangeLabelMaxLeft->setText (String (int (sldr->getValue())) + valueUnit, dontSendNotification);
	        
	    // in case the other thumb is dragged along..
		if (leftLowSlider->getThumbBeingDragged() == -1 && leftLowSlider->getValue() > leftHighSlider->getValue())
		{
            leftLowSlider->setValue (sldr->getValue(), dontSendNotification);
            updateLabelBounds (rangeLabelMinLeft);
			rangeLabelMinLeft->setText(String(int(sldr->getValue())) + valueUnit, dontSendNotification);
		}
	}

	// min right value changed
	if (sldr == rightLowSlider)
	{
        // Illegal attempt to get right range over left range
        if (leftHighSlider->getValue() > rightLowSlider->getValue())
        {
            sldr->setValue (double (leftHighSlider->getValue()), dontSendNotification);
            updateLabelBounds (rangeLabelMinRight);
            rangeLabelMinRight->setText (String (int (getRangeLeftHigh())) + valueUnit, dontSendNotification);
            //return;
        }

        // Normal case
        updateLabelBounds (rangeLabelMinRight);
	    rangeLabelMinRight->setText (String (int (sldr->getValue())) + valueUnit, dontSendNotification);
	        
	    // in case the other thumb is dragged along..
		if (rightHighSlider->getThumbBeingDragged() == -1 && rightLowSlider->getValue() > rightHighSlider->getValue())
		{
            rightHighSlider->setValue (sldr->getValue(), dontSendNotification);
            updateLabelBounds (rangeLabelMaxRight);
			rangeLabelMaxRight->setText(String(int(sldr->getValue())) + valueUnit, dontSendNotification);
		}
	}

	// max right value changed
	else if (sldr == rightHighSlider)
	{
        // Illegal attempt to get right range over left range
        if (leftHighSlider->getValue() > sldr->getValue())
        {
            sldr->setValue (double (leftHighSlider->getValue()), dontSendNotification);
            updateLabelBounds (rangeLabelMaxRight);
            rangeLabelMaxRight->setText (String (int (getRangeLeftHigh())) + valueUnit, dontSendNotification);
            //return;
        }
        else // Normal drag
        {
            updateLabelBounds (rangeLabelMaxRight);
    	    rangeLabelMaxRight->setText (String (int (sldr->getValue())) + valueUnit, dontSendNotification);
	    }  
	    // in case the other thumb is dragged along..
		if (rightLowSlider->getThumbBeingDragged() == -1 && rightLowSlider->getValue() > rightHighSlider->getValue())
		{
            rightLowSlider->setValue (sldr->getValue(), dontSendNotification);
            updateLabelBounds (rangeLabelMinRight);
			rangeLabelMinRight->setText(String(int(sldr->getValue())) + valueUnit, dontSendNotification);
		}
	}
}

void TwoRangeTuner::buttonClicked (Button* bttn)
{
    if (bttn == maxLeftAngleButton)
    {
        if (value > getRangeRightLow())
        {
            leftHighSlider->setValue (getRangeRightLow(), sendNotification);
            leftLowSlider->setValue (getRangeRightLow(), sendNotification);
        }
        else
        {
            if (value > getRangeLeftHigh())
            {
                leftHighSlider->setValue (value);
            }

            leftLowSlider->setValue (value, sendNotification);
        }
        setRangeLeftHigh (float (leftHighSlider->getValue()));
        setRangeLeftLow (float (leftLowSlider->getValue()));
    }
    else if (bttn == minLeftAngleButton)
    {
        if (value < getRangeLeftLow())
        {
            leftLowSlider->setValue (value, sendNotification);
            setRangeLeftLow (float (leftLowSlider->getValue()));
        }

        leftHighSlider->setValue (/*gestureRange.convertFrom0to1 (value)*/ value, sendNotification);
        setRangeLeftHigh (float (leftHighSlider->getValue()));
    }
     if (bttn == minRightAngleButton)
    {
        if (value > getRangeRightHigh())
        {
            rightHighSlider->setValue (value, sendNotification);
            setRangeRightHigh (float (rightHighSlider->getValue()));
        }
        rightLowSlider->setValue (/*gestureRange.convertFrom0to1 (value)*/ value, sendNotification);
        setRangeRightLow (float (rightLowSlider->getValue()));
    }
    else if (bttn == maxRightAngleButton)
    {
        if (value < getRangeLeftHigh())
        {
            rightLowSlider->setValue (getRangeLeftHigh(), sendNotification);
            rightHighSlider->setValue (getRangeLeftHigh(), sendNotification);
        }
        else
        {
            if (value < getRangeRightLow())
            {
                rightLowSlider->setValue (value);
            }

            rightHighSlider->setValue (/*gestureRange.convertFrom0to1 (value)*/ value, sendNotification);
        }
        setRangeRightLow (float (rightLowSlider->getValue()));
        setRangeRightHigh (float (rightHighSlider->getValue()));
    }
}

void TwoRangeTuner::mouseDown (const MouseEvent& e)
{
    if (e.mods.isLeftButtonDown())
    {
        if (e.getNumberOfClicks() == 1)
        {
            handleSingleClick (e);
        }
        else
        {
            handleDoubleClick (e);
        }
    }
}

void TwoRangeTuner::handleSingleClick (const MouseEvent& e)
{
    objectBeingDragged = getObjectToDrag (e);

    if (objectBeingDragged == leftLowThumb)
    {
        leftLowSlider->mouseDown (e.getEventRelativeTo (leftLowSlider));
        rangeLabelMinLeft->setVisible (true);
    }
    else if (objectBeingDragged == leftHighThumb)
    {
        leftHighSlider->mouseDown (e.getEventRelativeTo (leftHighSlider));
        rangeLabelMaxLeft->setVisible (true);
    }
    else if (objectBeingDragged == rightLowThumb)
    {
        rightLowSlider->mouseDown (e.getEventRelativeTo (rightLowSlider));
        rangeLabelMinRight->setVisible (true);
    }
    else if (objectBeingDragged == rightHighThumb)
    {
        rightHighSlider->mouseDown (e.getEventRelativeTo (rightHighSlider));
        rangeLabelMaxRight->setVisible (true);
    }
    else if (objectBeingDragged == middleAreaLeft)
    {
        leftLowSlider->setSliderStyle (Slider::RotaryHorizontalDrag);
        leftHighSlider->setSliderStyle (Slider::RotaryHorizontalDrag);

        leftLowSlider->mouseDown (e.getEventRelativeTo (leftLowSlider));
        leftHighSlider->mouseDown (e.getEventRelativeTo (leftHighSlider));
        rangeLabelMinLeft->setVisible (true);
        rangeLabelMaxLeft->setVisible (true);
    }
    else if (objectBeingDragged == middleAreaRight)
    {
        rightLowSlider->setSliderStyle (Slider::RotaryHorizontalDrag);
        rightHighSlider->setSliderStyle (Slider::RotaryHorizontalDrag);

        rightLowSlider->mouseDown (e.getEventRelativeTo (rightLowSlider));
        rightHighSlider->mouseDown (e.getEventRelativeTo (rightHighSlider));
        rangeLabelMinRight->setVisible (true);
        rangeLabelMaxRight->setVisible (true);
    }

    updateMouseCursor();
    repaint (sliderBounds);
}

void TwoRangeTuner::handleDoubleClick (const MouseEvent& e)
{
    if (getObjectToDrag (e) == leftLowThumb)
    {
		rangeLabelMinLeft->setVisible (true);
        rangeLabelMinLeft->showEditor();
    }
    else if (getObjectToDrag (e) == leftHighThumb)
    {
		rangeLabelMaxLeft->setVisible (true);
        rangeLabelMaxLeft->showEditor();
    }
    else if (getObjectToDrag (e) == rightLowThumb)
    {
        rangeLabelMinRight->setVisible (true);
		rangeLabelMinRight->showEditor();
    }
    else if (getObjectToDrag (e) == rightHighThumb)
    {
		rangeLabelMaxRight->setVisible (true);
		rangeLabelMaxRight->showEditor();
    }
}

void TwoRangeTuner::mouseDrag (const MouseEvent& e)
{
    if (!e.mods.isLeftButtonDown() || e.getNumberOfClicks() > 1) return;

    if (objectBeingDragged == leftLowThumb)        leftLowSlider->mouseDrag (e.getEventRelativeTo (leftLowSlider));
    else if (objectBeingDragged == leftHighThumb)  leftHighSlider->mouseDrag (e.getEventRelativeTo (leftHighSlider));
    else if (objectBeingDragged == rightLowThumb)  rightLowSlider->mouseDrag (e.getEventRelativeTo (rightLowSlider));
    else if (objectBeingDragged == rightHighThumb) rightHighSlider->mouseDrag (e.getEventRelativeTo (rightHighSlider));
    
    else if (objectBeingDragged == middleAreaLeft)
    {
        leftLowSlider->mouseDrag (e.getEventRelativeTo (leftLowSlider));
        leftHighSlider->mouseDrag (e.getEventRelativeTo (leftHighSlider));
    }
    else if (objectBeingDragged == middleAreaRight)
    {
        rightLowSlider->mouseDrag (e.getEventRelativeTo (rightLowSlider));
        rightHighSlider->mouseDrag (e.getEventRelativeTo (rightHighSlider));
    }

    repaint (sliderBounds);
}
void TwoRangeTuner::mouseUp (const MouseEvent& e)
{
    if (objectBeingDragged == middleAreaLeft)
    {
        leftLowSlider->mouseUp (e.getEventRelativeTo (leftLowSlider));
        leftHighSlider->mouseUp (e.getEventRelativeTo (leftHighSlider));
        leftLowSlider->setSliderStyle (Slider::Rotary);
        leftHighSlider->setSliderStyle (Slider::Rotary);
    }
    else if (objectBeingDragged == middleAreaRight)
    {
        rightLowSlider->mouseUp (e.getEventRelativeTo (rightLowSlider));
        rightHighSlider->mouseUp (e.getEventRelativeTo (rightHighSlider));
        rightLowSlider->setSliderStyle (Slider::Rotary);
        rightHighSlider->setSliderStyle (Slider::Rotary);
    }
    else if (objectBeingDragged == leftLowThumb)
    {
        leftLowSlider->mouseUp (e.getEventRelativeTo (leftLowSlider));
    }
    else if (objectBeingDragged == leftHighThumb)
    {
        leftHighSlider->mouseUp (e.getEventRelativeTo (leftHighSlider));
    }
    else if (objectBeingDragged == rightLowThumb)
    {
        rightLowSlider->mouseUp (e.getEventRelativeTo (rightLowSlider));
    }
    else if (objectBeingDragged == rightHighThumb)
    {
        rightHighSlider->mouseUp (e.getEventRelativeTo (rightHighSlider));
    }

    if (objectBeingDragged != none)
    {
        setRangeLeftLow (float (leftLowSlider->getValue()), false);
        setRangeLeftHigh (float (leftHighSlider->getValue()), false);
        setRangeRightLow (float (rightLowSlider->getValue()), false);
        setRangeRightHigh (float (rightHighSlider->getValue()));

        rangeLabelMinLeft->setVisible (false);
        rangeLabelMaxLeft->setVisible (false);
        rangeLabelMinRight->setVisible (false);
        rangeLabelMaxRight->setVisible (false);
        objectBeingDragged = none;
        updateMouseCursor();
        repaint (sliderBounds);
    }
}
MouseCursor TwoRangeTuner::getMouseCursor()
{
    return MouseCursor::NormalCursor;
}

void TwoRangeTuner::setAngles (float start, float end)
{
    startAngle = start;
    endAngle = end;
}

void TwoRangeTuner::createSliders()
{
    Tuner::addAndMakeVisible (leftLowSlider = new Slider ("Range Slider Left Low"));
    Tuner::addAndMakeVisible (leftHighSlider = new Slider ("Range Slider Left High"));
    Tuner::addAndMakeVisible (rightLowSlider = new Slider ("Range Slider Right Low"));
    Tuner::addAndMakeVisible (rightHighSlider = new Slider ("Range Slider Right High"));
        
    auto setSliderSettings = [this] (Slider& slider, float valueToSet)
    {
        slider.setSliderStyle (Slider::Rotary);
        slider.setRotaryParameters (startAngle, endAngle, true);
        slider.setSliderSnapsToMousePosition (false);
        slider.setTextBoxStyle (Slider::NoTextBox, false, 0, 0);
        slider.setColour (Slider::rotarySliderFillColourId, Colour (0x00000000));
        slider.setColour (Slider::rotarySliderOutlineColourId, Colour (0x00000000));
        slider.setRange (double (parameterMax.getStart()), double (parameterMax.getEnd()), 1.0);
        slider.setValue (double (valueToSet));
        slider.addListener (this);
        slider.setInterceptsMouseClicks (false, false);
        //slider.setLookAndFeel (&rangeTunerLookAndFeel);
	};

    setSliderSettings (*leftLowSlider, getRangeLeftLow());
    setSliderSettings (*leftHighSlider, getRangeLeftHigh());
    setSliderSettings (*rightLowSlider, getRangeRightLow());
    setSliderSettings (*rightHighSlider, getRangeRightHigh());
}
    
void TwoRangeTuner::createLabels()
{
    addAndMakeVisible (rangeLabelMinLeft  = new Label ("Min Left Label",
                                                       TRANS (String (int(getRangeLeftLow())) + valueUnit)));
    addAndMakeVisible (rangeLabelMaxLeft  = new Label ("Max Left Label",
                                                       TRANS (String(int(getRangeLeftHigh())) + valueUnit)));
    addAndMakeVisible (rangeLabelMinRight = new Label ("Min Right Label",
                                                       TRANS (String(int(getRangeRightLow())) + valueUnit)));
    addAndMakeVisible (rangeLabelMaxRight = new Label ("Max Right Label",
                                                       TRANS (String(int(getRangeRightHigh())) + valueUnit)));
    
    auto setLabelSettings = [this] (Label& label)
    {
        label.setEditable (true, false, false);
        label.setFont (neova_dash::font::dashFont.withHeight (13.0f));
        label.setJustificationType (Justification::centred);
        label.setColour (Label::textColourId, tunerColour);
        label.setColour (Label::textWhenEditingColourId, tunerColour);
        label.setColour (TextEditor::textColourId, tunerColour);
        label.setColour (TextEditor::highlightColourId, tunerColour.withAlpha (0.2f));
        label.setColour (TextEditor::highlightedTextColourId, tunerColour);
        label.setColour (CaretComponent::caretColourId, tunerColour.withAlpha (0.2f));
        label.setSize (30, 20);
        label.addListener (this);
        label.setVisible (false);
        //label.setLookAndFeel (&slidersLookAndFeel);
    };

    setLabelSettings (*rangeLabelMinLeft);
    setLabelSettings (*rangeLabelMaxLeft);
    setLabelSettings (*rangeLabelMinRight);
    setLabelSettings (*rangeLabelMaxRight);
}

void TwoRangeTuner::createButtons()
{
    addAndMakeVisible (minLeftAngleButton = new TextButton ("MinLeft Angle Button"));
    addAndMakeVisible (maxLeftAngleButton = new TextButton ("MaxLeft Angle Button"));
    addAndMakeVisible (minRightAngleButton = new TextButton ("MinRight Angle Button"));
    addAndMakeVisible (maxRightAngleButton = new TextButton ("MaxRight Angle Button"));

    auto setButtonSettings = [this] (TextButton& button)
    {
        button.setColour (TextButton::buttonColourId , neova_dash::colour::tunerSliderBackground);
        button.setColour (TextButton::buttonOnColourId , tunerColour);
        button.setColour (TextButton::textColourOffId , neova_dash::colour::mainText);
        button.setColour (TextButton::textColourOnId , neova_dash::colour::mainText);
        button.setButtonText (&button == minLeftAngleButton || &button == minRightAngleButton ? "MIN ANGLE"
                                                                                              : "MAX ANGLE");
        button.addListener (this);
    };

    setButtonSettings (*minLeftAngleButton);
    setButtonSettings (*maxLeftAngleButton);
    setButtonSettings (*minRightAngleButton);
    setButtonSettings (*maxRightAngleButton);
}
    
void TwoRangeTuner::setRangeLeftLow (float val, bool uploadToHub)
{
	hubConfig.setFloatValue (id, HubConfiguration::gestureParam0, val, uploadToHub);
}
    
void TwoRangeTuner::setRangeLeftHigh (float val, bool uploadToHub)
{
	hubConfig.setFloatValue (id, HubConfiguration::gestureParam1, val, uploadToHub);
}
    
void TwoRangeTuner::setRangeRightLow (float val, bool uploadToHub)
{
	hubConfig.setFloatValue (id, HubConfiguration::gestureParam2, val, uploadToHub);
}
    
void TwoRangeTuner::setRangeRightHigh (float val, bool uploadToHub)
{
	hubConfig.setFloatValue (id, HubConfiguration::gestureParam3, val, uploadToHub);
}
    
float TwoRangeTuner::getRangeLeftLow()
{
    return hubConfig.getGestureData (id).gestureParam0;
}
    
float TwoRangeTuner::getRangeLeftHigh()
{
    return hubConfig.getGestureData (id).gestureParam1;
}
    
float TwoRangeTuner::getRangeRightLow()
{
    return hubConfig.getGestureData (id).gestureParam2;
}
    
float TwoRangeTuner::getRangeRightHigh()
{
    return hubConfig.getGestureData (id).gestureParam3;
}

double TwoRangeTuner::getAngleFromMouseEventRadians (const MouseEvent& e)
{
    double angle =  std::atan2 (e.getMouseDownX() - sliderCentre.x,
                                -(e.getMouseDownY() - sliderCentre.y));

    if (angle < 0) angle += MathConstants<double>::twoPi;

    return angle;
}

double TwoRangeTuner::getThumbAngleRadians (const DraggableObject thumb)
{
    if (thumb == none || thumb == middleAreaLeft || thumb == middleAreaRight) return -0.01;
    
    Slider* slider;

    switch (thumb)
    {
        case leftLowThumb: slider = leftLowSlider; break;
        case leftHighThumb: slider = leftHighSlider; break;
        case rightLowThumb: slider = rightLowSlider; break;
        case rightHighThumb: slider = rightHighSlider; break;
        default: return -0.01;
    }

    return startAngle + ((slider->getValue() - slider->getMinimum())/slider->getRange().getLength())
                             *(endAngle - startAngle);
}

TwoRangeTuner::DraggableObject TwoRangeTuner::getObjectToDrag (const MouseEvent& e)
{
    double mouseAngle = getAngleFromMouseEventRadians (e);
    float twoPi =  MathConstants<float>::twoPi;

    //================ Figures out the scenario surrounding the click ==============
    bool inverted = startAngle > endAngle;

    // Computes "% 2*pi" if both angles are somehow greater that 2*pi
    float startAngleModulo = startAngle, endAngleModulo = endAngle;
    while (startAngleModulo > twoPi
           && endAngleModulo > twoPi)
    {
        startAngleModulo -= twoPi;
        endAngleModulo -= twoPi;
    }

    // If only one extreme angle is higher than 2*pi, the process needs a different logic.
    // Boolean "differentRef" notifies that this is the case.
    bool differentRef = jmax(startAngleModulo, endAngleModulo) > twoPi
                        && jmin(startAngleModulo, endAngleModulo) < twoPi;
    
    // If the higher angle minus 2pi is still higher that the smaller, then we fall back to the
    // previous logic with "inverted" angles.
    if (differentRef && jmax(startAngleModulo, endAngleModulo) - twoPi
                        > jmin(startAngleModulo, endAngleModulo))
    {
        differentRef = false;
        inverted ? startAngleModulo -= twoPi
                 : endAngleModulo -= twoPi;
    }

    //================ Primary check: should we consider the left or right range? ==============
    bool leftNotRight = true;

    float leftHighAngle = jmax (float (getThumbAngleRadians (leftLowThumb)), float (getThumbAngleRadians (leftHighThumb)));
    float rightLowAngle = jmin (float (getThumbAngleRadians (rightLowThumb)), float (getThumbAngleRadians (rightHighThumb)));
    
    auto adjustedMouseAngle = !differentRef ? mouseAngle
                                            : mouseAngle < startAngle ? mouseAngle + twoPi
                                                                      : mouseAngle;

    // case1: both middle thumbs in the same side (< 2pi)
    if (!differentRef || (leftHighAngle < twoPi && rightLowAngle < twoPi))
    {
        if (adjustedMouseAngle < (leftHighAngle + rightLowAngle)/2.0f) leftNotRight = true;
        else                                                           leftNotRight = false;
    }
    // case2: both middle thumbs in the same side (> 2pi)
    else if (leftHighAngle > twoPi && rightLowAngle > twoPi)
    {
        if (adjustedMouseAngle < (leftHighAngle + rightLowAngle)/2.0f) leftNotRight = true;
        else                                                           leftNotRight = false;
    }
    // case3: One midde thumb < 2pi, the other >2pi
    else if (leftHighAngle < twoPi && rightLowAngle > twoPi)
    {
        if (adjustedMouseAngle < (leftHighAngle + rightLowAngle)/2.0f)
        {
            leftNotRight = true;
        }
        else leftNotRight = false;
    }

    DraggableObject lowThumb = leftNotRight ? leftLowThumb : rightLowThumb;
    DraggableObject highThumb = leftNotRight ? leftHighThumb : rightHighThumb;
    DraggableObject middleArea = leftNotRight ? middleAreaLeft : middleAreaRight;
    Slider* lowSlider = leftNotRight ? leftLowSlider : rightLowSlider;
    Slider* highSlider = leftNotRight ? leftHighSlider : rightHighSlider;

    //================ Finds the object to drag ==============
    if (e.mods.isShiftDown()) return middleArea;

    DraggableObject thumb1 = inverted ? lowThumb : highThumb;
    DraggableObject thumb2 = inverted ? highThumb : lowThumb;

    // Tolerance: area around which a thumb can be clicked, its value is the 5th of the angle between the two thumbs.
    double tolerance = ((highSlider->getValue() - lowSlider->getValue())*(std::abs (endAngle - startAngle)))
                                /(lowSlider->getRange().getLength()*5);

    // Click within the slider range
    if ((!differentRef && (mouseAngle > jmin (startAngleModulo, endAngleModulo)
                       && mouseAngle < jmax (startAngleModulo, endAngleModulo)))
        || (differentRef && (mouseAngle > jmin(startAngleModulo, endAngleModulo)
                                || mouseAngle + twoPi
                                   < jmax(startAngleModulo, endAngleModulo))))
    {
        if (!differentRef)
        {
            if (mouseAngle > getThumbAngleRadians(thumb1) - tolerance)      return thumb1;
            else if (mouseAngle < getThumbAngleRadians(thumb2) + tolerance) return thumb2;
            else return middleArea;
        }
        else
        {
            if (mouseAngle < jmax(startAngleModulo, endAngleModulo) - twoPi)
                mouseAngle += twoPi;

            if (mouseAngle > getThumbAngleRadians(thumb1) - tolerance)      return thumb1;
            else if (mouseAngle < getThumbAngleRadians(thumb2) + tolerance) return thumb2;
            else return middleArea;
        }
    }
    
    return none;
}

void TwoRangeTuner::drawTunerSliderBackground (Graphics& g)
{
    auto outline = neova_dash::colour::tunerSliderBackground;
    auto fillLeft    = objectBeingDragged == middleAreaLeft ? tunerColour.interpolatedWith (Colour (0xffffffff), 0.8f)
                                                            : tunerColour;

    auto fillRight    = objectBeingDragged == middleAreaRight ? tunerColour.interpolatedWith (Colour (0xffffffff), 0.8f)
                                                              : tunerColour;


    auto leftLowAngle = startAngle
                        + (leftLowSlider->getValue() - leftLowSlider->getMinimum())
                            / leftLowSlider->getRange().getLength() * (endAngle - startAngle);
    auto leftHighAngle = startAngle
                        + (leftHighSlider->getValue() - leftLowSlider->getMinimum())
                            / leftLowSlider->getRange().getLength() * (endAngle - startAngle);

    auto rightLowAngle = startAngle
                        + (rightLowSlider->getValue() - rightLowSlider->getMinimum())
                            / rightLowSlider->getRange().getLength() * (endAngle - startAngle);
    auto rightHighAngle = startAngle
                        + (rightHighSlider->getValue() - rightLowSlider->getMinimum())
                            / rightLowSlider->getRange().getLength() * (endAngle - startAngle);

    auto lineW = jmin (8.0f, sliderRadius * 0.5f);
    auto arcRadius = sliderRadius - lineW * 0.5f;

    Path backgroundArc;
    backgroundArc.addCentredArc (sliderCentre.x,
                                 sliderCentre.y,
                                 arcRadius,
                                 arcRadius,
                                 0.0f,
                                 startAngle,
                                 endAngle,
                                 true);

    g.setColour (outline);
    g.strokePath (backgroundArc, PathStrokeType (12.0f, PathStrokeType::curved, PathStrokeType::rounded));

    if (isEnabled())
    {
        Path valueArcLeft, valueArcRight;
        valueArcLeft.addCentredArc (sliderCentre.x,
                                    sliderCentre.y,
                                    arcRadius,
                                    arcRadius,
                                    0.0f,
                                    float (leftLowAngle),
                                    float (leftHighAngle),
                                    true);

        g.setColour (fillLeft);
        g.strokePath (valueArcLeft, PathStrokeType (6.0f, PathStrokeType::curved, PathStrokeType::rounded));

        valueArcRight.addCentredArc (float (sliderCentre.x),
                                     float (sliderCentre.y),
                                     arcRadius,
                                     arcRadius,
                                     0.0f,
                                     float (rightLowAngle),
                                     float (rightHighAngle),
                                     true);

        g.setColour (fillRight);
        g.strokePath (valueArcRight, PathStrokeType (6.0f, PathStrokeType::curved, PathStrokeType::rounded));

    }

    if (objectBeingDragged != none && objectBeingDragged != middleAreaLeft
                                   && objectBeingDragged != middleAreaRight)
    {
        auto angle = getThumbAngleRadians (objectBeingDragged);

        juce::Point<float> thumbPoint (float (sliderCentre.x) + arcRadius * std::cos (angle - MathConstants<float>::halfPi),
                                 float (sliderCentre.y) + arcRadius * std::sin (angle - MathConstants<float>::halfPi));

        g.setColour (tunerColour.withAlpha (0.6f));
        g.fillEllipse (juce::Rectangle<float> (25.0f, 25.0f).withCentre (thumbPoint));
    }
}
void TwoRangeTuner::updateLabelBounds (Label* labelToUpdate)
{
    if (labelToUpdate == nullptr) return;

    float radius;
    double angle;

    if (labelToUpdate == rangeLabelMinLeft)
    {
        radius = sliderRadius + 15;
        angle = getThumbAngleRadians (leftLowThumb);
    }
    else if (labelToUpdate == rangeLabelMaxLeft)
    {
        radius = sliderRadius + 15;
        angle = getThumbAngleRadians (leftHighThumb);
    }
    else if (labelToUpdate == rangeLabelMinRight)
    {
        radius = sliderRadius + 15;
        angle = getThumbAngleRadians (rightLowThumb);
    }
    else if (labelToUpdate == rangeLabelMaxRight)
    {
        radius = sliderRadius + 15;
        angle = getThumbAngleRadians (rightHighThumb);
    }
    
    labelToUpdate->setCentrePosition (sliderCentre.x + int (radius * std::cos (angle - MathConstants<float>::halfPi)),
                                      sliderCentre.y + int (radius * std::sin (angle - MathConstants<float>::halfPi)));
}

float TwoRangeTuner::getValueAngle()
{
	float convertedValue = value;//gestureRange.convertFrom0to1 (value);

    float cursorAngle;

    if (gestureRange.getRange().getLength() > 0)
    {
        // Cursor stays at same angle if value out of range.
        if (convertedValue < parameterMax.getStart() || convertedValue > parameterMax.getEnd())
        {
            cursorAngle = previousCursorAngle;
        }
        else
        {
            cursorAngle = startAngle + (convertedValue - parameterMax.getStart()) * (endAngle - startAngle)
                                        / parameterMax.getLength();
        }
    }
    else
    {
        cursorAngle = startAngle;
    }

    // In case the angle somehow exceeds 2*pi
    while (cursorAngle > MathConstants<float>::twoPi)
    {
        cursorAngle -= MathConstants<float>::twoPi;
    }

    return cursorAngle;
}
void TwoRangeTuner::drawValueCursor (Graphics& g)
{
    float cursorAngle = getValueAngle();
    previousCursorAngle = cursorAngle;

    auto cursorRadius = sliderRadius + 7;
    juce::Point<float> cursorPoint (sliderCentre.x + cursorRadius * std::cos (cursorAngle - MathConstants<float>::halfPi),
                              sliderCentre.y + cursorRadius * std::sin (cursorAngle - MathConstants<float>::halfPi));

    Path cursorPath;
    cursorPath.addTriangle ({cursorPoint.x - 3.0f, cursorPoint.y - 3.0f},
                            {cursorPoint.x + 3.0f, cursorPoint.y - 3.0f},
                            {cursorPoint.x,        cursorPoint.y + 3.0f});
    
    AffineTransform transform = AffineTransform::rotation (cursorAngle,
                                                           cursorPath.getBounds().getCentreX(),
                                                           cursorPath.getBounds().getCentreY());

    g.setColour (   ((/*gestureRange.convertFrom0to1 (value)*/ value >= getRangeLeftLow())
                        && (/*gestureRange.convertFrom0to1 (value)*/ value <= getRangeLeftHigh()))
                 ||  ((/*gestureRange.convertFrom0to1 (value)*/ value >= getRangeRightLow())
                        && (/*gestureRange.convertFrom0to1 (value)*/ value <= getRangeRightHigh()))
                  ? tunerColour
                  : Colour (0x80808080));
    g.fillPath (cursorPath, transform);
}

void TwoRangeTuner::drawLineFromSliderCentre (Graphics& g, float angleRadian)
{
    juce::Point<float> point (sliderCentre.x + sliderRadius * std::cos (angleRadian - MathConstants<float>::halfPi),
                        sliderCentre.y + sliderRadius * std::sin (angleRadian - MathConstants<float>::halfPi));

    g.drawLine (Line<float> (sliderCentre.toFloat(), point), 1.0f);
}

void TwoRangeTuner::drawThumbsAndToleranceLines (Graphics& g)
{
    double leftTolerance = ((leftHighSlider->getValue() - leftLowSlider->getValue())*(std::abs (endAngle - startAngle)))
                         / (leftLowSlider->getRange().getLength() * 5);

    double rightTolerance = ((rightHighSlider->getValue() - rightLowSlider->getValue())*(std::abs (endAngle - startAngle)))
                         / (rightLowSlider->getRange().getLength() * 5);

    g.setColour (tunerColour.withAlpha (0.5f));
    drawLineFromSliderCentre (g, getThumbAngleRadians (leftLowThumb));
    drawLineFromSliderCentre (g, getThumbAngleRadians (leftHighThumb));
    drawLineFromSliderCentre (g, getThumbAngleRadians (rightLowThumb));
    drawLineFromSliderCentre (g, getThumbAngleRadians (rightHighThumb));

    g.setColour (Colour (0xff903030));
    bool invertTolerance = startAngle > endAngle;
    drawLineFromSliderCentre (g, getThumbAngleRadians (leftLowThumb) + (invertTolerance ? -leftTolerance
                                                                                    : leftTolerance));
    drawLineFromSliderCentre (g, getThumbAngleRadians (leftHighThumb) + (invertTolerance ? leftTolerance
                                                                                     : -leftTolerance));
    drawLineFromSliderCentre (g, getThumbAngleRadians (rightLowThumb) + (invertTolerance ? -rightTolerance
                                                                                    : rightTolerance));
    drawLineFromSliderCentre (g, getThumbAngleRadians (rightHighThumb) + (invertTolerance ? rightTolerance
                                                                                     : -rightTolerance));
}