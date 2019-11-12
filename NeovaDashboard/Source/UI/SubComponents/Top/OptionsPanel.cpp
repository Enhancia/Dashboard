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
    TRACE_IN;

    createButtons();
    createMidiBox();
}

OptionsPanel::~OptionsPanel()
{
    TRACE_IN;

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

    g.setFont (neova_dash::font::dashFont.withHeight (15.0f));
    g.drawText ("MIDI Channel :", area.removeFromTop (area.getHeight()/3)
                                      .withTrimmedRight (area.getWidth()/2)
                                      .reduced (neova_dash::ui::MARGIN),
                                  Justification::centred);

    paintFirmUpdateArea (g, area.removeFromTop (area.getHeight()/2));

    g.setFont (neova_dash::font::dashFont.withHeight (15.0f));
    g.drawText ("Contact Enhancia :", area.withTrimmedRight (area.getWidth()/2)
                        .reduced (neova_dash::ui::MARGIN),
                    Justification::centred);
    
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

    int buttonW = jmin (90, area.getWidth()/4 - neova_dash::ui::MARGIN_SMALL*2);

    auto midiChannelArea = area.removeFromTop (area.getHeight()/3)
                               .withTrimmedLeft (area.getWidth()/2);

    midiChannelBox->setBounds (midiChannelArea.removeFromLeft (midiChannelArea.getWidth()/2)
                                              .withSizeKeepingCentre (buttonW, 30));

    auto firmArea = area.removeFromTop (area.getHeight()/2)
                        .withTrimmedLeft (area.getWidth()/2);


    updateFirmwareButton->setBounds (firmArea.withTrimmedLeft (firmArea.getWidth()/2)
                                             .withSizeKeepingCentre (buttonW, 30));

    auto contactArea = area.withTrimmedLeft (area.getWidth()/2);

    contactButton->setBounds (contactArea.removeFromLeft (contactArea.getWidth()/2)
                                         .withSizeKeepingCentre (buttonW, 30));
    sendReportButton->setBounds (contactArea.withSizeKeepingCentre (buttonW, 30));
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

    else if (bttn == sendReportButton.get())
    {
        if (auto* dashLogger = dynamic_cast<FileLogger*> (Logger::getCurrentLogger()))
        {
            String fullLog = dashLogger->getLogFile().loadFileAsString().removeCharacters ("\n");
            
            /* Only keeps the last 3 entries of the log: the one that had an issue, 
               the one used to send the report, and one more to cover cases where the plugin
               has to be checked (For instance the plugin check when launching Ableton..)
               If too long, keeps the 6000 last characters.. */
            int startIndex = jmax (fullLog.upToLastOccurrenceOf("Neova Dashboard Log", false, false)
                                          .upToLastOccurrenceOf("Neova Dashboard Log", false, false)
                                          .lastIndexOf("Neova Dashboard Log"),
                                   fullLog.length() - 6000);
      
          #if JUCE_WINDOWS                    
            String mail_str ("mailto:damien.leboulaire@enhancia.co"
                             "?Subject=[Neova Dashboard Report]"
                             "&cc=alex.levacher@enhancia.co"
                             "&body=" + fullLog.substring (startIndex));
            LPCSTR mail_lpc = mail_str.toUTF8();

            ShellExecute (NULL, "open", mail_lpc,
                          "", "", SW_SHOWNORMAL);

          #elif JUCE_MAC                    
            String mail_str ("open mailto:damien.leboulaire@enhancia.co"
                             "?Subject=\"[Neova Dashboard Report]\""
                             "\\&cc=alex.levacher@enhancia.co"
                             "\\&body=\"" + fullLog.substring (startIndex) + "\"");
        
            system (mail_str.toUTF8());
          #endif
        }
    }

    else if (bttn == contactButton.get())
    {
        URL ("https://www.enhancia.co/contact").launchInDefaultBrowser();
    }
}

void OptionsPanel::comboBoxChanged (ComboBox* box)
{
    if (box == midiChannelBox.get())
    {
        hubConfig.setMidiChannel (midiChannelBox->getSelectedId() - 1);
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

void OptionsPanel::update()
{
    midiChannelBox->setSelectedId (hubConfig.getMidiChannel()+1);

    repaint();
}

void OptionsPanel::setMidiBoxActive (bool shouldBeActive)
{
    midiChannelBox->setInterceptsMouseClicks (shouldBeActive, false);
    midiChannelBox->setAlpha (shouldBeActive ? 1.0f : 0.6f);
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

    //Contact button
    contactButton = std::make_unique <TextButton> ("Contact Us");
    addAndMakeVisible (*contactButton);
    contactButton->addListener (this);

    //Send Report button
    sendReportButton = std::make_unique <TextButton> ("Send Report");
    addAndMakeVisible (*sendReportButton);
    sendReportButton->addListener (this);
}

void OptionsPanel::createMidiBox()
{
    midiChannelBox = std::make_unique <ComboBox> ("Midi Channel Box");
    addAndMakeVisible (*midiChannelBox);

    for (int chan=0; chan < 16; chan++)
    {
        midiChannelBox->addItem (String (chan+1), chan+1);
    }

    midiChannelBox->setSelectedId (hubConfig.getMidiChannel()+1);

    // ComboBox look
    midiChannelBox->setJustificationType (Justification::centred);
    midiChannelBox->setColour (ComboBox::outlineColourId, neova_dash::colour::subText);
    midiChannelBox->setColour (ComboBox::textColourId, neova_dash::colour::mainText);
    midiChannelBox->setColour (ComboBox::arrowColourId, neova_dash::colour::subText);

    midiChannelBox->addListener (this);
}

void OptionsPanel::paintProductInformations(Graphics& g, juce::Rectangle<int> area)
{
  	using namespace neova_dash::ui;

    // Enhancia Logo
    auto enhanciaArea = area.removeFromLeft (area.getWidth()/2);

    auto logoArea = enhanciaArea.removeFromTop (area.getHeight()/2)
                                .reduced (enhanciaArea.getWidth()/3, MARGIN)
                                .withTrimmedTop (enhanciaArea.getHeight()/6)
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
                   Justification::centredTop);

  	// Dash Text
  	auto dashTextArea = area.reduced (MARGIN*2, area.getHeight()/4);

  	g.setColour (neova_dash::colour::mainText);
  	g.setFont (neova_dash::font::dashFont.withHeight (20));
  	g.drawText ("Neova DASHBOARD", dashTextArea.removeFromTop (dashTextArea.getHeight()/2),
  								   Justification::bottomLeft);

  	g.setColour (neova_dash::colour::subText);
  	g.setFont (neova_dash::font::dashFont.withHeight (13));
  	g.drawText (String ("v " + JUCEApplication::getInstance()->getApplicationVersion()),
  		        dashTextArea, Justification::topLeft);
}

void OptionsPanel::paintFirmUpdateArea (Graphics& g, juce::Rectangle<int> area)
{
    g.setColour (neova_dash::colour::subText);
    g.setFont (neova_dash::font::dashFont.withHeight (15));

    g.drawText ("Current firmware version : ",
                area.removeFromLeft (area.getWidth()/2),
                Justification::centred);

    g.setFont (neova_dash::font::dashFont.withHeight (12));
    g.drawFittedText (hubConfig.getFirmwareVersionString(),
                      area.removeFromLeft (area.getWidth()/2),
                      Justification::centred, 2);
}