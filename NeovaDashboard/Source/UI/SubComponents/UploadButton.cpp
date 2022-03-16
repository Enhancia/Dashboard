/*
  ==============================================================================

    UploadButton.cpp
    Created: 3 Sep 2019 5:11:34pm
    Author:  Enhancia Dev

  ==============================================================================
*/

#include "UploadButton.h"

UploadButton::UploadButton (HubConfiguration& config, ApplicationCommandManager& cm, const bool shouldBeActive)
	: Button ("Upload Button"), active (shouldBeActive), commandManager (cm), hubConfig (config)
{
	  setInterceptsMouseClicks (active, false);
}

UploadButton::~UploadButton()
{
}

void UploadButton::resized()
{
}

void UploadButton::timerCallback()
{
	repaint (getLocalBounds().removeFromRight (40));

	if (animationCounter < ANIM_MAX)
	{
		animationCounter++;
	}
	else
	{
		stopAnimating();
	}
}

void UploadButton::setActive (const bool shouldBeActive)
{
	if (active == shouldBeActive) return;

	if (animating) stopAnimating();

	active = shouldBeActive;

	setInterceptsMouseClicks (active, false);

	repaint();
}

bool UploadButton::isActive() const
{
	return active;
}

void UploadButton::update()
{
    if (!active && hubConfig.wasConfigChangedSinceLastFlash())
    {
        setActive();
    }
}

void UploadButton::setInactiveAndShowUploadFeedback()
{
	setActive (false);
	startAnimating();
}

void UploadButton::paintButton (Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
{
	if (!active)
	{
		shouldDrawButtonAsHighlighted = false;
		shouldDrawButtonAsDown = false;

		g.beginTransparencyLayer (0.5f);
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
	Path uploadPath = createPath(neova_dash::path::upload);
	textArea = textArea.withSizeKeepingCentre (textArea.getWidth(), 16);
	uploadPath.scaleToFit (static_cast<float>(textArea.getX()), static_cast<float>(textArea.getY()),
                           static_cast<float>(textArea.getWidth()), static_cast<float>(textArea.getHeight()), true);

  	g.fillPath (uploadPath);

  	if (!active) g.endTransparencyLayer();

  	if (animating)
  	{
  		drawUploadFeedback (g, getLocalBounds().removeFromRight (40));
  	}
}

void UploadButton::clicked()
{
	hubConfig.flashHub();

	setInactiveAndShowUploadFeedback();
}

void UploadButton::startAnimating()
{
	animating = true;
	animationCounter = 0;
	startTimerHz (30);
}

void UploadButton::stopAnimating()
{
	animating = false;
	stopTimer();
}

void UploadButton::drawUploadFeedback (Graphics& g, juce::Rectangle<int> area) const
{
	const float animationProgress = animationCounter/static_cast<float>(ANIM_MAX);
	const float alpha = animationProgress < 0.5f ? 1.0f
												 : 1.0f - std::pow (animationProgress*2 - 1.0f, 2.0f);

	g.setColour (neova_dash::colour::mainText.withAlpha (alpha));

	g.setFont (neova_dash::font::dashFontLight.withHeight (11.0f));
	g.drawFittedText ("Uploaded!", area, Justification::centred, 1);
}
