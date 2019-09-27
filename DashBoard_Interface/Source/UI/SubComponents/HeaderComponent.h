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
#include "Top/OptionsPanel.h"

//==============================================================================
/*
*/

class HeaderComponent    : public Component,
						   private Button::Listener
{
public:
    //==============================================================================
    HeaderComponent (OptionsPanel& options);
    ~HeaderComponent();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

    //==============================================================================
    void buttonClicked (Button* bttn) override;
    
    //==============================================================================
    void mouseUp (const MouseEvent&) override;

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
		float batteryValueRef = 1.0f;

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
