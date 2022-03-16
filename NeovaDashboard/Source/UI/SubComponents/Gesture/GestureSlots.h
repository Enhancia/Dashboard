/*
  ==============================================================================

    GestureSlots.h
    Created: 6 Sep 2019 8:05:19pm
    Author:  aleva

  ==============================================================================
*/

#pragma once

#include "../../../../JuceLibraryCode/JuceHeader.h"
#include "../../../Common/DashCommon.h"
#include "../../../Common/HubConfiguration.h"
#include "../DashShapeButton.h"

class GestureComponent : public Component
{
public:
    //==============================================================================
    GestureComponent (HubConfiguration& hubCfg, ApplicationCommandManager& manager,
                      int gestNum,
                                                const bool& dragModeReference,
                                                const int& draggedGestureReference,
                                                const int& draggedOverSlotReference);
    ~GestureComponent() override;

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

    //==============================================================================
    void mouseEnter (const MouseEvent &event) override;
    void mouseExit (const MouseEvent &event) override;
    void mouseDrag (const MouseEvent &event) override;

    //==============================================================================
    bool isSelected() const;
    void setSelected (bool);
    void setHighlighted (bool);
    void setSolo (bool);

    //==============================================================================
    const int id;
    const int type;

private:
    //==============================================================================
    void createButton();
    void drawGesturePath (Graphics& g, juce::Rectangle<int> area) const;

    //==============================================================================
    HubConfiguration& hubConfig;
    ApplicationCommandManager& commandManager;

    std::unique_ptr<DashShapeButton> muteButton;

    /*
    Image vibratoImage = ImageFileFormat::loadFrom (DashData::VIBRATOicon_png,
                                                    DashData::VIBRATOicon_pngSize);
    Image pitchBendImage = ImageFileFormat::loadFrom (DashData::PITCHicon_png,
                                                      DashData::PITCHicon_pngSize);
    Image tiltImage = ImageFileFormat::loadFrom (DashData::TILTicon_png,
                                                 DashData::TILTicon_pngSize);
    Image rollImage = ImageFileFormat::loadFrom (DashData::ROLLicon_png,
                                                 DashData::ROLLicon_pngSize);*/

    //==============================================================================
    bool on = static_cast<bool>(hubConfig.getGestureData(id).on);
    bool selected = false, highlighted = false, solo = false;

    //==============================================================================
    const bool& dragMode;
    const int& draggedGesture;
    const int& draggedOverSlot;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GestureComponent)
};


class EmptyGestureSlotComponent : public Component
{
public:
    //==============================================================================
    EmptyGestureSlotComponent (HubConfiguration& hubCfg, int slotId,
                                                         const bool& dragModeReference,
                                                         const int& draggedGestureReference,
                                                         const int& draggedOverSlotReference);
    ~EmptyGestureSlotComponent() override;

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

    //==============================================================================
    void mouseEnter (const MouseEvent &event) override;
    void mouseExit (const MouseEvent &event) override;
    void mouseDrag (const MouseEvent &event) override;

    const int id;

private:
    //==============================================================================
    bool highlighted = false;
    
    //==============================================================================
    HubConfiguration& hubConfig;
    const bool& dragMode;
    const int& draggedGesture;
    const int& draggedOverSlot;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EmptyGestureSlotComponent)
};