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
					 ? neova_dash::colour::uploadButtonBackground.darker (0.3f)
				     : shouldDrawButtonAsHighlighted ? neova_dash::colour::uploadButtonBackground.brighter (0.07f)
				     								 : neova_dash::colour::uploadButtonBackground);

	auto buttonArea = getLocalBounds().reduced (neova_dash::ui::MARGIN);
	if (shouldDrawButtonAsDown) buttonArea = buttonArea.withTrimmedRight (1);

	// Background Fill
	g.fillRoundedRectangle (buttonArea.withLeft (getX()-20).toFloat(), 8.0f);

	auto textArea = getLocalBounds().withTrimmedRight (neova_dash::ui::MARGIN)
									.reduced (neova_dash::ui::MARGIN, neova_dash::ui::MARGIN);

	// Text Draw
	g.setColour (neova_dash::colour::mainText);
	g.setFont (neova_dash::font::dashFont.withHeight (16.0f));
	g.drawText ("Upload", textArea.removeFromRight (textArea.getWidth()*2/3), Justification::centred);

	// Upload Path Draw
	Path uploadPath = neova_dash::path::createPath (neova_dash::path::upload);
	textArea = textArea.withSizeKeepingCentre (textArea.getWidth(), 16);
	uploadPath.scaleToFit (textArea.getX(), textArea.getY(),
                           textArea.getWidth(), textArea.getHeight(), true);

  	g.fillPath (uploadPath);
}

void UploadButton::clicked()
{
	commandManager.invokeDirectly (neova_dash::commands::flashHub, true);
}