/*
  ==============================================================================

    GesturePanel.h
    Created: 3 Sep 2019 11:38:56am
    Author:  Enhancia Dev

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "DashCommon.h"

//==============================================================================
/*
*/
class GesturePanel    : public Component
{
public:
    //==============================================================================
    GesturePanel();
    ~GesturePanel();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GesturePanel)
};
