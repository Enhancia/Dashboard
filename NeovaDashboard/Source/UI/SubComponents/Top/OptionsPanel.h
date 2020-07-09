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

    //==============================================================================
    void update();

private:
    class TabbedOptions    : public Component,
                             private Button::Listener
    {
    public:
        enum TabbedPanelStyle
        {
            tabsVertical =0,
            tabsHorizontal
        };

        //==============================================================================
        TabbedOptions();
        ~TabbedOptions();

        //==============================================================================
        void addTab (Component* panel, String tabName);
        void switchToTab (const int tabNumber);
        Component* getComponentFromTab (const int tabNumber);
        Component* getComponentFromTab (const String tabName);
        Component* getComponentFromSelectedTab();

        //==============================================================================
        void setStyle (TabbedPanelStyle newStyle);

        //==============================================================================
        void paint (Graphics&) override;
        void resized() override;

        //==============================================================================
        void buttonClicked (Button* bttn) override;

    private:
        //==============================================================================
        struct Tab
        {
            Tab (Component* panelToUse, String tabName) : name (tabName), panel (panelToUse)
            {
                button.reset (new TextButton (name));
                button->setButtonText ("");
                button->setColour (TextButton::buttonColourId, Colour (0x00000000));
                button->setColour (TextButton::buttonOnColourId, Colour (0x00000000));
            }

            ~Tab ()
            {
                button = nullptr;
            }

            std::unique_ptr<TextButton> button;
            std::unique_ptr<Component> panel;
            const String name;
        };

        //==============================================================================
        juce::Rectangle<int> panelArea, tabsArea;
        OwnedArray<Tab> tabs;
        TabbedPanelStyle style = tabsVertical;
        int selectedTab = 0;

        //==============================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TabbedOptions)
    };

    //==============================================================================
    void createButtons();
    void paintProductInformations (Graphics& g, juce::Rectangle<int> area);
    void paintFirmUpdateArea (Graphics& g, juce::Rectangle<int> area);
    void paintLegalAndRegulatoryArea (Graphics& g, juce::Rectangle<int> area);

    //==============================================================================
    juce::Rectangle<int> optionsArea;
    std::unique_ptr<DashShapeButton> closeButton;
    TabbedOptions options;
    
    //==============================================================================
    HubConfiguration& hubConfig;
    ApplicationCommandManager& commandManager;
    std::unique_ptr<TextButton> upgradeButton;
    std::unique_ptr<TextButton> sendReportButton;
    std::unique_ptr<TextButton> contactButton;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OptionsPanel)
};

class LegalPanel: public Component
{
public:
    LegalPanel();
    ~LegalPanel();

    void paint (Graphics& g) override;
    void resized() override;
private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LegalPanel)
};