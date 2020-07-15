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
    options.addTab (new GeneralPanel(), "General");
    options.addTab (new FirmwarePanel (hubConfig, *upgradeButton.get()), "Firmware");
    options.addTab (new LegalPanel(), "Legal");
    options.addTab (new LicensePanel(), "EULA");
    options.switchToTab (0);
	  addAndMakeVisible (options);
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

    paintProductInformations (g, area.removeFromTop (area.getHeight()/3).reduced (neova_dash::ui::MARGIN));
    
    /*
    paintLegalAndRegulatoryArea (g, area.removeFromBottom (area.getHeight()/3)
                                            .reduced (neova_dash::ui::MARGIN*3,
                                                      neova_dash::ui::MARGIN));

    area.reduce (neova_dash::ui::MARGIN, neova_dash::ui::MARGIN);
    
    g.setColour (neova_dash::colour::subText);
    g.drawHorizontalLine (area.getY(), float (optionsArea.getX() + neova_dash::ui::MARGIN*7),
                                       float (optionsArea.getRight() - neova_dash::ui::MARGIN*7));

    g.drawHorizontalLine (area.getBottom(), float (optionsArea.getX() + neova_dash::ui::MARGIN*7),
                                       float (optionsArea.getRight() - neova_dash::ui::MARGIN*7));

    paintFirmUpdateArea (g, area.removeFromTop (area.getHeight()/2));

    g.setFont (neova_dash::font::dashFont.withHeight (15.0f));
    g.drawText ("Contact Enhancia :", area.withTrimmedRight (area.getWidth()/2)
                        .reduced (neova_dash::ui::MARGIN),
                    Justification::centred);*/
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
    area.removeFromTop (area.getHeight()/3);
    //area.removeFromBottom (area.getHeight()/3);

    area.reduce (neova_dash::ui::MARGIN, neova_dash::ui::MARGIN);

    options.setBounds (area);

    /*
    int buttonW = jmin (90, area.getWidth()/4 - neova_dash::ui::MARGIN_SMALL*2);

    auto firmArea = area.removeFromTop (area.getHeight()/2)
                        .withTrimmedLeft (area.getWidth()/2);


    upgradeButton->setBounds (firmArea.withTrimmedLeft (firmArea.getWidth()/2)
                                             .withSizeKeepingCentre (buttonW, 30));

    auto contactArea = area.withTrimmedLeft (area.getWidth()/2);

    contactButton->setBounds (contactArea.removeFromLeft (contactArea.getWidth()/2)
                                         .withSizeKeepingCentre (buttonW, 30));
    sendReportButton->setBounds (contactArea.withSizeKeepingCentre (buttonW, 30));*/
}

//==============================================================================
void OptionsPanel::buttonClicked (Button* bttn)
{
    if (bttn == closeButton.get())
    {
        setVisible (false);
    }

    else if (bttn == upgradeButton.get())
    {
        commandManager.invokeDirectly (neova_dash::commands::upgradeHub, true);
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
    if (!hubConfig.getHubIsConnected() || hubConfig.getHubIsCompatibleInt() > 0)
    {
        upgradeButton->setInterceptsMouseClicks (false, false);
        upgradeButton->setOpaque (false);
        upgradeButton->setAlpha (0.5f);
    }
    else
    {
        upgradeButton->setInterceptsMouseClicks (true, false);
        upgradeButton->setAlpha (1.0f);
    }
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
    upgradeButton = std::make_unique <TextButton> ("Check Upgrades");
    //addAndMakeVisible (*upgradeButton);
    upgradeButton->addListener (this);
}

void OptionsPanel::paintProductInformations(Graphics& g, juce::Rectangle<int> area)
{
  	using namespace neova_dash::ui;

    // Enhancia Logo
    auto enhanciaArea = area.removeFromLeft (area.getWidth()/2);

    auto logoArea = enhanciaArea.removeFromTop (area.getHeight()/2)
                                .reduced (enhanciaArea.getWidth()/4, 0)
                                .withTrimmedTop (enhanciaArea.getHeight()/6)
                                .toFloat();
                         
    Path logo = neova_dash::path::createPath (neova_dash::path::enhanciaLogo);
    logo.scaleToFit (logoArea.getX(), logoArea.getY(),
                     logoArea.getWidth(), logoArea.getHeight(), true);
    g.drawImage (enhanciaLogo, logoArea.toFloat(), RectanglePlacement::fillDestination);

    g.setColour (neova_dash::colour::mainText);
  	//g.fillPath (logo);

    //Enhancia Text
    g.setColour (neova_dash::colour::mainText);
    g.setFont (neova_dash::font::enhanciaFont.withHeight (25).withExtraKerningFactor (0.2f));
    g.drawText (String ("ENHANCIA "/* + String (CharPointer_UTF8 ("\xe2\x84\xa2"))*/), enhanciaArea.reduced (MARGIN*2),
                   Justification::centredTop);

  	// Dash Text
  	auto dashTextArea = area.reduced (MARGIN*2, area.getHeight()/6);

  	g.setColour (neova_dash::colour::mainText);
    g.setFont(neova_dash::font::neovaFont.withHeight (22.5f).withExtraKerningFactor (0.33f));
  	g.drawText ("NEOVA", dashTextArea.removeFromTop (dashTextArea.getHeight()*2/5),
  								   Justification::centredBottom);

    g.setFont(neova_dash::font::dashFont.withHeight (19.5f).withExtraKerningFactor (0.26f));
    g.drawText ("DASHBOARD", dashTextArea.removeFromTop (dashTextArea.getHeight()*2/3),
                     Justification::centred);

  	g.setColour (neova_dash::colour::subText);
  	g.setFont (neova_dash::font::dashFont.withHeight (13));
  	g.drawText (String ("v " + JUCEApplication::getInstance()->getApplicationVersion()),
  		        dashTextArea, Justification::centredBottom);
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

void OptionsPanel::paintLegalAndRegulatoryArea (Graphics& g, juce::Rectangle<int> area)
{
    g.setColour (neova_dash::colour::subText);
    g.setFont (neova_dash::font::dashFont.withHeight (13));

    g.drawText ("LEGAL AND REGULATORY : ", area,
                Justification::centredTop);

    g.setFont (neova_dash::font::dashFont.withHeight (13));
    g.drawFittedText ("\n\n\nRING :\n\n"
                      "FCC ID :    2AUJX-R1\n"
                      "IC :        25446-R1\n"
                      "HVIN :      NEOVA-R1",
                      area.removeFromLeft (area.getWidth()/2),
                      Justification::centred, 8);

    g.drawFittedText ("\n\n\nHUB :\n\n"
                      "FCC ID :    2AUJX-H1\n"
                      "IC :        25446-H1\n"
                      "HVIN :      NEOVA-H1",
                      area,
                      Justification::centred, 8);
}

// ====================== TabbedOptions =========================

OptionsPanel::TabbedOptions::TabbedOptions()
{
    setStyle (tabsHorizontal);
}

OptionsPanel::TabbedOptions::~TabbedOptions()
{
    removeAllChildren();
    tabs.clear();
}

//==============================================================================
void OptionsPanel::TabbedOptions::paint (Graphics& g)
{
    using namespace neova_dash::ui;

    auto area = tabsArea;

    if (!tabs.isEmpty())
    {
        //int tabHeight = (tabsArea.getHeight() - (tabs.size() - 1) * MARGIN)/tabs.size();
        int tabHeightOrWidth = (style == tabsVertical)
                                    ? jmin (30 , (area.getHeight() - (tabs.size() - 1) * MARGIN)/tabs.size())
                                    : (area.getWidth() - (tabs.size() - 1) * MARGIN)/tabs.size();

        for (int i =0; i < tabs.size(); i++)
        {
            auto tabArea = (style == tabsVertical) ? area.removeFromTop (tabHeightOrWidth)
                                                   : area.removeFromLeft (tabHeightOrWidth);

            if (i == selectedTab)
            {
                g.setColour (neova_dash::colour::mainText.withAlpha (0.1f));
                g.fillRect ((style == tabsVertical) ? tabArea.withWidth (3)
                                                    : tabArea.withHeight (3));
            }

            g.setColour (i == selectedTab ? neova_dash::colour::mainText
                                          : neova_dash::colour::subText);
            g.setFont (neova_dash::font::dashFont.withHeight (16.0f));
            g.drawText (tabs[i]->name,
                        (style == tabsVertical) ? tabArea.withTrimmedLeft (MARGIN + 5)
                                                : tabArea.withTrimmedTop (5),
                        (style == tabsVertical) ? Justification::centredLeft
                                                : Justification::centred,
                        true);

            (style == tabsVertical) ? area.removeFromTop (MARGIN)
                                    : area.removeFromLeft (MARGIN);
        }
    }
}

void OptionsPanel::TabbedOptions::resized()
{
    using namespace neova_dash::ui;

    auto area = getLocalBounds();
    
    if (style == tabsVertical)
    {
        panelArea = area.removeFromRight (getWidth()*4/5);
        tabsArea = area;
    }
    else
    {
        tabsArea = area.removeFromTop (30);
        panelArea = area;
    }

    if (!tabs.isEmpty())
    { 
        auto tabsAreaTemp = tabsArea;

        //int tabHeight = (tabsArea.getHeight() - (tabs.size() - 1) * MARGIN)/tabs.size();
        int tabHeightOrWidth = (style == tabsVertical)
                                    ? jmin (30 , (tabsAreaTemp.getHeight() - (tabs.size() - 1) * MARGIN)/tabs.size())
                                    : (tabsAreaTemp.getWidth() - (tabs.size() - 1) * MARGIN)/tabs.size();

        for (auto* tab : tabs)
        {
            tab->button->setBounds ((style == tabsVertical) ? tabsAreaTemp.removeFromTop (tabHeightOrWidth)
                                                            : tabsAreaTemp.removeFromLeft (tabHeightOrWidth));

            (style == tabsVertical) ? tabsAreaTemp.removeFromTop (MARGIN)
                                    : tabsAreaTemp.removeFromLeft (MARGIN);
            

            tab->panel->setBounds (panelArea);
        }
    }
}

//==============================================================================
void OptionsPanel::TabbedOptions::addTab (Component* panel, String tabName)
{
    tabs.add (new Tab (panel, tabName));
    addAndMakeVisible (tabs.getLast()->button.get());
    addChildAndSetID (tabs.getLast()->panel.get(), "panel" + String(tabs.size() - 1));
    findChildWithID ("panel" + String(tabs.size() - 1))->setVisible (false);
    tabs.getLast()->button->addListener (this);

    // Displays the first tab that is added
    if (tabs.size() == 1)
    {
        switchToTab (0);
    } 
}

void OptionsPanel::TabbedOptions::switchToTab (const int tabNumber)
{
    if (tabNumber < 0 || tabNumber >= tabs.size()) return;

    if (tabNumber == selectedTab) return;
    if (auto* pan = findChildWithID("panel" + String(selectedTab))) pan->setVisible(true);

    findChildWithID ("panel" + String(selectedTab))->setVisible (false);
    findChildWithID ("panel" + String(tabNumber)  )->setVisible (true);
    selectedTab = tabNumber;

    repaint();
}


Component* OptionsPanel::TabbedOptions::getComponentFromTab (const int tabNumber)
{
    if (tabNumber < 0 || tabNumber >= tabs.size()) return nullptr;

    return tabs[tabNumber]->panel.get();
}

Component* OptionsPanel::TabbedOptions::getComponentFromTab (const String tabName)
{
    for (auto* tab : tabs)
    {
        if (tab->name == tabName)
        {
            return tab->panel.get();
        }
    }

  return nullptr;
}

Component* OptionsPanel::TabbedOptions::getComponentFromSelectedTab()
{
    return getComponentFromTab (selectedTab);
}

//==============================================================================
void OptionsPanel::TabbedOptions::setStyle (TabbedPanelStyle newStyle)
{
    if (newStyle != style)
    {
        style = newStyle;
        resized();
        repaint();
    }
}

//==============================================================================
void OptionsPanel::TabbedOptions::buttonClicked (Button* bttn)
{
    for (int i =0; i < tabs.size(); i++)
    {
        if (bttn == tabs[i]->button.get())
        {
            switchToTab(i);
        }
    }
}

// ====================== GeneralPanel =========================

GeneralPanel::GeneralPanel()
{
    //Contact button
    contactButton = std::make_unique <TextButton> ("Contact");
    addAndMakeVisible (*contactButton);
    contactButton->addListener (this);

    //Send Report button
    sendReportButton = std::make_unique <TextButton> ("Send Report");
    addAndMakeVisible (*sendReportButton);
    sendReportButton->addListener (this);
}

GeneralPanel::~GeneralPanel()
{
}

void GeneralPanel::paint (Graphics& g)
{
    auto area = getLocalBounds().reduced (neova_dash::ui::MARGIN, neova_dash::ui::MARGIN);

    g.setColour (neova_dash::colour::mainText);
    g.setFont (neova_dash::font::dashFont.withHeight (13));

    g.drawFittedText ("Dashboard - Neova :"
                      "  - Developpers : Alex LEVACHER, Mathieu HERBELOT\n"
                      "                     - UI / UX     : Mario VIOLA,   Damien LE BOULAIRE\n\n\n",
                      area.removeFromTop (area.getHeight()/2),
                      Justification::topLeft, 5);
}

void GeneralPanel::resized()
{
	auto area = getLocalBounds();
    area.removeFromTop (area.getHeight()/2);

    contactButton->setBounds (area.removeFromLeft (area.getWidth()/2)
                                  .withSizeKeepingCentre (contactButton->getBestWidthForHeight (30), 30));
    
    sendReportButton->setBounds (area.withSizeKeepingCentre (contactButton->getBestWidthForHeight (30), 30));
}

void GeneralPanel::buttonClicked (Button* bttn)
{
    if (bttn == sendReportButton.get())
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

// ====================== FirmwarePanel =========================

FirmwarePanel::FirmwarePanel (HubConfiguration& hubConfiguration, TextButton& button)
  : hubConfig (hubConfiguration), upgradeButton (button)
{
    addAndMakeVisible (upgradeButton);
}

FirmwarePanel::~FirmwarePanel()
{
    removeAllChildren();
}

void FirmwarePanel::paint (Graphics& g)
{
    auto area = getLocalBounds().reduced (neova_dash::ui::MARGIN);

    auto ringArea = area.removeFromLeft (area.getWidth()/2);
    auto hubArea = area;

    g.setColour (neova_dash::colour::subText);
    g.setFont (neova_dash::font::dashFont.withHeight (15));
    g.drawFittedText (hubConfig.getRingFirmwareVersionString(),
                      ringArea.removeFromBottom (ringArea.getHeight()/3),
                      Justification::centredTop, 1);
    g.drawFittedText (hubConfig.getHubFirmwareVersionString(),
                      hubArea.removeFromBottom (hubArea.getHeight()/3),
                      Justification::centredTop, 1);

    Path ringPath = neova_dash::path::createPath (neova_dash::path::ring);
    Path hubPath = neova_dash::path::createPath (neova_dash::path::hub);

    ringPath.scaleToFit (ringArea.reduced (ringArea.getHeight()/4).getX(),
                         ringArea.reduced (ringArea.getHeight()/4).getY(),
                         ringArea.reduced (ringArea.getHeight()/4).getWidth(),
                         ringArea.reduced (ringArea.getHeight()/4).getHeight(),
                         true);

    hubPath.scaleToFit (hubArea.reduced (hubArea.getHeight()/8).getX(),
                        hubArea.reduced (hubArea.getHeight()/8).getY(),
                        hubArea.reduced (hubArea.getHeight()/8).getWidth(),
                        hubArea.reduced (hubArea.getHeight()/8).getHeight(),
                        true);

    g.strokePath (ringPath, {1.0f, PathStrokeType::curved});
    g.strokePath (hubPath, {1.0f, PathStrokeType::curved});
}

void FirmwarePanel::resized()
{
    upgradeButton.setBounds (getLocalBounds().withSizeKeepingCentre (upgradeButton.getBestWidthForHeight (30), 30)
                                             .withBottomY (getLocalBounds().getBottom() - neova_dash::ui::MARGIN));
}

// ====================== LegalPanel =========================

LegalPanel::LegalPanel() {}
LegalPanel::~LegalPanel() {}

void LegalPanel::paint (Graphics& g)
{
    auto area = getLocalBounds().reduced (neova_dash::ui::MARGIN);
    auto ringArea = area.removeFromLeft (area.getWidth()/2);
    auto hubArea = area;

    g.setColour (neova_dash::colour::subText);
    g.setFont (neova_dash::font::dashFont.withHeight (13));
    g.drawFittedText ("\nFCC ID :    2AUJX-R1\n"
                      "IC :             25446-R1\n"
                      "HVIN :      NEOVA-R1",
                      ringArea.removeFromBottom (ringArea.getHeight()/3),
                      Justification::centredTop, 4);
    g.drawFittedText ("\nFCC ID :    2AUJX-H1\n"
                      "IC :             25446-H1\n"
                      "HVIN :      NEOVA-H1",
                      hubArea.removeFromBottom (hubArea.getHeight()/3),
                      Justification::centredTop, 4);

    Path ringPath = neova_dash::path::createPath (neova_dash::path::ring);
    Path hubPath = neova_dash::path::createPath (neova_dash::path::hub);

    ringPath.scaleToFit (ringArea.reduced (ringArea.getHeight()/4).getX(),
                         ringArea.reduced (ringArea.getHeight()/4).getY(),
                         ringArea.reduced (ringArea.getHeight()/4).getWidth(),
                         ringArea.reduced (ringArea.getHeight()/4).getHeight(),
                         true);

    hubPath.scaleToFit (hubArea.reduced (hubArea.getHeight()/8).getX(),
                        hubArea.reduced (hubArea.getHeight()/8).getY(),
                        hubArea.reduced (hubArea.getHeight()/8).getWidth(),
                        hubArea.reduced (hubArea.getHeight()/8).getHeight(),
                        true);

    g.strokePath (ringPath, {1.0f, PathStrokeType::curved});
    g.strokePath (hubPath, {1.0f, PathStrokeType::curved});
}

void LegalPanel::resized() {}


// ====================== LicensePanel =========================

LicensePanel::LicensePanel()
{
    auto licenseTextString = String::createStringFromData (DashData::_200429__Enhancia_Software_Licence_txt, DashData::_200429__Enhancia_Software_Licence_txtSize);

    licenseTextEdit.setMultiLine (true);
    licenseTextEdit.setReadOnly (true);
    licenseTextEdit.setScrollbarsShown (true);
    licenseTextEdit.setFont (neova_dash::font::dashFont.withHeight (13.0f));
    licenseTextEdit.setJustification (Justification::horizontallyJustified);
    licenseTextEdit.setColour (TextEditor::backgroundColourId, neova_dash::colour::subText.withAlpha (0.05f));
    licenseTextEdit.setColour (TextEditor::textColourId, neova_dash::colour::mainText);
    licenseTextEdit.setText (licenseTextString, false);
    addAndMakeVisible (licenseTextEdit);
}

LicensePanel::~LicensePanel()
{
}

void LicensePanel::paint (Graphics& g)
{
}

void LicensePanel::resized()
{
    licenseTextEdit.setBounds (getLocalBounds().withTrimmedTop (neova_dash::ui::MARGIN));
}