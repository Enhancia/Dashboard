/*
  ==============================================================================

    DashBoardLookAndFeel.h
    Created: 12 Sep 2019 10:11:02am
    Author:  Enhancia Dev

  ==============================================================================
*/

#pragma once

#include "../../../JuceLibraryCode/JuceHeader.h"
#include "../../Common/DashCommon.h"

class DashBoardLookAndFeel : public LookAndFeel_V4
{
public:
    //==============================================================================
	  DashBoardLookAndFeel();
	  ~DashBoardLookAndFeel() override;
	
    //==============================================================================
	  void setColours();

    //==============================================================================
    void drawRotarySlider (Graphics &,
                           int x, int y, int width, int height,
                           float sliderPosProportional, float rotaryStartAngle, float rotaryEndAngle,
                           Slider &) override;
    
    void drawLinearSlider (Graphics&, int x, int y, int width, int height,
                           float sliderPos, float minSliderPos, float maxSliderPos,
                           Slider::SliderStyle, Slider&) override;
	  
    //==============================================================================
    int getDefaultScrollbarWidth() override;

    void drawScrollbar (Graphics& g, ScrollBar& scrollbar,
                        int x, int y,
                        int width, int height,
                        bool isScrollbarVertical,
                        int thumbStartPosition, int thumbSize,
                        bool isMouseOver, bool isMouseDown) override;

    //==============================================================================
    //Font getLabelFont (Label& lbl) override;
    Font getTextButtonFont (TextButton& bttn, int buttonHeight) override;
    Font getComboBoxFont (ComboBox& cb) override;
    
    //==============================================================================
    void drawButtonBackground (Graphics& g,
                               Button& buttonToDraw,
                               const Colour &   backgroundColour,
                               bool  shouldDrawButtonAsHighlighted,
                               bool  shouldDrawButtonAsDown ) override; 

    //==============================================================================
    //void drawPopupMenuBackground (Graphics&, int width, int height) override;
	  /*
    void drawPopupMenuItem (Graphics&, const juce::Rectangle<int>& area,
                            bool isSeparator, bool isActive, bool isHighlighted, bool isTicked, bool hasSubMenu,
                            const String& text, const String& shortcutKeyText,
                            const Drawable* icon, const Colour* textColour) override;
	  */
    
private:
	  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DashBoardLookAndFeel)
};


class RangeTunerLookAndFeel : public DashBoardLookAndFeel
{
public:
    RangeTunerLookAndFeel() {}
    ~RangeTunerLookAndFeel() override {}

    void drawRotarySlider (Graphics &,
                           int x, int y, int width, int height,
                           float sliderPosProportional, float rotaryStartAngle, float rotaryEndAngle,
                           Slider &) override;
                      
private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RangeTunerLookAndFeel)
};