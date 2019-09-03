/*
  ==============================================================================

    UploadButton.h
    Created: 3 Sep 2019 5:11:34pm
    Author:  Enhancia Dev

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "DashCommon.h"

//==============================================================================
/*
*/
class UploadButton : public Component
{
public:
    //==============================================================================
    UploadButton();
    ~UploadButton();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (UploadButton)
};