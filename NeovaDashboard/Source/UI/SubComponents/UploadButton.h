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
/**
    \class UploadButton UploadButton.h

    \brief Button used to upload current config to a connected HUB. 
*/
class UploadButton : public Button,
                     public Timer
{
public:
    //==============================================================================
    /**
        \brief Constructor.

        Creates the button with its default attributes' values.
    */
    UploadButton (HubConfiguration& config, ApplicationCommandManager& cm, bool shouldBeActive = false);

    /**
        \brief Destructor.
    */
    ~UploadButton() override;

    //==============================================================================
    void resized() override;

    //==============================================================================
    void timerCallback() override;

    //==============================================================================
    /**
        \brief Setter for the active attribute.

        Sets the button "active" attribute value, according to the shouldBeActive parameter.

        \param shouldBeActive Boolean to choose whether the button should be active or not.
    */
    void setActive (bool shouldBeActive = true);

    /**
        \brief Setter that triggers a visual feedback.

        Sets to button to inactive, and launches the visual feedback telling the user that the HUB was succesfully uploaded.
    */
    void setInactiveAndShowUploadFeedback();

    /**
        \brief Getter for the active attribute.
    */
    bool isActive() const;
    
    void update();

private:
    //==============================================================================
	void paintButton (Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;
	void clicked() override;

    //==============================================================================
    /**
        \brief Starts the visual feedback upon uploading to the HUB.

        Starts the animating progress: sets animating to true which will start incrementing the counter and draw the upload feedback.
    */
    void startAnimating();

    /**
        \brief Stops the visual feedback upon uploading to the HUB.

        Resets the animating boolean back to false, and resets the animation counter.
    */
    void stopAnimating();

    /**
        \brief Helper paint method for the upload feedback.

        Tracks the animating counter to draw the "Uploaded!" text accordingly, and increments it until reaching ANIM_MAX value.
    */
    void drawUploadFeedback (Graphics& g, juce::Rectangle<int> area) const;


    //==============================================================================
    bool active; /**< \brief Boolean that tracks the button being active or not.  */
    bool animating = false; /**< \brief Boolean that tracks the animation being occuring or not.  */
    int animationCounter = 0; /**< \brief Int that tracks the animation progress.  */
    const int ANIM_MAX = 25; /**< \brief Const int that tells the max number of frames of animating. */

	//==============================================================================
    ApplicationCommandManager& commandManager; /**< Reference to the command manager, to call front and back commands. */
    HubConfiguration& hubConfig; /**< Reference to HubConfiguration object to call or monitor internal values. */
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (UploadButton)
};
