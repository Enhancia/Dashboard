/*
  ==============================================================================

    UpdaterPanel.h
    Created: 12 Mar 2020 4:36:00pm
    Author:  aleva

  ==============================================================================
*/

#pragma once

#include "../../../../JuceLibraryCode/JuceHeader.h"
#include "../../../Common/DashCommon.h"
#include "../DashShapeButton.h"
#include "../../../DashUpdater/DashUpdater.h"

class UpdaterPanel    : public Component,
						public Timer,
                        private Button::Listener
{
public:
    //==============================================================================
	enum downloadProgress
	{
		downloadAvailable = 0,
		inProgress,
		downloadFinished,
        updateRequired
	};

    //==============================================================================
    UpdaterPanel (DashUpdater& updtr, float& updateProgress);
    ~UpdaterPanel();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

    //==============================================================================
    void timerCallback() override;

    //==============================================================================
    void buttonClicked (Button* bttn) override;

    //==============================================================================
    void resetAndOpenPanel (bool updateIsRequired = false);
    void closeAndResetPanel();
    
private:
    //==============================================================================
    void createLabels();
    void createButtons();
	void updateComponentsForSpecificStep (downloadProgress downloadStep);

    //==============================================================================
    juce::Rectangle<int> panelArea;
    std::unique_ptr<Label> bodyText;
    std::unique_ptr<Label> titleLabel;
    std::unique_ptr<DashShapeButton> closeButton;
    std::unique_ptr<TextButton> bottomButton;

    //==============================================================================
    downloadProgress currentProgress = downloadAvailable;

    //==============================================================================
    DashUpdater& updater;
    float& progress;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (UpdaterPanel)
};