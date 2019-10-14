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

private:
	class BatteryComponent : public Component,
                             public Timer
	{
	public:
		//==========================================================================
		BatteryComponent (const float& batteryValRef, HubConfiguration& config);
		~BatteryComponent();

		//==========================================================================
		void paint(Graphics&) override;
		void resized() override { repaint(); }

        //==========================================================================
        void timerCallback() override;
        
        //==========================================================================
        void repaintIfNeeded();
        void repaintChargeState();

		//==========================================================================
		const float& batteryValueRef;

	private:
        //==========================================================================
        void drawLightningPath (Graphics& g, Rectangle<float> area);

        //==========================================================================
        HubConfiguration& hubConfig;
        bool lastChargeState = false;
        float lastBattery = -1.0f;

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
