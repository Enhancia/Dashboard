/*
  ==============================================================================

    MidiPanel.h
    Created: 12 Sep 2019 11:43:05am
    Author:  Enhancia Dev

  ==============================================================================
*/

#pragma once

#include "../../../../JuceLibraryCode/JuceHeader.h"
#include "../../../Common/DashCommon.h"
#include "../../../DataReader/DataReader.h"
#include "../../../Common/HubConfiguration.h"
#include "../../LookAndFeel/DashBoardLookAndFeel.h"

//==============================================================================
/*
*/
class MidiRangeTuner : public Component,
                       private Label::Listener,
                       private Slider::Listener
{
public:
    //==============================================================================
    enum DraggableObject
    {
        none = 0,

        lowThumb,
        highThumb,
        middleArea
    };

    //==============================================================================
    MidiRangeTuner (HubConfiguration& config, DataReader& reader, const int gestId);
    ~MidiRangeTuner();

    //==============================================================================
    void paint (Graphics& g) override;
    void resized() override;

    //==============================================================================
    void labelTextChanged (Label* lbl) override;
    void editorShown (Label* lbl, TextEditor& ted) override;
    void editorHidden (Label* lbl, TextEditor& ted) override;
    void sliderValueChanged (Slider* sldr) override;

    //==============================================================================
    void mouseDown (const MouseEvent& e) override;
    void mouseDrag (const MouseEvent& e) override;
    void mouseUp (const MouseEvent& e) override;

    //==============================================================================
    void updateDisplay();
    void updateComponents();
    void updateHighlightColour();

    //==============================================================================
    void setRangeLow (float value, bool uploadToHub = true);
    void setRangeHigh (float value, bool uploadToHub = true);
    
    float getRangeLow();
    float getRangeHigh();

    //==============================================================================
    const int id;

private:
    //==============================================================================
    void createLabels();
    void createSliders();

    //==============================================================================
    float getThumbX(DraggableObject thumb);
    void setLabelBounds(Label& labelToResize);

    void handleSliderClick(const MouseEvent& e);
    DraggableObject getObjectToDrag(const MouseEvent& e);

    //==============================================================================
    void drawCursor(Graphics& g);
    void drawSliderBackground(Graphics& g);

    //==============================================================================
    HubConfiguration& hubConfig;
    DataReader& dataReader;
    int lastValue = -1;

    //==============================================================================
    ScopedPointer<Label> rangeLabelMin;
    ScopedPointer<Label> rangeLabelMax;
    ScopedPointer<Slider> lowSlider;
    ScopedPointer<Slider> highSlider;

    DraggableObject objectBeingDragged = none;
    Colour highlightColour;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MidiRangeTuner)
};


//==================================================================================

class MidiPanel    : public Component,
                     private Label::Listener,
                     private ComboBox::Listener,
                     private Button::Listener
{
public:
    MidiPanel (HubConfiguration& config, DataReader& reader, const int gestId);
    ~MidiPanel();

    //==============================================================================
    void paint (Graphics& g) override;
    void resized() override;

    //==============================================================================
    void labelTextChanged (Label* lbl) override;
    void editorShown (Label* lbl, TextEditor& ted) override;
    void comboBoxChanged (ComboBox* box) override;
    void buttonClicked (Button* bttn) override;
    
    //==============================================================================
    void updateComponents();
    void updateDisplay();

    MidiRangeTuner& getTuner();

    //==============================================================================
    const int id;

private:
    //==============================================================================
    void createComboBox();
    void createLabels();
    void createButton();
    void setComponentsVisibility();
    
    //==============================================================================
    ScopedPointer<ComboBox> midiTypeBox;
    ScopedPointer<Label> ccLabel;
    ScopedPointer<Label> rangeLabelMin;
    ScopedPointer<Label> rangeLabelMax;
    ScopedPointer<MidiRangeTuner> midiRangeTuner;
    std::unique_ptr<TextButton> reverseButton;

    //==============================================================================
    HubConfiguration& hubConfig;
    DataReader& dataReader;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MidiPanel)
};