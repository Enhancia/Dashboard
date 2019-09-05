/*
  ==============================================================================

    PresetSelectorComponent.cpp
    Created: 3 Sep 2019 11:37:45am
    Author:  Enhancia Dev

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "PresetSelectorComponent.h"

//==============================================================================
// PresetSelectorComponent

PresetSelectorComponent::PresetSelectorComponent (ConfigData& data) : configData (data)
{
	// Creates Toggles And Lights
	for (int i =0; i < 4; i++)
	{
		toggles.add (new PresetToggle (i));
		toggles.getLast()->addListener (this);
		addAndMakeVisible (toggles.getLast());

		leds.add (new GestureLED (i));
		addAndMakeVisible (leds.getLast());
	}
}

PresetSelectorComponent::~PresetSelectorComponent()
{
	for (int i =0; i < toggles.size(); i++)
	{
		toggles[i]->removeListener (this);
	}

	toggles.clear();
	leds.clear();
}

void PresetSelectorComponent::paint (Graphics& g)
{
	// TEMP (should be replaced with actual HUB image (Might be general background or this comp background))
	
	ColourGradient gradFill = ColourGradient::vertical (Colour (0xff3a3a3a),
														Colour (0xff313131),
														getLocalBounds());

	// shadow
    g.setColour (Colour (0x40000000));
    g.fillRoundedRectangle (getLocalBounds().reduced (neova_dash::ui::MARGIN*2)
    										.translated (neova_dash::ui::MARGIN*2,
    													 neova_dash::ui::MARGIN*2).toFloat(), 60.0f);

 	// HUB
	g.setGradientFill (gradFill);
    g.fillRoundedRectangle (getLocalBounds().reduced (neova_dash::ui::MARGIN*2).toFloat(), 60.0f);

}

void PresetSelectorComponent::resized()
{
	auto area = getLocalBounds().reduced (neova_dash::ui::MARGIN*3, neova_dash::ui::MARGIN*4)
								.withTop (getHeight()*2/3);
	const int columnWidth = area.getWidth() / toggles.size();

	for (int column = 0; column < toggles.size(); column++)
	{
		auto columnArea = area.removeFromLeft (columnWidth).reduced (neova_dash::ui::MARGIN_SMALL*3);
		if (column == 1 || column == 2) columnArea.translate (0, 22);

		toggles[column]->setBounds (columnArea);
		leds[column]->setBounds (columnArea.withHeight (20).translated (0, -23));
	}
}

void PresetSelectorComponent::buttonClicked (Button* bttn)
{
	if (auto presetToggle = dynamic_cast<PresetToggle*> (bttn))
	{
		// Sets a new current preset value
		if (presetToggle->id != currentPreset)
		{
			configData.setDataAndUploadToHub (ConfigData::selectedPreset, float (presetToggle->id));

			currentPreset = presetToggle->id; // TO DELETE IF UPDATE LOGIC
		}
	}
}

//==============================================================================
// PresetToggle

PresetSelectorComponent::PresetToggle::PresetToggle (const int toggleNum)
	: Button (String ("Preset Toggle ") + String (toggleNum)),
	  id (toggleNum)
{
	setClickingTogglesState (true);
	setRadioGroupId (1, dontSendNotification);
}
PresetSelectorComponent::PresetToggle::~PresetToggle()
{
}
void PresetSelectorComponent::PresetToggle::resized()
{
}
void PresetSelectorComponent::PresetToggle::paintButton (Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
{
	auto ellipseArea = getLocalBounds().withSize (jmin (getWidth(), getHeight()),
											  jmin (getWidth(), getHeight()))
								   .withCentre ({ getLocalBounds().getCentreX(),
								   				  jmin (getWidth(), getHeight())/2 })
								   .reduced (1);

	//Shadow Draw
	g.setColour (Colours::black.withAlpha (0.2f));
	g.drawEllipse (ellipseArea.toFloat()
							  .withTrimmedRight (1.5f)
							  .withTrimmedBottom (1.5f),
				   1.5f);

	// Button Draw
	g.setColour (neova_dash::colour::mainText.interpolatedWith (Colour (0xff313131),
																getToggleState()
																	? 0.0f : shouldDrawButtonAsHighlighted ? 0.3f
																										   : 0.6f));
	g.drawEllipse (ellipseArea.toFloat(), 1.5f);

}

//==============================================================================
// GestureLED

PresetSelectorComponent::GestureLED::GestureLED (const int ledNum) : id (ledNum)
{
}
PresetSelectorComponent::GestureLED::~GestureLED()
{
}
void PresetSelectorComponent::GestureLED::paint (Graphics& g)
{
	auto ledArea = Rectangle<float> (float (jmin (getHeight(), getWidth())),
									 float (jmin (getHeight(), getWidth()))).withCentre (getLocalBounds().getCentre()
																			                             .toFloat());
	Colour ledColour (Colours::lightblue);

	ColourGradient ledGrad (ledColour.withAlpha (0.6f), getLocalBounds().getCentre().toFloat(),
							Colour (0), getLocalBounds().getCentre().translated (0, 5).toFloat(),
							true);

	g.setColour (ledColour);
	g.fillEllipse (Rectangle<float>(3.0f, 3.0f).withCentre (ledArea.getCentre()));

	g.setGradientFill (ledGrad);
	g.fillEllipse (ledArea);
}
void PresetSelectorComponent::GestureLED::resized()
{
}