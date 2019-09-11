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

PresetSelectorComponent::PresetSelectorComponent (HubConfiguration& config, ApplicationCommandManager& manager)
    : hubConfig (config), commandManager (manager)
{
	// Creates Toggles And Lights
	for (int i =0; i < 4; i++)
	{
		toggles.add (new PresetToggle (i));
		toggles.getLast()->addListener (this);
		addAndMakeVisible (toggles.getLast());

		leds.add (new GestureLED (i, hubConfig));
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
	/*
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
	*/

	g.drawImage (hubImage, getLocalBounds().toFloat(),
						   RectanglePlacement::yBottom);

	//g.drawRect (getLocalBounds().withTop (getHeight()*2/3));
}

void PresetSelectorComponent::resized()
{
	using namespace neova_dash::ui;
	auto area = getLocalBounds().reduced (48, 62)
								.withTop (242)
								.translated (-1, 0);

	const int columnWidth = area.getWidth() / toggles.size();

	for (int column = 0; column < toggles.size(); column++)
	{
		auto columnArea = area.removeFromLeft (columnWidth);
		int columnXMargin = MARGIN;

		if (column == 0)                       columnArea.removeFromRight (columnXMargin);
		else if (column == toggles.size() - 1) columnArea.removeFromLeft (columnXMargin);
		else                                   columnArea.reduce (columnXMargin*2/3, 0);

		if (column == 1 || column == 2) columnArea.translate (0, 22);
		if (column == 2 || column == 3) columnArea.translate (4, 0);

		toggles[column]->setBounds (columnArea);
		leds[column]->setBounds (columnArea.withHeight (20).translated (0, -23));
	}
}

void PresetSelectorComponent::update()
{
	repaintLEDs();
	updateToggles();
}

void PresetSelectorComponent::repaintLEDs()
{
	for (int i =0; i < leds.size(); i++)
	{
		leds[i]->repaint();
	}
}

void PresetSelectorComponent::updateToggles()
{
	for (int i =0; i < toggles.size(); i++)
	{
		toggles[i]->setToggleState (i == hubConfig.getSelectedPreset(), dontSendNotification);
	}
}

void PresetSelectorComponent::buttonClicked (Button* bttn)
{
	if (auto presetToggle = dynamic_cast<PresetToggle*> (bttn))
	{
		// Sets a new current preset value
		if (presetToggle->id != currentPreset)
		{
			hubConfig.setPreset (presetToggle->id);

			// TO DELETE IF UPDATE LOGIC
			currentPreset = presetToggle->id; 
			commandManager.invokeDirectly (neova_dash::commands::updateDashInterface, true);
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

	/*
	//Shadow Draw
	g.setColour (Colours::black.withAlpha (0.2f));
	g.drawEllipse (ellipseArea.toFloat()
							  .withTrimmedRight (1.5f)
							  .withTrimmedBottom (1.5f),
				   1.5f);
	*/

	// Button Draw
	g.setColour (neova_dash::colour::mainText.withAlpha (getToggleState() ? 0.6f
																		  : shouldDrawButtonAsHighlighted ? 0.3f
																										  : 0.0f));
	g.drawEllipse (ellipseArea.toFloat(), 2.0f);

}

//==============================================================================
// GestureLED

PresetSelectorComponent::GestureLED::GestureLED (const int ledNum, HubConfiguration& config)
	: id (ledNum), hubConfig (config)
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

	if (hubConfig.getGestureData (id).type != int (neova_dash::gesture::none))
	{
		Colour ledColour (neova_dash::gesture::getHighlightColour
			                 (neova_dash::gesture::intToGestureType (hubConfig.getGestureData (id).type),
			                  hubConfig.getGestureData (id).on == 0 ? false : true));

		ColourGradient ledGrad (ledColour.withAlpha (0.6f), getLocalBounds().getCentre().toFloat(),
								Colour (0), getLocalBounds().getCentre().translated (0, 5).toFloat(),
								true);

		g.setColour (ledColour);
		g.fillEllipse (Rectangle<float>(3.0f, 3.0f).withCentre (ledArea.getCentre()));

		g.setGradientFill (ledGrad);
		g.fillEllipse (ledArea);
	}
	else
	{
		g.setColour (Colour (0x80000000));
		g.fillEllipse (Rectangle<float>(3.0f, 3.0f).withCentre (ledArea.getCentre()));
	}
}
void PresetSelectorComponent::GestureLED::resized()
{
}