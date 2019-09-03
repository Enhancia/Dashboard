/*
  ==============================================================================

    PresetSelectorComponent.h
    Created: 3 Sep 2019 11:37:45am
    Author:  Enhancia Dev

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "DashCommon.h"

//==============================================================================
/*
*/
class PresetSelectorComponent    : public Component
{
public:
    //==============================================================================
    PresetSelectorComponent();
    ~PresetSelectorComponent();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PresetSelectorComponent)
};
