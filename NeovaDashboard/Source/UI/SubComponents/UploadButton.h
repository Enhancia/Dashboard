/*
  ==============================================================================

    UploadButton.h
    Created: 3 Sep 2019 5:11:34pm
    Author:  Enhancia Dev

  ==============================================================================
*/

#pragma once

#include "../../../JuceLibraryCode/JuceHeader.h"
#include "../../Common/DashCommon.h"
#include "../../Common/HubConfiguration.h"

//==============================================================================
/*
*/
class UploadButton : public Button,
                     public Timer
{
public:
    //==============================================================================
    UploadButton (HubConfiguration& config, ApplicationCommandManager& cm);
    ~UploadButton();

    //==============================================================================
    void resized() override;

    //==============================================================================
    void timerCallback() override;

    //==============================================================================
    void setActive();
    void setInactiveAndShowUploadFeedback();

private:
    //==============================================================================
	void paintButton (Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;
	void clicked() override;

    //==============================================================================
    bool active = false;
    bool animating = false;
    void setActive (const bool shouldBeActive);

	//==============================================================================
    ApplicationCommandManager& commandManager;
    HubConfiguration& hubConfig;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (UploadButton)
};