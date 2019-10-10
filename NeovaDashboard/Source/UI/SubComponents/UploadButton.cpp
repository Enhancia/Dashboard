/*
  ==============================================================================

    UploadButton.cpp
    Created: 3 Sep 2019 5:11:34pm
    Author:  Enhancia Dev

  ==============================================================================
*/

#include "UploadButton.h"

UploadButton::UploadButton (HubConfiguration& config, ApplicationCommandManager& cm)
	: Button ("Upload Button"), commandManager (cm), hubConfig (config)
{
	TRACE_IN;

	setActive (false);
}

UploadButton::~UploadButton()
{
	TRACE_IN;
}

void UploadButton::resized()
{
}

void UploadButton::timerCallback()
{
}

void UploadButton::setActive()
{
	if (!active) setActive (true);
}

void UploadButton::setActive (const bool shouldBeActive)
{
	active = shouldBeActive;

	setInterceptsMouseClicks (active, false);
	setAlpha (active ? 1.0f : 0.5f);

	repaint();
}

void UploadButton::setInactiveAndShowUploadFeedback()
{
	setActive (false);
}

void UploadButton::paintButton (Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
{
	if (!active)
	{
		shouldDrawButtonAsHighlighted = false;
		shouldDrawButtonAsDown = false;
	}

	g.setColour (shouldDrawButtonAsDown
					 ? neova_dash::colour::uploadButtonBackground.darker (0.3f)
				     : shouldDrawButtonAsHighlighted ? neova_dash::colour::uploadButtonBackground.brighter (0.07f)
				     								 : neova_dash::colour::uploadButtonBackground);

	auto buttonArea = getLocalBounds().withTrimmedRight (40)
									  .reduced (neova_dash::ui::MARGIN);

	if (shouldDrawButtonAsDown) buttonArea = buttonArea.withTrimmedRight (1);

	// Background Fill
	g.fillRoundedRectangle (buttonArea.withLeft (getX()-20).toFloat(), 8.0f);

	auto textArea = getLocalBounds().withTrimmedRight (neova_dash::ui::MARGIN + 40)
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
	hubConfig.flashHub();

	setInactiveAndShowUploadFeedback();
}