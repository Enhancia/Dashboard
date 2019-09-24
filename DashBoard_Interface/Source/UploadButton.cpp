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
	TRACE_IN;
}

UploadButton::~UploadButton()
{
	TRACE_IN;
}

void UploadButton::resized()
{
}

void UploadButton::paintButton (Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
{
	g.setColour (shouldDrawButtonAsDown
					 ? neova_dash::colour::uploadButtonBackground.withAlpha (0.7f)
				     : shouldDrawButtonAsHighlighted ? neova_dash::colour::uploadButtonBackground.brighter (0.07f)
				     								 : neova_dash::colour::uploadButtonBackground);

	auto buttonArea = getLocalBounds().reduced (neova_dash::ui::MARGIN).translated (-20, 0);
	if (shouldDrawButtonAsDown) buttonArea.translate (-1, 0);

	// Background Fill
	g.fillRoundedRectangle (buttonArea.toFloat(), 8.0f);

	auto textArea = getLocalBounds().withRightX (getLocalBounds().getRight()-21)
									.withLeft (30)
									.reduced (neova_dash::ui::MARGIN*3, neova_dash::ui::MARGIN*2);

	// Text Draw
	g.setColour (neova_dash::colour::mainText);
	g.setFont (neova_dash::font::dashFont.withHeight (15.0f));
	g.drawText ("Upload", textArea.removeFromRight (textArea.getWidth()*3/4), Justification::centred);

	// Upload Path Draw
	Path uploadPath = neova_dash::path::createPath (neova_dash::path::upload);
	textArea.reduce (neova_dash::ui::MARGIN_SMALL, neova_dash::ui::MARGIN_SMALL);
	uploadPath.scaleToFit (textArea.getX(), textArea.getY(),
                           textArea.getWidth(), textArea.getHeight(), true);

  	g.fillPath (uploadPath);
}

void UploadButton::clicked()
{
	commandManager.invokeDirectly (neova_dash::commands::flashHub, true);
}