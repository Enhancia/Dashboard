/*
  ==============================================================================

    MidiPanel.cpp
    Created: 12 Sep 2019 11:43:05am
    Author:  Enhancia Dev

  ==============================================================================
*/

#include "MidiPanel.h"

MidiPanel::MidiPanel (HubConfiguration& config, DataReader& reader, const int gestId)
    : hubConfig (config), dataReader (reader), id (gestId)
{
    createComboBox();
    createLabels();
    createButton();

    addAndMakeVisible (*(midiRangeTuner = std::make_unique<MidiRangeTuner> (config, dataReader, gestId)));

    setComponentsVisibility();
}

MidiPanel::~MidiPanel()
{
    midiTypeBox->removeListener (this);
    ccLabel->removeListener (this);
    
    midiTypeBox = nullptr;
    ccLabel = nullptr;
    midiRangeTuner = nullptr;
    reverseButton = nullptr;
}

//==============================================================================
void MidiPanel::paint (Graphics& g)
{
    auto area = getLocalBounds().reduced (neova_dash::ui::MARGIN)
                                .withTrimmedLeft (neova_dash::ui::MARGIN);
    area.removeFromRight (getWidth()*2/3);

    g.setColour (neova_dash::colour::subText);
    g.setFont (neova_dash::font::dashFont.withHeight (13.0f));

    g.drawText ("MIDI Type:",
                area.withHeight (30).withCentre ({area.getCentreX(),
                                                  midiTypeBox->getBounds().getCentreY()}),
                Justification::centredLeft, false);

    g.drawText ("MIDI Range:",
                area.withHeight (30).withCentre ({area.getCentreX(),
                                                  midiRangeTuner->getBounds().getCentreY()}),
                Justification::centredLeft, false);
}

void MidiPanel::resized()
{
    using namespace neova_dash::ui;

	auto area = getLocalBounds().reduced (MARGIN, 0)
                                .withTrimmedBottom (MARGIN)
                                .withTrimmedTop (MARGIN_SMALL);
	area.removeFromLeft (area.getWidth()/3);

    auto typeArea = area.removeFromTop (getHeight()/2);
    midiTypeBox->setBounds (typeArea.removeFromLeft (typeArea.getWidth()/2)
    								.withSizeKeepingCentre (area.getWidth()*4/10, 20));
    ccLabel->setBounds (typeArea.withSizeKeepingCentre (area.getWidth()/4, 20));

    auto rangeArea = area.withTrimmedBottom (MARGIN).withTrimmedRight (MARGIN);

    reverseButton->setBounds (rangeArea.removeFromRight (18 + MARGIN)
                                           .withSizeKeepingCentre (18, 18));

    midiRangeTuner->setBounds (rangeArea.withSizeKeepingCentre (rangeArea.getWidth() - 2*MARGIN, 40));
}

//==============================================================================
void MidiPanel::labelTextChanged (Label* lbl)
{
    if (lbl == ccLabel.get())
    {
        // checks that the string is numbers only
        if (lbl->getText().containsOnly ("0123456789") == false)
        {
            lbl->setText (String (hubConfig.getGestureData (id).cc), dontSendNotification);
            return;
        }

        int val = lbl->getText().getIntValue();
    
        if (val < 0)
        {
        	val = 0;
        }
        else if (val > 127)
        {
        	val = 127;
        }

        lbl->setText (String(val), dontSendNotification);
        hubConfig.setUint8Value (id, HubConfiguration::cc, static_cast<uint8>(val));

        if (auto* gestPanel = getParentComponent()->getParentComponent())
        	gestPanel->repaint();
    }
}

void MidiPanel::editorShown (Label*, TextEditor& ted)
{
    ted.setJustification (Justification::centred);
}

void MidiPanel::comboBoxChanged (ComboBox* box)
{
    if (box == midiTypeBox.get())
    {
        hubConfig.setUint8Value (id, HubConfiguration::midiType, static_cast<uint8>(midiTypeBox->getSelectedId () - 1));

        if (auto* gestPanel = getParentComponent()->getParentComponent())
            gestPanel->repaint();
        
        setComponentsVisibility();
    }
}

void MidiPanel::buttonClicked (Button* bttn)
{
    if (bttn == reverseButton.get())
    {
        hubConfig.setUint8Value (id, HubConfiguration::reverse,
                                     reverseButton->getToggleState() ? 1 : 0);
    }
}

void MidiPanel::updateComponents()
{
    ccLabel->setText (String (hubConfig.getGestureData (id).cc), dontSendNotification);

    //reverseButton->setToggleState (hubConfig.hubConfig.getGestureData (id).reversed); TODO
    reverseButton->setColour (TextButton::buttonOnColourId,
                              neova_dash::gesture::getHighlightColour (hubConfig.getGestureData (id).type,
                                                                       hubConfig.isGestureActive (id)));
    midiRangeTuner->updateHighlightColour();
}

void MidiPanel::updateDisplay()
{
    midiRangeTuner->updateDisplay();
}

MidiRangeTuner& MidiPanel::getTuner()
{
    return *midiRangeTuner;
}

//==============================================================================
void MidiPanel::createComboBox()
{
    addAndMakeVisible (*(midiTypeBox = std::make_unique<ComboBox> ("midiTypeBox")));

    midiTypeBox->addItem ("Pitch", neova_dash::gesture::pitchMidi + 1);
    midiTypeBox->addItem ("CC", neova_dash::gesture::ccMidi + 1);
    midiTypeBox->addItem ("AfterTouch", neova_dash::gesture::afterTouchMidi + 1);

    if (hubConfig.getGestureData (id).type == neova_dash::gesture::none)
    {
        midiTypeBox->setSelectedId (0, dontSendNotification);
    }
    else
    {
        midiTypeBox->setSelectedId (int (hubConfig.getGestureData (id).midiType) + 1, dontSendNotification);
    }
    
    // ComboBox look
    midiTypeBox->setJustificationType (Justification::centred);
    midiTypeBox->setColour (ComboBox::outlineColourId, neova_dash::colour::subText);
    midiTypeBox->setColour (ComboBox::textColourId, neova_dash::colour::mainText);
    midiTypeBox->setColour (ComboBox::arrowColourId, neova_dash::colour::subText);

    midiTypeBox->addListener (this);
}

void MidiPanel::createLabels()
{
    //=== Midi Type label ===
    
    // CC label
    addAndMakeVisible (*(ccLabel = std::make_unique<Label> ("CC Label", TRANS (String(hubConfig.getGestureData (id).cc)))));
    ccLabel->setEditable ((midiTypeBox->getSelectedId() == 1), false, false);
    ccLabel->setFont (neova_dash::font::dashFont.withHeight (13.0f));
    ccLabel->setJustificationType (Justification::centred);

    ccLabel->setColour (Label::backgroundColourId , Colour (0));
    ccLabel->setColour (Label::textColourId, neova_dash::colour::mainText);
    ccLabel->setColour (Label::outlineColourId, neova_dash::colour::subText);
    
    ccLabel->addListener (this);
}

void MidiPanel::createButton()
{
    reverseButton = std::make_unique<TextButton> ("Reverse Button");
    addAndMakeVisible (*reverseButton);

    reverseButton->setButtonText ("R");
    reverseButton->setColour (TextButton::buttonColourId, neova_dash::colour::tunerSliderBackground);
    reverseButton->setColour (TextButton::buttonOnColourId,
                              neova_dash::gesture::getHighlightColour (hubConfig.getGestureData (id).type,
                                                                       hubConfig.isGestureActive (id)));
    reverseButton->setColour (TextButton::textColourOnId , neova_dash::colour::mainText);
    reverseButton->setColour (TextButton::textColourOffId , neova_dash::colour::mainText);
    reverseButton->setToggleState (hubConfig.getGestureData (id).reverse != 0, dontSendNotification);
    reverseButton->setClickingTogglesState (true);
    reverseButton->addListener (this);
}

void MidiPanel::setComponentsVisibility()
{
    bool shouldGrayOutBox =   (hubConfig.getGestureData (id).type == neova_dash::gesture::none);

    bool shouldGrayOutTuner = (hubConfig.getGestureData (id).midiType == neova_dash::gesture::pitchMidi||
                               hubConfig.getGestureData (id).type == neova_dash::gesture::none);

    midiTypeBox->setAlpha (shouldGrayOutBox ? 0.3f : 1.0f);
    midiTypeBox->setInterceptsMouseClicks (!shouldGrayOutBox, false);

    /*
    midiRangeTuner->setRangeLow (shouldGrayOutTuner ? 0.0f
                                                    : float (hubConfig.getGestureData (id).midiLow));
    midiRangeTuner->setRangeHigh (shouldGrayOutTuner ? 127.0f
                                                     : float (hubConfig.getGestureData (id).midiHigh));*/

    midiRangeTuner->setAlpha (shouldGrayOutTuner ? 0.3f : 1.0f);
    midiRangeTuner->setInterceptsMouseClicks (!shouldGrayOutTuner, false);
    midiRangeTuner->setEnabled (!shouldGrayOutTuner);
    
    reverseButton->setAlpha (shouldGrayOutTuner ? 0.3f : 1.0f);
    reverseButton->setInterceptsMouseClicks (!shouldGrayOutTuner, false);

    ccLabel->setEditable (midiTypeBox->getSelectedId() == neova_dash::gesture::ccMidi + 1, false, false);
    ccLabel->setVisible (midiTypeBox->getSelectedId() == neova_dash::gesture::ccMidi + 1);
}

//==============================================================================
// Midi Range Tuner

MidiRangeTuner::MidiRangeTuner (HubConfiguration& config, DataReader& reader, const int gestId)
    : hubConfig (config), dataReader (reader), id (gestId)
{
    highlightColour = neova_dash::gesture::getHighlightColour (hubConfig.getGestureData (id).type,
    														   hubConfig.isGestureActive (id));
    createLabels();
    createSliders();
}

MidiRangeTuner::~MidiRangeTuner()
{
    rangeLabelMin = nullptr;
    rangeLabelMax = nullptr;
    lowSlider = nullptr;
    highSlider = nullptr;
}

void MidiRangeTuner::paint (Graphics& g)
{
    drawSliderBackground (g);

    if (hubConfig.isGestureActive (id)) drawCursor (g);
}

void MidiRangeTuner::resized()
{
    lowSlider->setBounds (getLocalBounds());
    highSlider->setBounds (getLocalBounds());

    lowSlider->setMouseDragSensitivity (getWidth());
    highSlider->setMouseDragSensitivity (getWidth());

    setLabelBounds (*rangeLabelMin);
    setLabelBounds (*rangeLabelMax);
}

void MidiRangeTuner::labelTextChanged (Label* lbl)
{
    // checks that the string is numbers only (and dot)
    if (lbl->getText().containsOnly ("0123456789") == false)
    {
        if (lbl == rangeLabelMin.get())       lbl->setText (String (getRangeLow()),
        											  dontSendNotification);
        else if (lbl == rangeLabelMax.get())  lbl->setText (String (getRangeHigh()),
        											  dontSendNotification);

        return;
    }
    
    // Gets the float value of the text 
    float val = lbl->getText().getFloatValue();
    
    if (val < 0.0f)      val = 0.0f;
    else if (val > 127.0f) val = 127.0f;
    
    // Sets slider and labels accordingly
    if (lbl == rangeLabelMin.get())
    {
        // Min > Max
        if ( val > getRangeHigh()) val = getRangeHigh();
        
        // Normal case
        {
            setRangeLow (val);
            lbl->setText (String (val), dontSendNotification);
            lowSlider->setValue (val, dontSendNotification);
            setLabelBounds (*rangeLabelMin);
            rangeLabelMin->setVisible (false);
        }
    }
    else if (lbl == rangeLabelMax.get())
    {
        // Max < Min
        if ( val <  getRangeLow()) val = getRangeLow();
        
        // Normal case
        {
            setRangeHigh (val);
            lbl->setText (String (val), dontSendNotification);
            highSlider->setValue (val, dontSendNotification);
            setLabelBounds (*rangeLabelMax);
            rangeLabelMax->setVisible (false);
        }
    }
}

void MidiRangeTuner::editorShown (Label*, TextEditor& ted)
{
    ted.setJustification (Justification::centred);
}

void MidiRangeTuner::editorHidden (Label* lbl, TextEditor&)
{
    lbl->setVisible (false);
}

void MidiRangeTuner::sliderValueChanged (Slider* sldr)
{
    if (sldr == lowSlider.get())
    {
        rangeLabelMin->setText (String (lowSlider->getValue()), dontSendNotification);
        setLabelBounds (*rangeLabelMin);

        // in case the other thumb is dragged along..
        if (highSlider->getThumbBeingDragged() == -1 && lowSlider->getValue() > highSlider->getValue())
        {
            highSlider->setValue (sldr->getValue(), dontSendNotification);
            setLabelBounds (*rangeLabelMax);
            rangeLabelMax->setText (String (highSlider->getValue()), dontSendNotification);
        }
    }

    else if (sldr == highSlider.get())
    {
        rangeLabelMax->setText (String (highSlider->getValue()), dontSendNotification);
        setLabelBounds (*rangeLabelMax);

        // in case the other thumb is dragged along..
        if (lowSlider->getThumbBeingDragged() == -1 && lowSlider->getValue() > highSlider->getValue())
        {
            lowSlider->setValue (sldr->getValue(), dontSendNotification);
            setLabelBounds (*rangeLabelMin);
            rangeLabelMin->setText (String (lowSlider->getValue()), dontSendNotification);
        }
    }
}

void MidiRangeTuner::mouseDown (const MouseEvent& e)
{
    if (e.mods.isLeftButtonDown())
    {
        objectBeingDragged = getObjectToDrag (e);

        if (e.getNumberOfClicks() == 1)
        {
            if (objectBeingDragged == lowThumb)
            {
                rangeLabelMin->setVisible (true);
                lowSlider->mouseDown (e.getEventRelativeTo (lowSlider.get()));
            }

            else if (objectBeingDragged == highThumb)
            {
                rangeLabelMax->setVisible (true);
                highSlider->mouseDown (e.getEventRelativeTo (highSlider.get()));
            }

            else if (objectBeingDragged == middleArea)
            {
                rangeLabelMin->setVisible (true);
                rangeLabelMax->setVisible (true);
                
                lowSlider->setSliderSnapsToMousePosition (false);
                highSlider->setSliderSnapsToMousePosition (false);
                
                lowSlider->mouseDown (e.getEventRelativeTo (lowSlider.get()));
                highSlider->mouseDown (e.getEventRelativeTo (highSlider.get()));
            }

            repaint();
        }
        else // double click
        {
            if (objectBeingDragged == lowThumb)
            {
                objectBeingDragged = none;
                rangeLabelMin->setVisible (true);
                rangeLabelMin->showEditor();
                repaint();
            }

            else if (objectBeingDragged == highThumb)
            {
                objectBeingDragged = none;
                rangeLabelMax->setVisible (true);
                rangeLabelMax->showEditor();
                repaint();
            }
        }
    }
}

void MidiRangeTuner::mouseDrag (const MouseEvent& e)
{
    if (!e.mods.isLeftButtonDown() || e.getNumberOfClicks() > 1)
    {
        objectBeingDragged = none;
        return;
    }

    if (objectBeingDragged == lowThumb)
    {
        lowSlider->mouseDrag (e.getEventRelativeTo (lowSlider.get()));
    }

    else if (objectBeingDragged == highThumb)
    {
        highSlider->mouseDrag (e.getEventRelativeTo (highSlider.get()));
    }

    else if (objectBeingDragged == middleArea)
    {
        lowSlider->mouseDrag (e.getEventRelativeTo (lowSlider.get()));
        highSlider->mouseDrag (e.getEventRelativeTo (highSlider.get()));
    }
}

void MidiRangeTuner::mouseUp (const MouseEvent& e)
{
    if (e.mods.isLeftButtonDown() && e.getNumberOfClicks() == 1)
    {
        if (objectBeingDragged != none)
        {
            if (objectBeingDragged == lowThumb)
            {
                lowSlider->mouseUp (e.getEventRelativeTo (lowSlider.get()));
                rangeLabelMin->setVisible (false);
            }

            else if (objectBeingDragged == highThumb)
            {
                highSlider->mouseUp (e.getEventRelativeTo (highSlider.get()));
                rangeLabelMax->setVisible (false);
            }

            else if (objectBeingDragged == middleArea)
            {
                lowSlider->mouseUp (e.getEventRelativeTo (lowSlider.get()));
                highSlider->mouseUp (e.getEventRelativeTo (highSlider.get()));

                lowSlider->setSliderSnapsToMousePosition (true);
                highSlider->setSliderSnapsToMousePosition (true);

                rangeLabelMin->setVisible (false);
                rangeLabelMax->setVisible (false);
            }

        	setRangeLow (float (lowSlider->getValue()), false);
        	setRangeHigh (float (highSlider->getValue()));

            objectBeingDragged = none;
            repaint();
        }
    }
}

void MidiRangeTuner::updateDisplay()
{
    using namespace neova_dash::gesture;

    HubConfiguration::GestureData& gestureData = hubConfig.getGestureData (id);
    const int type = gestureData.type;

    if (type == neova_dash::gesture::none || !isEnabled()) return;

    const float& value =  (type == vibrato) ? dataReader.getFloatValueReference (neova_dash::data::variance)
                         :(type == pitchBend) ? dataReader.getFloatValueReference (neova_dash::data::roll)
                         :(type == roll) ? dataReader.getFloatValueReference (neova_dash::data::roll)
                         :(type == tilt) ? dataReader.getFloatValueReference (neova_dash::data::tilt)
                         : dataReader.getFloatValueReference (neova_dash::data::tilt); // default: tilt value

    const bool valueIsInRange = !isValueOutOfGestureRange (type, value);
    
    const int rescaledMidiValue = computeMidiValue (type, value, gestureData.midiLow,
                                                                 gestureData.midiHigh,
                                                                 gestureData.reverse,
                                                                 gestureData.gestureParam0,
                                                                 gestureData.gestureParam1,
                                                                 (type != vibrato ? gestureData.gestureParam2
                                                                                  : dataReader.getFloatValueReference (neova_dash::data::acceleration)),
                                                                 gestureData.gestureParam3,
                                                                 gestureData.gestureParam4,
                                                                 gestureData.gestureParam5);

    if (valueIsInRange && rescaledMidiValue != lastValue)
    {
        lastValue = rescaledMidiValue;
        
        repaint (lowSlider->getBounds().withY (lowSlider->getBounds().getCentreY() + 5)
                                       .withHeight (8));
    }
}

void MidiRangeTuner::updateComponents()
{
    // Sets slider value
    if (lowSlider->getThumbBeingDragged() == -1)
    {
        lowSlider->setValue (double (getRangeLow()), dontSendNotification);
    }

    if (highSlider->getThumbBeingDragged() == -1)
    {
        highSlider->setValue (double (getRangeHigh()), dontSendNotification);
    }
    
    // Sets label text
    if (!(rangeLabelMin->isBeingEdited()))
    {
        rangeLabelMin->setText (String (int (getRangeLow())), dontSendNotification);
    }
      
    if (!(rangeLabelMax->isBeingEdited()))
    {
        rangeLabelMax->setText (String (int (getRangeHigh())), dontSendNotification);
    }

    //reverseButton->setToggleState (true); TO CHANGE
    updateHighlightColour();
}

void MidiRangeTuner::updateHighlightColour()
{
    highlightColour = neova_dash::gesture::getHighlightColour (hubConfig.getGestureData (id).type,
    												           hubConfig.isGestureActive (id));

    auto updateLabelColours = [this] (Label& label)
    {
        label.setColour (Label::textColourId, highlightColour);
        label.setColour (Label::textWhenEditingColourId, highlightColour);
        label.setColour (TextEditor::textColourId, highlightColour);
        label.setColour (TextEditor::highlightColourId, highlightColour.withAlpha (0.2f));
        label.setColour (TextEditor::highlightedTextColourId, highlightColour);
        label.setColour (CaretComponent::caretColourId, highlightColour.withAlpha (0.2f));
    };

    updateLabelColours (*rangeLabelMin);
    updateLabelColours (*rangeLabelMax);

    repaint();
}

void MidiRangeTuner::setRangeLow (float val, bool uploadToHub)
{
	hubConfig.setUint8Value (id, HubConfiguration::midiLow, uint8 (val), uploadToHub);
}
void MidiRangeTuner::setRangeHigh (float val, bool uploadToHub)
{
	hubConfig.setUint8Value (id, HubConfiguration::midiHigh, uint8 (val), uploadToHub);
}

float MidiRangeTuner::getRangeLow()
{
    return float (hubConfig.getGestureData (id).midiLow);
}
float MidiRangeTuner::getRangeHigh()
{
    return float (hubConfig.getGestureData (id).midiHigh);
}

void MidiRangeTuner::createLabels()
{
    addAndMakeVisible (*(rangeLabelMin = std::make_unique<Label> ("Min Label", String (getRangeLow()))));
    addAndMakeVisible (*(rangeLabelMax = std::make_unique<Label> ("Max Label", String (getRangeHigh()))));
    
    // Label style

    auto setLabelAttributes = [this] (Label& label)
    {
        label.setEditable (true, false, false);
        label.setSize (30, 30);
        label.setFont (neova_dash::font::dashFont.withHeight (11.0f));
        label.setColour (Label::textColourId, highlightColour);
        label.setColour (Label::backgroundColourId, Colour (0x00000000));
        label.setColour (Label::textWhenEditingColourId, highlightColour);
        label.setColour (TextEditor::textColourId, highlightColour);
        label.setColour (TextEditor::highlightColourId, highlightColour.withAlpha (0.2f));
        label.setColour (TextEditor::highlightedTextColourId, highlightColour);
        label.setColour (CaretComponent::caretColourId, highlightColour.withAlpha (0.2f));
        label.setJustificationType (Justification::centred);
        label.setVisible (false);
        label.addListener (this);
    };

    setLabelAttributes (*rangeLabelMin);
    setLabelAttributes (*rangeLabelMax);
}

void MidiRangeTuner::setLabelBounds (Label& labelToResize)
{
    if (&labelToResize == rangeLabelMin.get())
    {
        rangeLabelMin->setCentrePosition (jmin (jmax ((int) getThumbX (lowThumb), rangeLabelMin->getWidth()/2),
                                                getWidth() - rangeLabelMin->getWidth()/2),
                                          lowSlider->getBounds().getCentreY() - 16);
    }
    else if (&labelToResize == rangeLabelMax.get())
    {
        rangeLabelMax->setCentrePosition (jmin (jmax ((int) getThumbX (highThumb), rangeLabelMax->getWidth()/2),
                                                getWidth() - rangeLabelMax->getWidth()/2),
                                          highSlider->getBounds().getCentreY() - 16);
    }
}

void MidiRangeTuner::createSliders()
{
    addAndMakeVisible (*(lowSlider = std::make_unique<Slider> ("Range Low Slider")));
    addAndMakeVisible (*(highSlider = std::make_unique<Slider> ("Range High Slider")));
    
    auto setSliderSettings = [this] (Slider& slider, float valueToSet)
    {
        slider.setSliderStyle (Slider::LinearHorizontal);
        slider.setTextBoxStyle (Slider::NoTextBox, false, 0, 0);
        slider.setColour (Slider::backgroundColourId, Colour (0x00000000));
        slider.setColour (Slider::trackColourId, Colour (0x00000000));
        slider.setRange (0.0, 127.0, 1.0);
        slider.setValue (valueToSet);
        slider.setInterceptsMouseClicks (false, false);
        slider.addListener (this);
    };

    setSliderSettings (*lowSlider, getRangeLow());
    setSliderSettings (*highSlider, getRangeHigh());
}

float MidiRangeTuner::getThumbX (DraggableObject thumb)
{
    if (thumb == lowThumb)
    {
        return static_cast<float>(lowSlider->getX() + 11.5f + (lowSlider->getWidth() - 23.0f) * lowSlider->getValue()/127.0f);
    }

    if (thumb == highThumb)
    {
        return static_cast<float>(highSlider->getX() + 11.5f + (highSlider->getWidth() - 23.0f) * highSlider->getValue()/127.0f);
    }

    return -1.0f;
}


void MidiRangeTuner::handleSliderClick (const MouseEvent&)
{

}

MidiRangeTuner::DraggableObject MidiRangeTuner::getObjectToDrag (const MouseEvent& e)
{
    if (e.x < 0 || e.x > highSlider->getWidth())
        return none;

    if (e.mods.isShiftDown())
        return middleArea;

    const float tolerance = (getThumbX (highThumb) - getThumbX (lowThumb))/5.0f;

    if ((float) e.x <= getThumbX (lowThumb) + tolerance)
        return lowThumb;
    
    if ((float) e.x < getThumbX (highThumb) - tolerance)
        return middleArea;

    return highThumb;
}

void MidiRangeTuner::drawCursor (Graphics& g)
{
    if (lastValue == -1) return;

    float maxMidiFloat = 127.0f;

    Path cursorPath;
    float cursorX = 11.5f + (lowSlider->getWidth() - 23.0f) * (lastValue / maxMidiFloat);

    juce::Point<float> cursorPoint = {cursorX, lowSlider->getBounds().getCentreY() + 9.0f};

    cursorPath.addTriangle ({cursorPoint.x - 3.0f, cursorPoint.y + 3.0f},
                            {cursorPoint.x + 3.0f, cursorPoint.y + 3.0f},
                            {cursorPoint.x       , cursorPoint.y - 3.0f});

    g.setColour (highlightColour);
    g.fillPath (cursorPath);
}

void MidiRangeTuner::drawSliderBackground (Graphics& g)
{
    g.setColour (neova_dash::colour::tunerSliderBackground);
    g.fillRoundedRectangle (lowSlider->getBounds().withSizeKeepingCentre (lowSlider->getWidth() - 10, 12)
                                                  .toFloat(),
                            3.0f);

    juce::Point<float> startPoint (getThumbX (lowThumb), lowSlider->getY() + lowSlider->getHeight() * 0.5f);

    juce::Point<float> endPoint (getThumbX (highThumb), highSlider->getY() + highSlider->getHeight() * 0.5f);

    Path valueTrack;
    valueTrack.startNewSubPath (startPoint);
    valueTrack.lineTo (endPoint);

    g.setColour (objectBeingDragged == middleArea ? highlightColour.interpolatedWith (Colour (0xffffffff),
                                                                                      0.5f)
                                                  : highlightColour);
    g.strokePath(valueTrack, { 6.0f //Old: trackWidth
                               , PathStrokeType::curved, PathStrokeType::rounded });

    if (objectBeingDragged == lowThumb || objectBeingDragged == highThumb)
    {
        g.setColour (highlightColour.withAlpha (0.6f));
        g.fillEllipse(juce::Rectangle<float> (25.0f, 25.0f)
                          .withCentre (objectBeingDragged == lowThumb ? startPoint
                                                                      : endPoint));
    }
}