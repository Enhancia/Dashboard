/*
  ==============================================================================

    PresetSelectorComponent.cpp
    Created: 17 Sep 2019 3:57:45pm
    Author:  Enhancia Dev

  ==============================================================================
*/

#include "PresetSelectorComponent.h"

//==============================================================================
PresetSelectorComponent::PresetSelectorComponent (HubConfiguration& config, ApplicationCommandManager& manager)
	: hubConfig (config), commandManager (manager)
{
	createButtons();
}

PresetSelectorComponent::~PresetSelectorComponent()
{
}

void PresetSelectorComponent::paint (Graphics& g)
{
    g.setColour (neova_dash::colour::mainText);
    g.setFont (neova_dash::font::dashFont.withHeight (14.0f));

    g.drawText (String ("Slot " + String (hubConfig.getSelectedPreset() + 1)),
    			getLocalBounds().withSizeKeepingCentre (getWidth()/2, getHeight()),
    			Justification::centred);
}

void PresetSelectorComponent::resized()
{
	auto area = getLocalBounds();

	leftArrowButton->setBounds (area.removeFromLeft (20).reduced (0, 3));
	rightArrowButton->setBounds (area.removeFromRight (20).reduced (0, 3));
}

void PresetSelectorComponent::buttonClicked (Button* bttn)
{
	if (bttn == leftArrowButton.get())
	{
		hubConfig.setPreset (hubConfig.getSelectedPreset() - 1);
	}
	else if (bttn == rightArrowButton.get())
	{
		hubConfig.setPreset (hubConfig.getSelectedPreset() + 1);
	}

	commandManager.invokeDirectly (neova_dash::commands::updateDashInterface, true);
}

void PresetSelectorComponent::update()
{
    leftArrowButton->setVisible (hubConfig.getSelectedPreset() == 0 ? false : true);
    rightArrowButton->setVisible (hubConfig.getSelectedPreset() == neova_dash::gesture::NUM_GEST-1 ? false : true);
    
	repaint();
}

void PresetSelectorComponent::createButtons()
{
    Path arrowLeft;
    arrowLeft.startNewSubPath (5.0f, 0.0f);
    arrowLeft.lineTo (0.0f, 5.0f);
    arrowLeft.lineTo (5.0f, 10.0f);

    leftArrowButton = std::make_unique<DashShapeButton> ("Change Preset Left Button",
                                                         Colour(0x00000000),
                                                         neova_dash::colour::mainText);

    addAndMakeVisible (*leftArrowButton);

    leftArrowButton->setShape (arrowLeft, false, true, false);
    leftArrowButton->addListener (this);
    leftArrowButton->addMouseListener (this, false);

    Path arrowRight;
    arrowRight.startNewSubPath (0.0f, 0.0f);
    arrowRight.lineTo (5.0f, 5.0f);
    arrowRight.lineTo (0.0f, 10.0f);

    rightArrowButton = std::make_unique<DashShapeButton> ("Change Preset Right Button",
                                                          Colour(0x00000000),
                                                          neova_dash::colour::mainText);

    addAndMakeVisible (*rightArrowButton);

    rightArrowButton->setShape (arrowRight, false, true, false);
    rightArrowButton->addListener (this);
    rightArrowButton->addMouseListener (this, false);

    if (hubConfig.getSelectedPreset() == 0)
    {
    	leftArrowButton->setVisible (false);
    }
    else if (hubConfig.getSelectedPreset() == neova_dash::gesture::NUM_GEST-1)
    {
    	rightArrowButton->setVisible (false);
    }
}

