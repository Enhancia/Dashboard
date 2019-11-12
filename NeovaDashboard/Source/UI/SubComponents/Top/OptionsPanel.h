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
                        private Button::Listener,
                        private ComboBox::Listener
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
    void comboBoxChanged (ComboBox* box) override;
    void mouseUp (const MouseEvent& event) override;
    void visibilityChanged() override;

    //==============================================================================
    void update();
    void setMidiBoxActive (bool shouldBeActive);

private:
    //==============================================================================
    void createButtons();
    void createMidiBox();
    void paintProductInformations (Graphics& g, juce::Rectangle<int> area);
    void paintFirmUpdateArea (Graphics& g, juce::Rectangle<int> area);

    //==============================================================================
    juce::Rectangle<int> optionsArea;
    std::unique_ptr<DashShapeButton> closeButton;
    std::unique_ptr<ComboBox> midiChannelBox;
    
    //==============================================================================
    HubConfiguration& hubConfig;
    ApplicationCommandManager& commandManager;
    std::unique_ptr<TextButton> updateFirmwareButton;
    std::unique_ptr<TextButton> sendReportButton;
    std::unique_ptr<TextButton> contactButton;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OptionsPanel)
};