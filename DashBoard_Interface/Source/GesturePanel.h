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
#include "HubConfiguration.h"

//==============================================================================
/*
*/
class GesturePanel    : public Component
{
public:
    //==============================================================================
    GesturePanel (HubConfiguration& data);
    ~GesturePanel();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

private:
    //==============================================================================
    HubConfiguration& hubConfig;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GesturePanel)
};
