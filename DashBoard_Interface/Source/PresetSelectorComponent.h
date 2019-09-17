/*
  ==============================================================================

    PresetSelectorComponent.h
    Created: 17 Sep 2019 3:57:45pm
    Author:  Enhancia Dev

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/*
*/
class PresetSelectorComponent    : public Component
{
public:
    //==============================================================================
    PresetSelectorComponent (HubConfiguration& config);
    ~PresetSelectorComponent();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

    //==============================================================================
    void mouseEnter (const MouseEvent &event) override;
    void mouseExit (const MouseEvent &event) override;
    void mouseUp (const MouseEvent &event) override;

private:
    //==============================================================================
    HubConfiguration& hubConfig;
    Path arrowLeft, arrowRight;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PresetSelectorComponent)
};
