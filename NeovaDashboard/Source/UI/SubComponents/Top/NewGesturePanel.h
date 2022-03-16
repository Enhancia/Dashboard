/*
  ==============================================================================

    NewGesturePanel.h
    Created: 9 Sep 2019 11:47:26am
    Author:  Enhancia Dev

  ==============================================================================
*/

#pragma once


#include "../../../../JuceLibraryCode/JuceHeader.h"
#include "../../../Common/DashCommon.h"
#include "../../../Common/HubConfiguration.h"
#include "../DashShapeButton.h"

//==============================================================================
/*
*/
class NewGesturePanel    : public Component,
                           private Label::Listener,
                           private Button::Listener
{
public:
    //==============================================================================
    explicit NewGesturePanel (HubConfiguration& config, ApplicationCommandManager& commandManager);
    ~NewGesturePanel() override;

    //==============================================================================
    // Component Methods
    void paint (Graphics&) override;
    void resized() override;
    
    //==============================================================================
    // Callbacks
    void visibilityChanged() override;
    void mouseEnter (const MouseEvent &event) override;
    void mouseUp (const MouseEvent &event) override;
    void buttonClicked (Button* bttn) override;
    bool keyPressed (const KeyPress& key) override;
    void labelTextChanged (Label* lbl) override;
    void editorShown (Label* lbl, TextEditor& ed) override;

    //==============================================================================
    void createNewGesture() const;
    void showPanelForGestureID (int gestureID);
    void hidePanel (bool resetSelectedSlot = false);
    void updateInterface() const;
    const int getLastSelectedSlot() const;

private:
    class GestureTypeSelector : public Component
    {
    public:
        GestureTypeSelector (int gestType);
        ~GestureTypeSelector() override;

        void paint (Graphics&) override;
        void resized() override;

        void mouseEnter (const MouseEvent &event) override;
        void mouseExit (const MouseEvent &event) override;

        void setHighlighted (bool shouldBeHighlighted);

        const int gestureType; // from Gesture::GestureType

    private:
        //==============================================================================
        bool highlighted = false;
        void drawGesturePath (Graphics& g, Rectangle<int> area) const;

        //==============================================================================
        /*Image vibratoImage = ImageFileFormat::loadFrom (DashData::VIBRATOicon_png,
                                                        DashData::VIBRATOicon_pngSize);
        Image pitchBendImage = ImageFileFormat::loadFrom (DashData::PITCHicon_png,
                                                          DashData::PITCHicon_pngSize);
        Image tiltImage = ImageFileFormat::loadFrom (DashData::TILTicon_png,
                                                     DashData::TILTicon_pngSize);
        Image rollImage = ImageFileFormat::loadFrom (DashData::ROLLicon_png,
                                                     DashData::ROLLicon_pngSize);*/

        //==============================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GestureTypeSelector)
    };

    void selectGestureType (const GestureTypeSelector* gestureTypeToSelect);
    void unselectGestureType();

    //==============================================================================
    // Overriden to prevent public use!
    // Use showPanelForGestureID (int) or hidePanel (bool) instead.
    void setVisible (bool) override; 

    //==============================================================================
    void createCloseButton();
    void createAndAddTextEditor();
    void createGestureSelectorButtons();
    void resizeGestureSelectorButtons (Rectangle<int> buttonsArea) const;

    //==============================================================================
    Rectangle<int> panelArea {0, 0, getWidth(), getHeight()};
    int selectedGestureType = -1;
    int selectedGestureSlot = -1;

    //==============================================================================
    HubConfiguration& hubConfig;
    ApplicationCommandManager& commandManager;
    
    //==============================================================================
    std::unique_ptr<DashShapeButton> closeButton;
    std::unique_ptr<TextEditor> descriptionTextEditor;
    OwnedArray<GestureTypeSelector> gestureSelectors;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NewGesturePanel)
};