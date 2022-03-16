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
    MidiRangeTuner (HubConfiguration& config, DataReader& reader, int gestId);
    ~MidiRangeTuner() override;

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
    void setRangeLow (float value, bool uploadToHub = true) const;
    void setRangeHigh (float value, bool uploadToHub = true) const;
    
    float getRangeLow() const;
    float getRangeHigh() const;

    //==============================================================================
    const int id;

private:
    //==============================================================================
    void createLabels();
    void createSliders();

    //==============================================================================
    float getThumbX(DraggableObject thumb) const;
    void setLabelBounds(Label& labelToResize) const;

    void handleSliderClick(const MouseEvent& e);
    DraggableObject getObjectToDrag(const MouseEvent& e) const;

    //==============================================================================
    void drawCursor(Graphics& g) const;
    void drawSliderBackground(Graphics& g) const;

    //==============================================================================
    HubConfiguration& hubConfig;
    DataReader& dataReader;
    int lastValue = -1;

    //==============================================================================
    std::unique_ptr<Label> rangeLabelMin;
    std::unique_ptr<Label> rangeLabelMax;
    std::unique_ptr<Slider> lowSlider;
    std::unique_ptr<Slider> highSlider;

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
    MidiPanel (HubConfiguration& config, DataReader& reader, int gestId);
    ~MidiPanel() override;

    //==============================================================================
    void paint (Graphics& g) override;
    void resized() override;

    //==============================================================================
    void labelTextChanged (Label* lbl) override;
    void editorShown (Label* lbl, TextEditor& ted) override;
    void comboBoxChanged (ComboBox* box) override;
    void buttonClicked (Button* bttn) override;
    
    //==============================================================================
    void updateComponents() const;
    void updateDisplay() const;

    MidiRangeTuner& getTuner() const;

    //==============================================================================
    const int id;

private:
    //==============================================================================
    void createComboBox();
    void createLabels();
    void createButton();
    void setComponentsVisibility() const;
    
    //==============================================================================
    std::unique_ptr<ComboBox> midiTypeBox;
    std::unique_ptr<Label> ccLabel;
    std::unique_ptr<MidiRangeTuner> midiRangeTuner;
    std::unique_ptr<TextButton> reverseButton;

    //==============================================================================
    HubConfiguration& hubConfig;
    DataReader& dataReader;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MidiPanel)
};