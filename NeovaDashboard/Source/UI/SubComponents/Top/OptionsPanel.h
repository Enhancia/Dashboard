/*
  ==============================================================================

    OptionsPanel.h
    Created: 18 Sep 2019 1:59:02pm
    Author:  Enhancia Dev

  ==============================================================================
*/

#pragma once

#include "../../../../JuceLibraryCode/JuceHeader.h"
#include "../../../Common/DashCommon.h"
#include "../../../Common/HubConfiguration.h"
#include "../DashShapeButton.h"

#if JUCE_WINDOWS
#include <windows.h>
#include <ShellAPI.h>
#elif JUCE_MAC
#include <stdlib.h>
#endif

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
    void createButtons();
    void paintProductInformations (Graphics& g, juce::Rectangle<int> area);
    void paintFirmUpdateArea (Graphics& g, juce::Rectangle<int> area);

    //==============================================================================
    juce::Rectangle<int> optionsArea;
    std::unique_ptr<DashShapeButton> closeButton;
    
    //==============================================================================
    HubConfiguration& hubConfig;
    ApplicationCommandManager& commandManager;
    std::unique_ptr<TextButton> updateFirmwareButton;
    std::unique_ptr<TextButton> sendReportButton;
    std::unique_ptr<TextButton> contactButton;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OptionsPanel)
};