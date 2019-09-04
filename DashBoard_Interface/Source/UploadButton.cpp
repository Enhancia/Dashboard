/*
  ==============================================================================

    UploadButton.cpp
    Created: 3 Sep 2019 5:11:34pm
    Author:  Enhancia Dev

  ==============================================================================
*/

#include "UploadButton.h"


UploadButton::UploadButton (ApplicationCommandManager& cm) : Button ("Upload Button"), commandManager (cm)
{
}

UploadButton::~UploadButton()
{
}

void UploadButton::resized()
{
}

void UploadButton::paintButton (Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
{
	g.setColour (neova_dash::colour::uploadButtonBackground.withAlpha (shouldDrawButtonAsDown ? 0.4f : 1.0f));

	auto buttonArea = getLocalBounds().reduced (neova_dash::ui::MARGIN).translated (-20, 0);
	if (shouldDrawButtonAsHighlighted || shouldDrawButtonAsDown) buttonArea.translate (-3, 0);

	// Background Fill
	g.fillRoundedRectangle (buttonArea.toFloat(), 8.0f);

	// Text Draw
	g.setColour (neova_dash::colour::mainText);
	g.setFont (Font().withHeight (16.0f));
	g.drawText ("Upload", buttonArea.withLeft (buttonArea.getWidth()/4), Justification::centred);

	// Upload Path Draw
}

void UploadButton::clicked()
{
	commandManager.invokeDirectly (neova_dash::commands::flashHub, true);
}