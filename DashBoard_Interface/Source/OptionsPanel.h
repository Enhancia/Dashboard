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
    void createButtons();
    void paintProductInformations (Graphics& g, Rectangle<int> area);
    void paintFirmUpdateArea (Graphics& g, Rectangle<int> area);

    //==============================================================================
    juce::Rectangle<int> optionsArea;
    std::unique_ptr<DashShapeButton> closeButton;
    Image logoImage = ImageFileFormat::loadFrom (DashData::Logo_ENHANCIA_Round_png,
                                                 DashData::Logo_ENHANCIA_Round_pngSize);
    
    //==============================================================================
    HubConfiguration& hubConfig;
    ApplicationCommandManager& commandManager;
    std::unique_ptr<TextButton> updateFirmwareButton;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OptionsPanel)
};