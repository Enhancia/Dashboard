/*
  ==============================================================================

    PresetSelectorComponent.cpp
    Created: 17 Sep 2019 3:57:45pm
    Author:  Enhancia Dev

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "PresetSelectorComponent.h"

//==============================================================================
PresetSelectorComponent::PresetSelectorComponent (HubConfiguration& config)
{
}

PresetSelectorComponent::~PresetSelectorComponent()
{
}

void PresetSelectorComponent::paint (Graphics& g)
{
    g.setColour (neova_dash::colour::mainText);
    g.setFont (Font().withHeight (14.0f));

    g.drawText (String ("Preset " + String (hubConfig.getSelectedPreset())),
    			getLocalBounds().withSizeKeepingCentre (getWidth()/2, getHeight()),
    			Justification::centred);



    g.set
}

void PresetSelectorComponent::resized()
{
}

void PresetSelectorComponent::mouseEnter (const MouseEvent &event)
{
}

void PresetSelectorComponent::mouseExit (const MouseEvent &event)
{
}

void PresetSelectorComponent::mouseUp (const MouseEvent &event)
{
}

void PresetSelectorComponent::createButtons()
{
	// Preset Change buttons
    Path arrowLeft;
    arrowLeft.startNewSubPath (5.0f, 0.0f);
    arrowLeft.lineTo (0.0f, 5.0f);
    arrowLeft.lineTo (5.0f, 10.0f);

    Path arrowRight;
    arrowRight.startNewSubPath (0.0f, 0.0f);
    arrowRight.lineTo (5.0f, 5.0f);
    arrowRight.lineTo (0.0f, 10.0f);

    addAndMakeVisible (leftArrowButton = new DashShapeButton ("Change Preset Left Button",
                                                              Colour(0x00000000),
                                                              neova_dash::colour::mainText));


    leftArrowButton->setShape (arrowLeft, false, true, false);
    leftArrowButton->addListener (this);

    addAndMakeVisible (rightArrowButton = new DashShapeButton ("Change Preset Right Button",
                                                                Colour(0x00000000),
                                                                neova_dash::colour::mainText));


    rightArrowButton->setShape (arrowRight, false, true, false);
    rightArrowButton->addListener (this);
}