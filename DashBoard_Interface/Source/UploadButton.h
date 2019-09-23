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
class UploadButton : public Button
{
public:
    //==============================================================================
    UploadButton (ApplicationCommandManager& cm);
    ~UploadButton();

    //==============================================================================
    void resized() override;

private:
    //==============================================================================
	void paintButton (Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;
	void clicked() override;

	//==============================================================================
    ApplicationCommandManager& commandManager;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (UploadButton)
};