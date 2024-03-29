/*
  ==============================================================================

    HeaderComponent.h
    Created: 3 Sep 2019 11:36:30am
    Author:  Enhancia Dev

  ==============================================================================
*/

#pragma once

#include "../../../JuceLibraryCode/JuceHeader.h"
#include "../../Common/DashCommon.h"
#include "../../Common/HubConfiguration.h"
#include "../../DataReader/DataReader.h"
#include "Top/OptionsPanel.h"

//==============================================================================
/*
*/

class HeaderComponent    : public Component,
						   private Button::Listener

{
public:
    //==============================================================================
    HeaderComponent (OptionsPanel& options, HubConfiguration& config, DataReader& reader);
    ~HeaderComponent();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

    //==============================================================================
    void buttonClicked (Button* bttn) override;

    //==============================================================================
    void update();
    void setBatteryVisible (bool shouldBeVisible);

private:
	class BatteryComponent : public Component,
                             public MultiTimer
	{
	public:
        //==========================================================================
        enum timerIds
        {
            batteryCheckTimer = 0,
            blinkTimer = 1
        };

		//==========================================================================
		BatteryComponent (const float& batteryValRef, HubConfiguration& config);
		~BatteryComponent();

		//==========================================================================
		void paint(Graphics&) override;
		void resized() override { repaint(); }

        //==========================================================================
        void timerCallback (int timerID) override;
        
        //==========================================================================
        void repaintIfNeeded (bool forceRepaint = false);
        void repaintBlinkingIndicators();
        void update();

		//==========================================================================
		const float& batteryValueRef;

	private:

        //==========================================================================
        void launchDelayedRepaint (const int delayMs, bool forceRepaint = false);
        void drawLightningPath (Path& path, juce::Rectangle<float> area);
        void drawBatteryPath (Graphics& g, juce::Rectangle<float> area);
        void drawConnectedPath (Graphics& g, juce::Rectangle<float> area);
        void drawRingPath (Graphics& g, juce::Rectangle<float> area);

        bool waitForRepaint = false;

        //==========================================================================
        HubConfiguration& hubConfig;
        bool lastChargeState = false;
        bool lastConnectionState = false;

        //==========================================================================
        float lastBattery = -1.0f;
        float lastRawBattery = 3.0f;
        int numIndicators = 0;
        int numBlinkingIndicators = 0;
        juce::Rectangle<int> indicatorsArea;
        bool blinkState = false;

		//==========================================================================
		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BatteryComponent)
	};

	//==============================================================================
	void createButton();
	
    //==============================================================================
    std::unique_ptr<BatteryComponent> batteryComponent;
    std::unique_ptr<DashShapeButton> optionsButton;
    OptionsPanel& optionsPanel;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HeaderComponent)
};
