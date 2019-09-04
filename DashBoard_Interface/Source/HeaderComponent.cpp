/*
  ==============================================================================

    HeaderComponent.cpp
    Created: 3 Sep 2019 11:36:30am
    Author:  Enhancia Dev

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "HeaderComponent.h"

//==============================================================================
HeaderComponent::HeaderComponent()
{
    batteryComponent = std::make_unique<BatteryComponent>();
    addAndMakeVisible (*batteryComponent);
}

HeaderComponent::~HeaderComponent()
{
	batteryComponent = nullptr;
}

void HeaderComponent::paint (Graphics& g)
{
    g.setColour (neova_dash::colour::headerBackground);
    g.fillRoundedRectangle (getLocalBounds().toFloat(), 3.0f);

	auto area = getLocalBounds().reduced (0, neova_dash::ui::MARGIN_SMALL);
    g.setColour (neova_dash::colour::mainText);

    g.setFont (Font().withHeight (17.0f).boldened().withExtraKerningFactor (0.1f));
    g.drawText ("Neova", area, Justification::centredTop, false);

	g.setFont(Font().withHeight (10.0f));
	g.drawText("DASHBOARD", area, Justification::centredBottom, false);
}

void HeaderComponent::resized()
{
	auto area = getLocalBounds().reduced (neova_dash::ui::MARGIN, 0);

	batteryComponent->setBounds (area.withLeft (area.getWidth()*8/9));
}

void HeaderComponent::BatteryComponent::paint (Graphics& g)
{
	g.setColour (neova_dash::colour::mainText);

	auto area = getLocalBounds().reduced(neova_dash::ui::MARGIN);

    g.setFont (Font().withHeight (11.0f));
    g.drawText ("Ring Battery", getLocalBounds().withWidth (getWidth()*2/3), Justification::centred, true);

    auto batteryArea = area.withLeft (getWidth()*2/3)
    					   .withSizeKeepingCentre (15, 8)
    					   .translated (0, 1);

    g.drawRect (batteryArea, 1);

	g.drawRect (juce::Rectangle<int>(2, 4).withPosition ({batteryArea.getRight(), batteryArea.getY() + 2}), 1);

	float battery = jmax (jmin (batteryValueRef, 1.0f), 0.0f);

	if (battery <= 0.2f) g.setColour (Colours::red);
	else if (battery == 1.0f) g.setColour (Colours::lime);

    g.fillRect (batteryArea.reduced (2)
    					   .withRight (batteryArea.getX() + 2
    					   				   + int ((batteryArea.getWidth() - 4) * battery)));
}