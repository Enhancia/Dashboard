/*
  ==============================================================================

    HeaderComponent.cpp
    Created: 3 Sep 2019 11:36:30am
    Author:  Enhancia Dev

  ==============================================================================
*/

#include "HeaderComponent.h"

//==============================================================================
HeaderComponent::HeaderComponent (OptionsPanel& options, HubConfiguration& config, DataReader& reader)
    : optionsPanel (options)
{
    TRACE_IN;
    batteryComponent = std::make_unique<BatteryComponent> (reader.getFloatValueReference (neova_dash::data::battery),
                                                           config);
    addAndMakeVisible (*batteryComponent);

    createButton();
}

HeaderComponent::~HeaderComponent()
{
    TRACE_IN;

	batteryComponent = nullptr;
}

void HeaderComponent::paint (Graphics& g)
{
    g.setColour (neova_dash::colour::headerBackground);
    g.fillRoundedRectangle (getLocalBounds().toFloat(), 3.0f);

	auto area = getLocalBounds().reduced (0, neova_dash::ui::MARGIN_SMALL);
    g.setColour (neova_dash::colour::mainText);

    g.setFont (neova_dash::font::dashFont.withHeight (19.0f).boldened().withExtraKerningFactor (0.1f));
    g.drawText ("Neova", area, Justification::centredTop, false);

	g.setFont(neova_dash::font::dashFont.withHeight (12.0f));
	g.drawText("DASHBOARD", area, Justification::centredBottom, false);
}

void HeaderComponent::resized()
{
	auto area = getLocalBounds().reduced (neova_dash::ui::MARGIN, 0);

	batteryComponent->setBounds (area.removeFromRight (jmax (area.getWidth()/9, 88)));
    DBG ("Battery Comp " << batteryComponent->getWidth()); // TO DELETE

    optionsButton->setBounds (area.removeFromLeft (40));
}

void HeaderComponent::buttonClicked (Button* bttn)
{
    if (bttn == optionsButton.get())
    {
        optionsPanel.setVisible (true);
    }
}

void HeaderComponent::createButton()
{
    // Close button
    optionsButton = std::make_unique <DashShapeButton> ("Open Options Button",
                                                       Colour(0),
                                                       neova_dash::colour::mainText);
    addAndMakeVisible (*optionsButton);

    optionsButton->setShape (neova_dash::path::createPath (neova_dash::path::options),
                            false, true, false);
    optionsButton->setPaintMode (DashShapeButton::fillAndStroke);
    optionsButton->setStrokeThickness (1.8f);
    optionsButton->addListener (this);
}

void HeaderComponent::timerCallback()
{
    update();
}

void HeaderComponent::update()
{
    repaintBattery();
}

void HeaderComponent::repaintBattery()
{
    batteryComponent->repaintIfNeeded();
}

void HeaderComponent::BatteryComponent::paint (Graphics& g)
{
	g.setColour (neova_dash::colour::mainText);

	auto area = getLocalBounds().reduced(neova_dash::ui::MARGIN);

    g.setFont (neova_dash::font::dashFont.withHeight (12.0f));
    g.drawText ("Ring Battery", getLocalBounds().withWidth (getWidth()*2/3), Justification::centred, true);

    auto batteryArea = area.withLeft (getWidth()*2/3)
    					   .withSizeKeepingCentre (15, 8)
    					   .translated (0, 1);

    g.drawRect (batteryArea, 1);

	g.drawRect (juce::Rectangle<int>(2, 4).withPosition ({batteryArea.getRight(), batteryArea.getY() + 2}), 1);

	g.setColour ((lastBattery <= 0.2f) ? Colours::red
                                       : (lastBattery == 1.0f) ? Colours::lime
                                                               : neova_dash::colour::mainText);

    g.fillRect (batteryArea.reduced (2)
    					   .withRight (batteryArea.getX() + 2
    					   				   + int ((batteryArea.getWidth() - 4) * lastBattery)));
}

void HeaderComponent::BatteryComponent::repaintIfNeeded()
{
    const float battery = jmax (jmin (neova_dash::dataconvertRawBatteryToPercentage (batteryValueRef),
                                      1.0f),
                                0.0f);

    if (battery != lastBattery)
    {
        lastBattery = battery;
        repaint();
    }
}