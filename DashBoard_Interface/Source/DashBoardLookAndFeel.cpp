/*
  ==============================================================================

    DashBoardLookAndFeel.cpp
    Created: 12 Sep 2019 10:11:02am
    Author:  Enhancia Dev

  ==============================================================================
*/

#include "DashBoardLookAndFeel.h"

DashBoardLookAndFeel::DashBoardLookAndFeel()
{                   
	  setColours();
}

DashBoardLookAndFeel::~DashBoardLookAndFeel()
{
}

void DashBoardLookAndFeel::setColours()
{
    using namespace neova_dash::colour;
    
    // Label
	  setColour (Label::textColourId, mainText);
	  setColour (Label::backgroundColourId, Colour (0));
	  setColour (Label::outlineColourId, Colour (0));

    // Text 
    setColour (TextEditor::outlineColourId, Colour (0));
    setColour (TextEditor::backgroundColourId, Colour (0));
    setColour (TextEditor::focusedOutlineColourId, Colour (0));
    setColour (TextEditor::highlightColourId, mainText.withAlpha (0.5f));
    setColour (CaretComponent::caretColourId, mainText);

  	// Slider
  	setColour (Slider::thumbColourId, tunerSliderThumb);
  	setColour (Slider::trackColourId, tunerSliderBackground);
  	setColour (Slider::backgroundColourId, Colour (0));

  	// TextButton
  	setColour (TextButton::buttonColourId, midiSliderBackground);

  	// ComboBox
  	setColour (ComboBox::backgroundColourId, Colour (0));
  	setColour (ComboBox::textColourId, mainText);
  	setColour (ComboBox::arrowColourId, mainText);
  	setColour (ComboBox::outlineColourId, Colour (0)); // Necessary to not draw Label outlines due to an Error in the JUCE code..
  	
  	// PopupMenu
  	setColour (PopupMenu::backgroundColourId, gestureBackground);
  	setColour (PopupMenu::textColourId, mainText);
  	setColour (PopupMenu::highlightedBackgroundColourId, subText);
  	setColour (PopupMenu::highlightedTextColourId, mainText);
}

Font DashBoardLookAndFeel::getTextButtonFont (TextButton&, int buttonHeight)
{
    return neova_dash::font::dashFont.withHeight ((buttonHeight * 6.0f) / 10.0f);
}

Font DashBoardLookAndFeel::getComboBoxFont (ComboBox& cb)
{
	return neova_dash::font::dashFont.withHeight (jmax (11.0f, jmin (14.0f, (cb.getHeight() * 6.0f) / 10.0f)));
}

void DashBoardLookAndFeel::drawRotarySlider (Graphics& g, int x, int y, int width, int height, float sliderPos,
                                             const float rotaryStartAngle, const float rotaryEndAngle, Slider& slider)
{
    auto outline = slider.findColour (Slider::rotarySliderOutlineColourId);
    auto fill    = slider.findColour (Slider::rotarySliderFillColourId);

    auto bounds = juce::Rectangle<int> (x, y, width, height).toFloat().reduced (10);

    auto radius = jmin (bounds.getWidth(), bounds.getHeight()) / 2.0f;
    auto toAngle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
    auto lineW = jmin (8.0f, radius * 0.5f);
    auto arcRadius = radius - lineW * 0.5f;

    Path backgroundArc;
    backgroundArc.addCentredArc (bounds.getCentreX(),
                                 bounds.getCentreY(),
                                 arcRadius,
                                 arcRadius,
                                 0.0f,
                                 rotaryStartAngle,
                                 rotaryEndAngle,
                                 true);

    g.setColour (outline);
    g.strokePath (backgroundArc, PathStrokeType (12.0f, PathStrokeType::curved, PathStrokeType::rounded));

    if (slider.isEnabled())
    {
        Path valueArc;
        valueArc.addCentredArc (bounds.getCentreX(),
                                bounds.getCentreY(),
                                arcRadius,
                                arcRadius,
                                0.0f,
                                rotaryStartAngle,
                                toAngle,
                                true);

        g.setColour (fill);
        g.strokePath (valueArc, PathStrokeType (6.0f, PathStrokeType::curved, PathStrokeType::rounded));
    }

    auto thumbWidth = 6.0f;
    Point<float> thumbPoint (bounds.getCentreX() + arcRadius * std::cos (toAngle - MathConstants<float>::halfPi),
                             bounds.getCentreY() + arcRadius * std::sin (toAngle - MathConstants<float>::halfPi));
  if (slider.getThumbBeingDragged() != -1)
  {
    g.setColour (fill.withAlpha(0.6f));
    g.fillEllipse (juce::Rectangle<float> (25.0f, 25.0f).withCentre (thumbPoint));
  }

    g.setColour (slider.findColour (Slider::thumbColourId));
    g.fillEllipse (juce::Rectangle<float> (thumbWidth, thumbWidth).withCentre (thumbPoint));
}

void DashBoardLookAndFeel::drawLinearSlider (Graphics& g, int x, int y, int width, int height,
                                             float sliderPos,
                                             float minSliderPos,
                                             float maxSliderPos,
                                             const Slider::SliderStyle style, Slider& slider)
{
  using juce::Rectangle;

    if (slider.isBar())
    {
        g.setColour (slider.findColour (Slider::trackColourId));
        g.fillRect (slider.isHorizontal() ? juce::Rectangle<float> (static_cast<float> (x), y + 0.5f, sliderPos - x, height - 1.0f)
                                          : juce::Rectangle<float> (x + 0.5f, sliderPos, width - 1.0f, y + (height - sliderPos)));
    }
    else
    {
        auto isTwoVal   = (style == Slider::SliderStyle::TwoValueVertical   || style == Slider::SliderStyle::TwoValueHorizontal);
        auto isThreeVal = (style == Slider::SliderStyle::ThreeValueVertical || style == Slider::SliderStyle::ThreeValueHorizontal);

        auto trackWidth = jmin (6.0f, slider.isHorizontal() ? height * 0.25f : width * 0.25f);

        Point<float> startPoint (slider.isHorizontal() ? x : x + width * 0.5f,
                                 slider.isHorizontal() ? y + height * 0.5f : height + y);

        Point<float> endPoint (slider.isHorizontal() ? width + x : startPoint.x,
                               slider.isHorizontal() ? startPoint.y : y);

        Path backgroundTrack;
        backgroundTrack.startNewSubPath (startPoint);
        backgroundTrack.lineTo (endPoint);
        g.setColour (slider.findColour (Slider::backgroundColourId));
        g.strokePath (backgroundTrack, {  12.0f //Old: trackWidth
                                          , PathStrokeType::curved, PathStrokeType::rounded }); // changed track width

        Path valueTrack;
        Point<float> minPoint, maxPoint, thumbPoint;

        if (isTwoVal || isThreeVal)
        {
            minPoint = { slider.isHorizontal() ? minSliderPos : width * 0.5f,
                         slider.isHorizontal() ? height * 0.5f : minSliderPos };

            if (isThreeVal)
                thumbPoint = { slider.isHorizontal() ? sliderPos : width * 0.5f,
                               slider.isHorizontal() ? height * 0.5f : sliderPos };

            maxPoint = { slider.isHorizontal() ? maxSliderPos : width * 0.5f,
                         slider.isHorizontal() ? height * 0.5f : maxSliderPos };
        }
        else
        {
            auto kx = slider.isHorizontal() ? sliderPos : (x + width * 0.5f);
            auto ky = slider.isHorizontal() ? (y + height * 0.5f) : sliderPos;

            minPoint = startPoint;
            maxPoint = { kx, ky };
        }

        auto thumbWidth = 6.0f;

        valueTrack.startNewSubPath (minPoint);
        valueTrack.lineTo (isThreeVal ? thumbPoint : maxPoint);
        g.setColour (slider.findColour (Slider::trackColourId));
        g.strokePath(valueTrack, { 6.0f //Old: trackWidth
                                   , PathStrokeType::curved, PathStrokeType::rounded });
        

        if (! isTwoVal)
        {
      if (slider.getThumbBeingDragged() != -1 &&
                slider.findColour (Slider::trackColourId) != Colour (0x00000000))
      {
        g.setColour(slider.findColour (Slider::trackColourId).withAlpha (0.6f));
        g.fillEllipse(juce::Rectangle<float> (25.0f, 25.0f).withCentre (maxPoint));
      }

            g.setColour (slider.findColour (Slider::thumbColourId));
            g.fillEllipse (juce::Rectangle<float> (static_cast<float> (thumbWidth),
                                                   static_cast<float> (thumbWidth)).withCentre (maxPoint));
        }

        if (isTwoVal || isThreeVal)
        {
            if (slider.getThumbBeingDragged() != -1 &&
                slider.findColour (Slider::trackColourId) != Colour (0x00000000))
            {
                g.setColour (slider.findColour (Slider::trackColourId).withAlpha (0.6f));
                g.fillEllipse (juce::Rectangle<float> (25.0f, 25.0f)
                                  .withCentre (slider.getThumbBeingDragged() == 1 ? minPoint
                                                                                  : maxPoint));
            }

            g.setColour (slider.findColour (Slider::thumbColourId));
            g.fillEllipse (juce::Rectangle<float> (static_cast<float> (thumbWidth),
                                                   static_cast<float> (thumbWidth)).withCentre (maxPoint));
            g.fillEllipse (juce::Rectangle<float> (static_cast<float> (thumbWidth),
                                                   static_cast<float> (thumbWidth)).withCentre (minPoint));
        }
    }
}

int DashBoardLookAndFeel::getDefaultScrollbarWidth()
{
    return 8;
}

void DashBoardLookAndFeel::drawScrollbar (Graphics& g, ScrollBar& scrollbar,
                                      int x, int y,
                                      int width, int height,
                                      bool isScrollbarVertical,
                                      int thumbStartPosition, int thumbSize,
                                      bool isMouseOver, bool isMouseDown)
{
    ignoreUnused (isMouseDown);

    juce::Rectangle<int> thumbBounds;

    if (isScrollbarVertical)
        thumbBounds = { x + int (std::ceil (width/2.0f)), thumbStartPosition,
                        int (std::floor (width/2.0f)),     thumbSize };
    else
        thumbBounds = { thumbStartPosition, y, thumbSize, height };

    auto c = scrollbar.findColour (ScrollBar::ColourIds::thumbColourId);
    g.setColour (isMouseOver ? c.brighter (0.25f) : c);
    g.fillRect (thumbBounds.reduced (1).toFloat());
}

//==============================================================================================================================
// RangeTunerLookAndFeel:

void RangeTunerLookAndFeel::drawRotarySlider (Graphics& g, int x, int y, int width, int height, float sliderPos,
                                       const float rotaryStartAngle, const float rotaryEndAngle, Slider& slider)
{
    auto outline = slider.findColour (Slider::rotarySliderOutlineColourId);
    auto fill    = slider.findColour (Slider::rotarySliderFillColourId);

    auto bounds = juce::Rectangle<int> (x, y, width, height).toFloat().reduced (10);

    auto radius = jmin (bounds.getWidth(), bounds.getHeight()) / 2.0f;
    auto toAngle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
    auto lineW = jmin (8.0f, radius * 0.5f);
    auto arcRadius = radius - lineW * 0.5f;

    Path backgroundArc;
    backgroundArc.addCentredArc (bounds.getCentreX(),
                                 bounds.getCentreY(),
                                 arcRadius,
                                 arcRadius,
                                 0.0f,
                                 rotaryStartAngle,
                                 rotaryEndAngle,
                                 true);

    g.setColour (outline);
    g.strokePath (backgroundArc, PathStrokeType (1.5f /*lineW*/, PathStrokeType::curved, PathStrokeType::rounded));

    if (slider.isEnabled())
    {
        Path valueArc;
        valueArc.addCentredArc (bounds.getCentreX(),
                                bounds.getCentreY(),
                                arcRadius,
                                arcRadius,
                                0.0f,
                                rotaryStartAngle,
                                toAngle,
                                true);

        g.setColour (fill);
        g.strokePath (valueArc, PathStrokeType (1.5f /*lineW*/, PathStrokeType::curved, PathStrokeType::rounded));
    }

    auto thumbWidth = 6.0f;
    Point<float> thumbPoint (bounds.getCentreX() + arcRadius * std::cos (toAngle - MathConstants<float>::halfPi),
                             bounds.getCentreY() + arcRadius * std::sin (toAngle - MathConstants<float>::halfPi));

    g.setColour (slider.findColour (Slider::thumbColourId));
    g.fillEllipse (juce::Rectangle<float> (thumbWidth, thumbWidth).withCentre (thumbPoint));
}