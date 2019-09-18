/*
  ==============================================================================

    OptionsPanel.h
    Created: 18 Sep 2019 1:59:02pm
    Author:  Enhancia Dev

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "DashCommon.h"
#include "DashShapeButton.h"

#include "HubConfiguration.h"

class OptionsPanel    : public Component,
                        private Button::Listener
{
public:
    //==============================================================================
    explicit OptionsPanel (HubConfiguration& config, ApplicationCommandManager& manager);
    ~OptionsPanel();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;
    
    //==============================================================================
    void buttonClicked (Button* bttn) override;
    void mouseUp (const MouseEvent& event) override;
    void visibilityChanged() override;

private:
    //==============================================================================
    void createButton();

    //==============================================================================
    juce::Rectangle<int> optionsArea;
    std::unique_ptr<DashShapeButton> closeButton;
    
    HubConfiguration& hubConfig;
    ApplicationCommandManager& commandManager;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OptionsPanel)
};