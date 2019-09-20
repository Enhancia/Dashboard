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
    createButtons();
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

    auto area = optionsArea.reduced (neova_dash::ui::MARGIN*2);

    paintProductInformations (g, area.removeFromTop (area.getHeight()/2)
    										                    .reduced (neova_dash::ui::MARGIN));

    g.setColour (neova_dash::colour::subText);
    g.drawHorizontalLine (area.getY(), float (optionsArea.getX() + neova_dash::ui::MARGIN*7),
                                       float (optionsArea.getRight() - neova_dash::ui::MARGIN*7));

    paintFirmUpdateArea (g, area.removeFromTop (area.getHeight()).reduced (neova_dash::ui::MARGIN));
}

void OptionsPanel::resized()
{
    using namespace neova_dash::ui;
    
    optionsArea = getBounds().reduced (getWidth()/5, getHeight()/4);

    #if JUCE_WINDOWS
    closeButton->setBounds (juce::Rectangle<int> (25, 25).withRightX (optionsArea.getRight() - MARGIN_SMALL)
                                                                     .withY (optionsArea.getY() + MARGIN_SMALL));
    #elif JUCE_MAC
    closeButton->setBounds (juce::Rectangle<int> (25, 25).withPosition (optionsArea.getTopLeft()
    																			   .translated (MARGIN_SMALL,
    																			   				MARGIN_SMALL)));
    #endif

    auto area = optionsArea.reduced (neova_dash::ui::MARGIN*2);
    area.removeFromTop (area.getHeight()/2);
    area.reduce (neova_dash::ui::MARGIN, neova_dash::ui::MARGIN);

    updateFirmwareButton->setBounds (area.withTrimmedRight (area.getWidth()*2/3)
                                         .withSizeKeepingCentre (100, 35));
}

//==============================================================================
void OptionsPanel::buttonClicked (Button* bttn)
{
    if (bttn == closeButton.get())
    {
        setVisible (false);
    }

    else if (bttn == updateFirmwareButton.get())
    {
        commandManager.invokeDirectly (neova_dash::commands::upgradeHub, true);
        commandManager.invokeDirectly (neova_dash::commands::upgradeRing, true);
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

void OptionsPanel::createButtons()
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

    //Update Firm button
    updateFirmwareButton = std::make_unique <TextButton> ("Update Firmware");
    addAndMakeVisible (*updateFirmwareButton);
    updateFirmwareButton->addListener (this);
}

void OptionsPanel::paintProductInformations (Graphics& g, Rectangle<int> area)
{
  	using namespace neova_dash::ui;

    // Enhancia Logo
    auto enhanciaArea = area.removeFromLeft (area.getWidth()/2);

    auto logoArea = enhanciaArea.removeFromTop (area.getHeight()*2/3)
                                .withTrimmedTop (MARGIN*3)
                                .toFloat();

    Path logo = neova_dash::path::createPath (neova_dash::path::enhanciaLogo);
    logo.scaleToFit (logoArea.getX(), logoArea.getY(),
                     logoArea.getWidth(), logoArea.getHeight(), true);

    g.setColour (neova_dash::colour::mainText);
  	g.fillPath (logo);

    //Enhancia Text
    g.setColour (neova_dash::colour::mainText);
    g.setFont (neova_dash::font::enhanciaFont.withHeight (25).withExtraKerningFactor (0.2f));
    g.drawText (String ("ENHANCIA "/* + String (CharPointer_UTF8 ("\xe2\x84\xa2"))*/), enhanciaArea.reduced (MARGIN*2),
                   Justification::centred);

  	// Dash Text
  	auto dashTextArea = area.reduced (MARGIN*2, area.getHeight()/4);

  	g.setColour (neova_dash::colour::mainText);
  	g.setFont (neova_dash::font::dashFont.withHeight (25));
  	g.drawText ("Neova DASHBOARD", dashTextArea.removeFromTop (dashTextArea.getHeight()/2),
  								   Justification::bottomLeft);

  	g.setColour (neova_dash::colour::subText);
  	g.setFont (neova_dash::font::dashFont.withHeight (13));
  	g.drawText (String ("v " + JUCEApplication::getInstance()->getApplicationVersion()),
  		        dashTextArea, Justification::topLeft);
}

void OptionsPanel::paintFirmUpdateArea (Graphics& g, Rectangle<int> area)
{
    g.setColour (neova_dash::colour::subText);
    g.setFont (neova_dash::font::dashFont.withHeight (12));
    g.drawText ("Current firmware version : ",
                area.withTrimmedLeft (area.getWidth()/3),
                Justification::centredLeft);
}