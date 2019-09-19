/*
  ==============================================================================

    OptionsPanel.cpp
    Created: 18 Sep 2019 1:59:02pm
    Author:  Enhancia Dev

  ==============================================================================
*/

#include "OptionsPanel.h"

//==============================================================================
OptionsPanel::OptionsPanel (HubConfiguration& config, ApplicationCommandManager& manager)
    : hubConfig (config), commandManager (manager)
{
    createButton();
}

OptionsPanel::~OptionsPanel()
{
    closeButton = nullptr;
}

//==============================================================================
void OptionsPanel::paint (Graphics& g)
{
	using namespace neova_dash::colour;

    ColourGradient gradFill (topPanelBackground.overlaidWith (Colour (0x10000000)),
                             float (optionsArea.getCentreX()),
                             float (optionsArea.getBottom()),
                             topPanelBackground,
                             float (optionsArea.getCentreX()),
                             float (optionsArea.getY()),
                             true);
    gradFill.addColour (0.7, topPanelBackground.overlaidWith (Colour (0x10000000)));

    // transparent area
    g.setColour (topPanelTransparentArea);
    g.fillRect (getBounds());
    
    // options panel area
    g.setGradientFill (gradFill);
    g.fillRoundedRectangle (optionsArea.toFloat(), 10.0f);
    
    // options panel outline
    auto gradOut = ColourGradient::horizontal (Colour (0x10ffffff),
                                               float(optionsArea.getX()), 
                                               Colour (0x10ffffff),
                                               float(optionsArea.getRight()));
    gradOut.addColour (0.5, Colour (0x50ffffff));

    g.setGradientFill (gradOut);
    g.drawRoundedRectangle (optionsArea.toFloat(), 10.0f, 1.0f);

    paintProductInformations (g, optionsArea.withTrimmedBottom (optionsArea.getHeight()*2/3)
    										.reduced (neova_dash::ui::MARGIN));
}

void OptionsPanel::resized()
{
    using namespace neova_dash::ui;
    
    optionsArea = getBounds().reduced (getWidth()/5, getHeight()/6);

  #if JUCE_WINDOWS
    closeButton->setBounds (juce::Rectangle<int> (25, 25).withRightX (optionsArea.getRight() - MARGIN_SMALL)
                                                                     .withY (optionsArea.getY() + MARGIN_SMALL));
  #elif JUCE_MAC
    closeButton->setBounds (juce::Rectangle<int> (25, 25).withPosition (optionsArea.getTopLeft()
    																			   .translated (MARGIN_SMALL,
    																			   				MARGIN_SMALL)));
  #endif
}

//==============================================================================
void OptionsPanel::buttonClicked (Button* bttn)
{
    if (bttn == closeButton.get())
    {
        setVisible (false);
    }
}

void OptionsPanel::mouseUp (const MouseEvent& event)
{
    if (!optionsArea.contains (event.getPosition()))
    {
        setVisible (false);
    }
}

void OptionsPanel::visibilityChanged()
{
}

void OptionsPanel::createButton()
{
	// Close button
    closeButton = std::make_unique <DashShapeButton> ("Close Options Button",
                                                       Colour(0),
                                                       neova_dash::colour::mainText);
    addAndMakeVisible (*closeButton);

    Path p;
    p.startNewSubPath (0, 0);
    p.lineTo (3*neova_dash::ui::MARGIN, 3*neova_dash::ui::MARGIN);
    p.startNewSubPath (0, 3*neova_dash::ui::MARGIN);
    p.lineTo (3*neova_dash::ui::MARGIN, 0);

    closeButton->setShape (p, false, true, false);
	closeButton->addListener (this);
}

void OptionsPanel::paintProductInformations (Graphics& g, Rectangle<int> area)
{
	using namespace neova_dash::ui;

	g.drawImage (logoImage, area.removeFromLeft (area.getWidth()/2)
								.reduced (MARGIN*6, MARGIN*3)
								.toFloat(),
						    RectanglePlacement::centred);

	// Dash Text
	auto dashTextArea = area.removeFromTop (area.getHeight()/2).reduced (MARGIN*2);

	g.setColour (neova_dash::colour::mainText);
	g.setFont (Font().boldened().withHeight (25));
	g.drawText ("Neova DashBoard", dashTextArea.removeFromTop (dashTextArea.getHeight()/2),
								   Justification::centredLeft);

	g.setColour (neova_dash::colour::subText);
	g.setFont (Font().withHeight (13));
	g.drawText (String ("v " + JUCEApplication::getInstance()->getApplicationVersion()),
		        dashTextArea, Justification::centredLeft);

	//Enhancia Text
	g.setColour (neova_dash::colour::mainText);
	g.setFont (Font().withHeight (20));
	g.drawText (String ("Enhancia " + String (CharPointer_UTF8 ("\xe2\x84\xa2"))), area.reduced (MARGIN*2),
							   Justification::centredLeft);

}