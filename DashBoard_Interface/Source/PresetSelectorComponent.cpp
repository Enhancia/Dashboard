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
PresetSelectorComponent::PresetSelectorComponent()
{
}

PresetSelectorComponent::~PresetSelectorComponent()
{
}

void PresetSelectorComponent::paint (Graphics& g)
{
	ColourGradient gradFill = ColourGradient::vertical (Colour (0xff3a3a3a),
														Colour (0xff313131),
														getLocalBounds());

    g.setColour (Colour (0x40000000));
    g.fillRoundedRectangle (getLocalBounds().reduced (neova_dash::ui::MARGIN*2)
    										.translated (neova_dash::ui::MARGIN*2,
    													 neova_dash::ui::MARGIN*2).toFloat(), 30.0f);

	g.setGradientFill (gradFill);
    g.fillRoundedRectangle (getLocalBounds().reduced (neova_dash::ui::MARGIN*2).toFloat(), 30.0f);

}

void PresetSelectorComponent::resized()
{
}
