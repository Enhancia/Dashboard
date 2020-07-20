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
			class TabButton : public Button
			{
			public:
				TabButton(String tabName) : Button(tabName + String("Button")) {}
				~TabButton() {}

				void paintButton(Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override
				{
					if (shouldDrawButtonAsDown || shouldDrawButtonAsHighlighted)
					{
						g.setColour(neova_dash::colour::subText.withAlpha(shouldDrawButtonAsDown ? 0.2f : 0.05f));
						g.fillRect(getLocalBounds());
					}
				}

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TabButton)
			};

            Tab (Component* panelToUse, String tabName) : name (tabName), panel (panelToUse)
            {
                button.reset (new TabButton (name));
            }

            ~Tab ()
            {
                button = nullptr;
            }

            std::unique_ptr<TabButton> button;
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
    //Image enhanciaLogo = ImageFileFormat::loadFrom (DashData::logoenhanciawhitetextured_png, DashData::logoenhanciawhitetextured_pngSize);
    
    //==============================================================================
    HubConfiguration& hubConfig;
    ApplicationCommandManager& commandManager;
    std::unique_ptr<TextButton> sendReportButton;
    std::unique_ptr<TextButton> contactButton;
    std::unique_ptr<TextButton> upgradeButton;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OptionsPanel)
};

class ContactPanel: public Component, Button::Listener
{
public:
    //==============================================================================
    ContactPanel();
    ~ContactPanel();

    //==============================================================================
    void paint (Graphics& g) override;
    void resized() override;

    //==============================================================================
    void buttonClicked (Button* bttn) override;

private:
    //==============================================================================
    std::unique_ptr<TextButton> contactButton;
    std::unique_ptr<TextButton> sendReportButton;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ContactPanel)
};

class LegalPanel: public Component
{
public:
    //==============================================================================
    LegalPanel();
    ~LegalPanel();

    //==============================================================================
    void paint (Graphics& g) override;
    void resized() override;
private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LegalPanel)
};

class FirmwarePanel: public Component
{
public:
    //==============================================================================
    FirmwarePanel (HubConfiguration& hubConfiguration, TextButton& button);
    ~FirmwarePanel();

    //==============================================================================
    void paint (Graphics& g) override;
    void resized() override;
private:
    //==============================================================================
    HubConfiguration& hubConfig;
    TextButton& upgradeButton;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FirmwarePanel)
};

class LicensePanel: public Component
{
public:
    //==============================================================================
    LicensePanel();
    ~LicensePanel();

    //==============================================================================
    void paint (Graphics& g) override;
    void resized() override;
private:
    //==============================================================================
    TextEditor licenseTextEdit;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LicensePanel)
};