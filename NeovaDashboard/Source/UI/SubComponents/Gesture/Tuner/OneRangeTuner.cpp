/*
  ==============================================================================

    OneRangeTuner.cpp
    Created: 13 Sep 2019 3:57:33pm
    Author:  Enhancia Dev

  ==============================================================================
*/

#include "OneRangeTuner.h"


OneRangeTuner::OneRangeTuner (HubConfiguration& config, const int gestureId,
							  const float& val, NormalisableRange<float> gestRange,
                   			  const Range<float> paramMax, const String unit, TunerStyle style)

    : Tuner (unit, neova_dash::gesture::getHighlightColour (config.getGestureData (gestureId).type,
    														config.isGestureActive (gestureId))),
      hubConfig (config), id (gestureId),
      value (val), gestureRange (gestRange),
      parameterMax (paramMax), tunerStyle (style)
{
    setLookAndFeel (&rangeTunerLookAndFeel);
    setStyle (style);
    createSliders();
    createLabels();
    createButtons();
}

OneRangeTuner::~OneRangeTuner()
{
    lowSlider = nullptr;
    highSlider = nullptr;
    rangeLabelMin = nullptr;
    rangeLabelMax = nullptr;

    setLookAndFeel (nullptr);
}
    
//==============================================================================
void OneRangeTuner::paint (Graphics& g)
{
    drawTunerSliderBackground (g);
    drawValueCursor (g);

    g.setColour (neova_dash::colour::subText);
    g.setFont (neova_dash::font::dashFontLight.withHeight (13.0f).withExtraKerningFactor (0.08f));
    juce::Rectangle<int> textRect (100, 50);
    textRect.setCentre ({getLocalBounds().getCentreX(),
                         sliderBounds.getCentreY() + static_cast<int>(jmin(sliderBounds.getWidth() / 4.0f,
                                                                           sliderBounds.getHeight() / 4.0f))});

    g.drawText ("RANGE", textRect, Justification::centredTop);
}

void OneRangeTuner::resized()
{
    // Sets bounds and changes the slider and labels position
    sliderBounds = getLocalBounds().reduced (neova_dash::ui::MARGIN).translated (0, jmax (20, getHeight()/8));
    resizeSliders();
    resizeButtons();

    updateLabelBounds (rangeLabelMin.get());
    updateLabelBounds (rangeLabelMax.get());

    repaint();
}

void OneRangeTuner::resizeSliders()
{
    switch (tunerStyle)
    {
        case wave:
            sliderRadius = jmin (sliderBounds.getWidth()/2.0f, sliderBounds.getHeight()/2.0f);
            sliderCentre = {getLocalBounds().getCentreX() - static_cast<int>(sliderRadius)/2,
                            sliderBounds.getCentreY()};
            break;

        case tilt:
            sliderRadius = jmin (sliderBounds.getWidth()*2/3.0f, sliderBounds.getHeight()*2/3.0f);
            sliderCentre = {getLocalBounds().getCentreX() + static_cast<int>(sliderRadius)/2,
                            sliderBounds.getCentreY() + static_cast<int>(sliderRadius)/2};
            break;

        case roll:
            sliderRadius = jmin (sliderBounds.getWidth()/2.0f, sliderBounds.getHeight()/2.0f);
            sliderCentre = {getLocalBounds().getCentreX(),
                            sliderBounds.getCentreY() + static_cast<int>(sliderRadius)/2};
            break;
    }

    juce::Rectangle<int> adjustedBounds = sliderBounds.withWidth (static_cast<int>(sliderRadius)*2)
                                                      .withHeight (static_cast<int>(sliderRadius)*2)
                                                      .expanded (10)
                                                      .withCentre (sliderCentre);

    lowSlider->setBounds (adjustedBounds);
    highSlider->setBounds (adjustedBounds);
}

void OneRangeTuner::resizeButtons()
{
    using namespace neova_dash::ui;

    auto buttonsArea = getLocalBounds().withLeft (getLocalBounds().getRight() - 80)
                                       .withHeight (70)
                                       .reduced (MARGIN);

    maxAngleButton->setBounds (buttonsArea.removeFromTop (buttonsArea.getHeight()/2).withTrimmedBottom (MARGIN/2));
    minAngleButton->setBounds (buttonsArea.withTrimmedTop (MARGIN/2));
}
    
void OneRangeTuner::updateComponents()
{
    // Sets slider value
    if (lowSlider->getThumbBeingDragged() == -1)
    {
        lowSlider->setValue (static_cast<double>(getRangeLow()), dontSendNotification);
    }

    if (highSlider->getThumbBeingDragged() == -1)
    {
        highSlider->setValue (static_cast<double>(getRangeHigh()), dontSendNotification);
    }
    
    // Sets label text
    if (!(rangeLabelMin->isBeingEdited()))
    {
        rangeLabelMin->setText (String (static_cast<int>(getRangeLow())) + valueUnit, dontSendNotification);
    }
      
    if (!(rangeLabelMax->isBeingEdited()))
    {
        rangeLabelMax->setText (String (static_cast<int>(getRangeHigh())) + valueUnit, dontSendNotification);
    }
    
}

void OneRangeTuner::updateDisplay()
{
    if (getValueAngle() != previousCursorAngle)
    {
        repaint (sliderBounds);
    }
}

void OneRangeTuner::setColour (const Colour newColour)
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

    setLabelColours (*rangeLabelMin);
    setLabelColours (*rangeLabelMax);

    repaint();
}

void OneRangeTuner::updateColour()
{
	setColour (neova_dash::gesture::getHighlightColour (hubConfig.getGestureData (id).type,
    												    hubConfig.isGestureActive (id)));
}
    
void OneRangeTuner::setStyle (TunerStyle newStyle)
{
    tunerStyle = newStyle;

    switch (newStyle)
    {
        case wave:
            setAngles (0.0f, MathConstants<float>::pi);
            //tunerColour = neova_dash::colour::wave;
            break;

        case tilt:
            setAngles (MathConstants<float>::pi*16/10, MathConstants<float>::twoPi);
            //tunerColour = neova_dash::colour::tilt;
            break;

        case roll:
            setAngles (MathConstants<float>::pi*5/3, MathConstants<float>::pi*7/3);
            //tunerColour = neova_dash::colour::roll;
            break;
    }
}
void OneRangeTuner::setAngles (float start, float end)
{
    startAngle = start;
    endAngle = end;
}

//==============================================================================
void OneRangeTuner::labelTextChanged (Label* lbl)
{
    // checks that the string is numbers only (and unit)
    if (lbl->getText().containsOnly ("-0123456789"+valueUnit) == false)
    {
        if (lbl == rangeLabelMin.get())       lbl->setText (String (static_cast<int>(getRangeLow())) + valueUnit, dontSendNotification);
        else if (lbl == rangeLabelMax.get())  lbl->setText (String (static_cast<int>(getRangeHigh())) + valueUnit, dontSendNotification);

        return;
    }
        
    float val;
        
    // Gets the float value of the text 
    if (valueUnit != "" &&
        lbl->getText().endsWith(valueUnit)) val = lbl->getText().upToFirstOccurrenceOf(valueUnit, false, false)
                                                                .getFloatValue();
    else                                    val = lbl->getText().getFloatValue();
        
    if (val < parameterMax.getStart())    val = parameterMax.getStart();
    else if (val > parameterMax.getEnd()) val = parameterMax.getEnd();
        
    // Sets slider and labels accordingly
    if (lbl == rangeLabelMin.get())
    {
        if ( val > getRangeHigh()) val = getRangeHigh();
            
        lowSlider->setValue (val, sendNotification);
    	setRangeLow (static_cast<float>(lowSlider->getValue()), false);
    	setRangeHigh (static_cast<float>(highSlider->getValue()));
        updateLabelBounds (rangeLabelMin.get());
        lbl->setText (String (static_cast<int>(getRangeLow())) + valueUnit, dontSendNotification);
    }
    
    else if (lbl == rangeLabelMax.get())
    {
        if ( val < getRangeLow()) val = getRangeLow();
            
        highSlider->setValue (val, sendNotification);
    	setRangeLow (static_cast<float>(lowSlider->getValue()), false);
    	setRangeHigh (static_cast<float>(highSlider->getValue()));
        updateLabelBounds (rangeLabelMax.get());
        lbl->setText (String (static_cast<int>(getRangeHigh())) + valueUnit, dontSendNotification);
    }
}

void OneRangeTuner::editorShown (Label*, TextEditor& ted)
{
    ted.setJustification (Justification::centred);
}

void OneRangeTuner::editorHidden (Label* lbl, TextEditor&)
{
    lbl->setVisible (false);
}
    
void OneRangeTuner::sliderValueChanged (Slider* sldr)
{
    //ScopedLock sliderLock (rangeUpdateLock);
    
    if (sldr == lowSlider.get())
    {
        // min value changed by user
        updateLabelBounds (rangeLabelMin.get());
        rangeLabelMin->setText (String (static_cast<int>(sldr->getValue())) + valueUnit, dontSendNotification);
        
        // in case the other thumb is dragged along..
        if (highSlider->getThumbBeingDragged() == -1 && lowSlider->getValue() > highSlider->getValue())
        {
            highSlider->setValue (sldr->getValue(), dontSendNotification);
            updateLabelBounds (rangeLabelMax.get());
            rangeLabelMax->setText (String (static_cast<float>(sldr->getValue())) + valueUnit, dontSendNotification);
        }
    }

    // max value changed by user
    else if (sldr == highSlider.get())
    {
        updateLabelBounds (rangeLabelMax.get());
        rangeLabelMax->setText (String (static_cast<int>(sldr->getValue())) + valueUnit, dontSendNotification);
        
        // in case the other thumb is dragged along..
        if (lowSlider->getThumbBeingDragged() == -1 && lowSlider->getValue() > highSlider->getValue())
        {
            lowSlider->setValue (sldr->getValue(), dontSendNotification);
            updateLabelBounds (rangeLabelMin.get());
            rangeLabelMin->setText (String (static_cast<float>(sldr->getValue())) + valueUnit, dontSendNotification);
        }
    }
}

void OneRangeTuner::buttonClicked (Button* bttn)
{
    if (bttn == minAngleButton.get())
    {
        if (value > getRangeHigh())
        {
            highSlider->setValue (value, sendNotification);
            setRangeHigh (static_cast<float>(highSlider->getValue()));
        }

        lowSlider->setValue (value, sendNotification);
        setRangeLow (static_cast<float>(lowSlider->getValue()));
    }

    else if (bttn == maxAngleButton.get())
    {
        if (value < getRangeLow())
        {
            lowSlider->setValue (value, sendNotification);
            setRangeLow (static_cast<float>(lowSlider->getValue()));
        }

        highSlider->setValue (value, sendNotification);
        setRangeHigh (static_cast<float>(highSlider->getValue()));
    }
}

void OneRangeTuner::buttonStateChanged (Button* btn) {

    if (btn->isOver ()) {

        if (btn == maxAngleButton.get())
            maxAngleBtnIsHovered = true;
        else if (btn == minAngleButton.get())
            minAngleBtnIsHovered = true;
        else
            return;

        repaint ();
    }
    else {
        maxAngleBtnIsHovered = false;
        minAngleBtnIsHovered = false;
        repaint ();
    }
}

void OneRangeTuner::mouseDown (const MouseEvent& e)
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

void OneRangeTuner::handleSingleClick (const MouseEvent& e)
{
    objectBeingDragged = getObjectToDrag (e);
    
    if (objectBeingDragged == lowThumb)
    {
        lowSlider->mouseDown (e.getEventRelativeTo (lowSlider.get()));
        rangeLabelMin->setVisible (true);
    }
    else if (objectBeingDragged == highThumb)
    {
        highSlider->mouseDown (e.getEventRelativeTo (highSlider.get()));
        rangeLabelMax->setVisible (true);
    }
    else if (objectBeingDragged == middleArea)
    {
        lowSlider->setSliderStyle (tunerStyle == wave ? Slider::RotaryVerticalDrag
                                                      : tunerStyle == tilt ? Slider::RotaryHorizontalVerticalDrag
                                                                           : Slider::RotaryHorizontalDrag);
        highSlider->setSliderStyle (tunerStyle == wave ? Slider::RotaryVerticalDrag
                                                       : tunerStyle == tilt ? Slider::RotaryHorizontalVerticalDrag
                                                                            : Slider::RotaryHorizontalDrag);

        lowSlider->mouseDown (e.getEventRelativeTo (lowSlider.get()));
        highSlider->mouseDown (e.getEventRelativeTo (highSlider.get()));
        rangeLabelMin->setVisible (true);
        rangeLabelMax->setVisible (true);
    }

    updateMouseCursor();
    repaint (sliderBounds);
}

void OneRangeTuner::handleDoubleClick (const MouseEvent& e)
{
    if (getObjectToDrag (e) == lowThumb)
    {
        rangeLabelMin->setVisible (true);
        rangeLabelMin->showEditor();
    }

    else if (getObjectToDrag (e) == highThumb)
    {
        rangeLabelMax->setVisible (true);
        rangeLabelMax->showEditor();
    }
}

void OneRangeTuner::mouseDrag (const MouseEvent& e)
{
    if (!e.mods.isLeftButtonDown() || e.getNumberOfClicks() > 1) return;

    if (objectBeingDragged == lowThumb)
    {
        lowSlider->mouseDrag (e.getEventRelativeTo (lowSlider.get()));
    }
    else if (objectBeingDragged == highThumb)
    {
        highSlider->mouseDrag (e.getEventRelativeTo (highSlider.get()));
    }
    else
    {
        if (tunerStyle == tilt)
        {
            lowSlider->mouseDrag (e.getEventRelativeTo (lowSlider.get()));
            highSlider->mouseDrag (e.getEventRelativeTo (highSlider.get()));
        }
        else
        {
            // Inverts the drag for the y axis
            auto invertedYEvent = e.withNewPosition(juce::Point<int> (e.x, e.getMouseDownY() - e.getDistanceFromDragStartY()));

            lowSlider->mouseDrag (invertedYEvent.getEventRelativeTo (lowSlider.get()));
            highSlider->mouseDrag (invertedYEvent.getEventRelativeTo (highSlider.get()));
        }
    }

    repaint (sliderBounds);
}

void OneRangeTuner::mouseUp (const MouseEvent& e)
{
    if (objectBeingDragged == lowThumb)
    {
        lowSlider->mouseUp (e.getEventRelativeTo (lowSlider.get()));
    }
    else if (objectBeingDragged == highThumb)
    {
        highSlider->mouseUp (e.getEventRelativeTo (highSlider.get()));
    }
    else if (objectBeingDragged == middleArea)
    {
        lowSlider->setSliderStyle (Slider::Rotary);
        highSlider->setSliderStyle (Slider::Rotary);

        lowSlider->mouseUp (e.getEventRelativeTo (lowSlider.get()));
        highSlider->mouseUp (e.getEventRelativeTo (highSlider.get()));
    }

    if (objectBeingDragged != none)
    {
    	setRangeLow (static_cast<float>(lowSlider->getValue()), false);
    	setRangeHigh (static_cast<float>(highSlider->getValue()));
        rangeLabelMin->setVisible (false);
        rangeLabelMax->setVisible (false);
        objectBeingDragged = none;
        updateMouseCursor();
        repaint (sliderBounds);
    }
}

MouseCursor OneRangeTuner::getMouseCursor()
{
    return MouseCursor::NormalCursor;
}

void OneRangeTuner::createSliders()
{
    addAndMakeVisible (*(lowSlider = std::make_unique<Slider> ("Range Low Slider")));
    addAndMakeVisible (*(highSlider = std::make_unique<Slider> ("Range High Slider")));
    
    auto setSliderSettings = [this] (Slider& slider, float valueToSet)
    {
        slider.setSliderStyle (Slider::Rotary);
        slider.setRotaryParameters (startAngle, endAngle, true);
        slider.setSliderSnapsToMousePosition (false);
        slider.setTextBoxStyle (Slider::NoTextBox, false, 0, 0);
        slider.setColour (Slider::rotarySliderFillColourId, Colour (0x00000000));
        slider.setColour (Slider::rotarySliderOutlineColourId, Colour (0x00000000));
        slider.setRange (static_cast<double>(parameterMax.getStart()), static_cast<double>(parameterMax.getEnd()), 1.0);
        slider.setValue (static_cast<double>(valueToSet));
        slider.addListener (this);
        slider.setInterceptsMouseClicks (false, false);
	};

    setSliderSettings (*lowSlider, getRangeLow());
    setSliderSettings (*highSlider, getRangeHigh());
}
    
void OneRangeTuner::createLabels()
{
    addAndMakeVisible (*(rangeLabelMin = std::make_unique<Label> ("Min Label", TRANS (String(static_cast<int>(getRangeLow())) + valueUnit))));
    addAndMakeVisible (*(rangeLabelMax = std::make_unique<Label> ("Max Label", TRANS (String(static_cast<int>(getRangeHigh())) + valueUnit))));

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
    };

    setLabelSettings (*rangeLabelMin);
    setLabelSettings (*rangeLabelMax);
}

void OneRangeTuner::createButtons()
{
    addAndMakeVisible (*(minAngleButton = std::make_unique<TextButton> ("Min Angle Button")));
    addAndMakeVisible (*(maxAngleButton = std::make_unique<TextButton> ("Max Angle Button")));

    auto setButtonSettings = [this] (TextButton& button)
    {
        button.setColour (TextButton::buttonColourId , neova_dash::colour::tunerSliderBackground);
        button.setColour (TextButton::buttonOnColourId , tunerColour);
        button.setColour (TextButton::textColourOffId , neova_dash::colour::mainText);
        button.setColour (TextButton::textColourOnId , neova_dash::colour::mainText);
        button.setButtonText (&button == minAngleButton.get() ? "MIN ANGLE" : "MAX ANGLE");
        button.addListener (this);
    };

    setButtonSettings (*minAngleButton);
    setButtonSettings (*maxAngleButton);
}
    
void OneRangeTuner::setRangeLow (float val, bool uploadToHub)
{
	hubConfig.setFloatValue (id, HubConfiguration::gestureParam0, val, uploadToHub);
}
    
void OneRangeTuner::setRangeHigh (float val, bool uploadToHub)
{
	hubConfig.setFloatValue (id, HubConfiguration::gestureParam1, val, uploadToHub);
}
    
float OneRangeTuner::getRangeLow()
{
    return hubConfig.getGestureData (id).gestureParam0;
}
    
float OneRangeTuner::getRangeHigh()
{
    return hubConfig.getGestureData (id).gestureParam1;
}


double OneRangeTuner::getAngleFromMouseEventRadians (const MouseEvent& e)
{
    double angle =  std::atan2 (e.getMouseDownX() - sliderCentre.x,
                                -(e.getMouseDownY() - sliderCentre.y));

    if (angle < 0) angle += MathConstants<double>::twoPi;

    return angle;
}

double OneRangeTuner::getThumbAngleRadians (const DraggableObject thumb)
{
    if (thumb != lowThumb && thumb != highThumb) return -0.01;
    
    Slider& slider = (thumb == lowThumb) ? *lowSlider : *highSlider;

    return startAngle + ((slider.getValue() - slider.getMinimum())/slider.getRange().getLength())
                             *(endAngle - startAngle);
}

OneRangeTuner::DraggableObject OneRangeTuner::getObjectToDrag (const MouseEvent& e)
{
    //================ Figures out the scenario surrounding the click ==============

    if (e.mods.isShiftDown()) return middleArea;

    double mouseAngle = getAngleFromMouseEventRadians (e);
    bool inverted = startAngle > endAngle;


    // Computes "% 2*pi" if both angles are somehow greater that 2*pi
    float startAngleModulo = startAngle, endAngleModulo = endAngle;
    while (startAngleModulo > MathConstants<float>::twoPi
           && endAngleModulo > MathConstants<float>::twoPi)
    {
        startAngleModulo -= MathConstants<float>::twoPi;
        endAngleModulo -= MathConstants<float>::twoPi;
    }

    // If only one extreme angle is higher than 2*pi, the process needs a different logic.
    // Boolean "differentRef" notifies that this is the case.
    bool differentRef = jmax(startAngleModulo, endAngleModulo) > MathConstants<float>::twoPi
                        && jmin(startAngleModulo, endAngleModulo) < MathConstants<float>::twoPi;
    
    // If the higher angle minus 2pi is still higher that the smaller, then we fall back to the
    // previous logic with "inverted" angles.
    if (differentRef && jmax(startAngleModulo, endAngleModulo) - MathConstants<float>::twoPi
                        > jmin(startAngleModulo, endAngleModulo))
    {
        differentRef = false;
        inverted ? startAngleModulo -= MathConstants<float>::twoPi
                 : endAngleModulo -= MathConstants<float>::twoPi;
    }

    //================ Finds the object to drag ==============

    DraggableObject thumb1 = inverted ? lowThumb : highThumb;
    DraggableObject thumb2 = inverted ? highThumb : lowThumb;

    // Tolerance: area around which a thumb can be clicked, its value is the 5th of the angle between the two thumbs.
    double tolerance = ((highSlider->getValue() - lowSlider->getValue())*(std::abs (endAngle - startAngle)))
                                /(lowSlider->getRange().getLength()*5);

    // Click within the slider range
    if ((!differentRef && (mouseAngle > jmin (startAngleModulo, endAngleModulo)
                       && mouseAngle < jmax (startAngleModulo, endAngleModulo)))
        || (differentRef && (mouseAngle > jmin(startAngleModulo, endAngleModulo)
                                || mouseAngle + MathConstants<float>::twoPi
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
            if (mouseAngle < jmax(startAngleModulo, endAngleModulo) - MathConstants<float>::twoPi)
                mouseAngle += MathConstants<float>::twoPi;

            if (mouseAngle > getThumbAngleRadians(thumb1) - tolerance)      return thumb1;
            else if (mouseAngle < getThumbAngleRadians(thumb2) + tolerance) return thumb2;
            else return middleArea;
        }
    }
    
    return none; // Somehow no thumb could be chosen...
}

void OneRangeTuner::drawTunerSliderBackground (Graphics& g)
{
    auto outline = neova_dash::colour::tunerSliderBackground;
    auto fill    = objectBeingDragged == middleArea ? tunerColour.interpolatedWith (Colour (0xffffffff), 0.8f)
                                                    : tunerColour;

    auto lowAngle = startAngle
                        + (lowSlider->getValue() - lowSlider->getMinimum()) / lowSlider->getRange().getLength()
                                                                          * (endAngle - startAngle);
    auto highAngle = startAngle
                        + (highSlider->getValue() - lowSlider->getMinimum()) / lowSlider->getRange().getLength()
                                                                           * (endAngle - startAngle);
    auto lineW = jmin (8.0f, sliderRadius * 0.5f);
    auto arcRadius = sliderRadius - lineW * 0.5f;

    Path backgroundArc;
    backgroundArc.addCentredArc (static_cast<float>(sliderCentre.x),
                                 static_cast<float>(sliderCentre.y),
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
        Path valueArc;
        valueArc.addCentredArc (static_cast<float>(sliderCentre.x),
                                static_cast<float>(sliderCentre.y),
                                arcRadius,
                                arcRadius,
                                0.0f,
                                static_cast<float>(lowAngle),
                                static_cast<float>(highAngle),
                                true);

        g.setColour (fill);
        g.strokePath (valueArc, PathStrokeType (6.0f, PathStrokeType::curved, PathStrokeType::rounded));
    }

    if (objectBeingDragged == lowThumb || objectBeingDragged == highThumb)
    {
        auto angle = getThumbAngleRadians (objectBeingDragged);

        juce::Point<float> thumbPoint (static_cast<float>(sliderCentre.x) + arcRadius * std::cos (static_cast<float>(angle) - MathConstants<float>::halfPi),
                                 static_cast<float>(sliderCentre.y) + arcRadius * std::sin (static_cast<float>(angle) - MathConstants<float>::halfPi));

        g.setColour (fill.withAlpha (0.6f));
        /*g.setGradientFill (ColourGradient (fill.withAlpha (0.9f),
                                           thumbPoint,
                                           Colour (0),
                                           thumbPoint.translated (12.0f, 0),
                                           true));*/

        g.fillEllipse (juce::Rectangle<float> (25.0f, 25.0f).withCentre (thumbPoint));
    }

    // Add highlight on tuner thumb when min/max button are hoverred
    if (maxAngleBtnIsHovered || minAngleBtnIsHovered) {

        double angle;

        if (maxAngleBtnIsHovered)
            angle = getThumbAngleRadians (highThumb);
        else
            angle = getThumbAngleRadians (lowThumb);

        juce::Point<float> thumbPoint (static_cast<float>(sliderCentre.x) + arcRadius * std::cos (static_cast<float>(angle) - MathConstants<float>::halfPi),
            static_cast<float>(sliderCentre.y) + arcRadius * std::sin (static_cast<float>(angle) - MathConstants<float>::halfPi));

        g.setColour (fill.withAlpha (0.3f));
        g.fillEllipse (juce::Rectangle<float> (25.0f, 25.0f).withCentre (thumbPoint));
    }
}

void OneRangeTuner::updateLabelBounds (Label* labelToUpdate)
{
    if (labelToUpdate == nullptr) return;

    if (labelToUpdate == rangeLabelMin.get())
    {
        auto radius = sliderRadius + 15;
        auto angle = getThumbAngleRadians (lowThumb);

        rangeLabelMin->setCentrePosition (static_cast<int>(sliderCentre.x + radius * std::cos (angle - MathConstants<float>::halfPi)),
                                          static_cast<int>(sliderCentre.y + radius * std::sin (angle - MathConstants<float>::halfPi)));

    }
    else if (labelToUpdate == rangeLabelMax.get())
    {
        auto radius = sliderRadius + 15;
        auto angle = getThumbAngleRadians (highThumb);

        rangeLabelMax->setCentrePosition (static_cast<int>(sliderCentre.x + radius * std::cos (angle - MathConstants<float>::halfPi)),
                                          static_cast<int>(sliderCentre.y + radius * std::sin (angle - MathConstants<float>::halfPi)));
    }
}

float OneRangeTuner::getValueAngle()
{
    float convertedValue = value; //gestureRange.convertFrom0to1 (value);

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

void OneRangeTuner::drawValueCursor (Graphics& g)
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

    g.setColour ((/*gestureRange.convertFrom0to1 (value)*/ value >= getRangeLow())
                 && (/*gestureRange.convertFrom0to1 (value)*/ value <= getRangeHigh()) ? tunerColour
                                                                             : Colour (0x80808080));
    g.fillPath (cursorPath, transform);
}

void OneRangeTuner::drawLineFromSliderCentre (Graphics& g, float angleRadian)
{
    juce::Point<float> point (sliderCentre.x + sliderRadius * std::cos (angleRadian - MathConstants<float>::halfPi),
                        sliderCentre.y + sliderRadius * std::sin (angleRadian - MathConstants<float>::halfPi));

    g.drawLine (Line<float> (sliderCentre.toFloat(), point), 1.0f);
}

void OneRangeTuner::drawThumbsAndToleranceLines (Graphics& g)
{
    double tolerance = ((highSlider->getValue() - lowSlider->getValue())*(std::abs (endAngle - startAngle)))
                         / (lowSlider->getRange().getLength() * 5);

    g.setColour (Colour (0xffdedeff));
    drawLineFromSliderCentre (g, static_cast<float>(getThumbAngleRadians(lowThumb)));
    drawLineFromSliderCentre (g, static_cast<float>(getThumbAngleRadians(highThumb)));

    g.setColour (Colour (0xff903030));
    bool invertTolerance = startAngle > endAngle;
    drawLineFromSliderCentre (g, static_cast<float>(getThumbAngleRadians(lowThumb)) + static_cast<float>((invertTolerance ? -tolerance
                                  : tolerance)));
    drawLineFromSliderCentre (g, static_cast<float>(getThumbAngleRadians(highThumb)) + static_cast<float>((invertTolerance ? tolerance
                                  : -tolerance)));
}