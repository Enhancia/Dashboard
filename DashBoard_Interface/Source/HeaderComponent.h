/*
  ==============================================================================

    HeaderComponent.h
    Created: 3 Sep 2019 11:36:30am
    Author:  Enhancia Dev

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "DashCommon.h"

//==============================================================================
/*
*/

class HeaderComponent    : public Component
{
public:
    //==============================================================================
    HeaderComponent();
    ~HeaderComponent();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

private:
	class BatteryComponent : public Component
	{
	public:
		//==========================================================================
		BatteryComponent(/*float& batteryValRef*/) /*: batteryValueRef (batteryValRef)*/ {}
		~BatteryComponent() {}

		//==========================================================================
		void paint(Graphics&) override;
		void resized() override { repaint(); }

	private:
		//==========================================================================
		float batteryValueRef = 0.2f;

		//==========================================================================
		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BatteryComponent)
	};

    //==============================================================================
    std::unique_ptr<BatteryComponent> batteryComponent;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HeaderComponent)
};
