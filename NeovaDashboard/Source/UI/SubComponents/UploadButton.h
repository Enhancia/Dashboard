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
    UploadButton (HubConfiguration& config, ApplicationCommandManager& cm, const bool shouldBeActive = false);
    ~UploadButton();

    //==============================================================================
    void resized() override;

    //==============================================================================
    void timerCallback() override;

    //==============================================================================
    void setActive (const bool shouldBeActive = true);
    void setInactiveAndShowUploadFeedback();
    bool isActive();

private:
    //==============================================================================
	void paintButton (Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;
	void clicked() override;

    //==============================================================================
    bool active;
    bool animating = false;

	//==============================================================================
    ApplicationCommandManager& commandManager;
    HubConfiguration& hubConfig;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (UploadButton)
};