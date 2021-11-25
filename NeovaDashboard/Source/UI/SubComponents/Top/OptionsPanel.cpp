/*
  ==============================================================================

    OptionsPanel.cpp
    Created: 18 Sep 2019 1:59:02pm
    Author:  Enhancia Dev

  ==============================================================================
*/

#include "OptionsPanel.h"

//==============================================================================
OptionsPanel::OptionsPanel (HubConfiguration& config, DashUpdater& updtr, UpgradeHandler& handler, ApplicationCommandManager& manager)
    : hubConfig (config), commandManager (manager), updater (updtr), upgradeHandler (handler)
{
    createButtons();
    options.addTab (new ContactPanel (*sendReportButton.get()), "About");
    options.addTab (new UpdateAndUpgradePanel (hubConfig, updater, upgradeHandler,
											   *upgradeButton.get(), *updateButton.get()), "Updates");
    options.addTab (new LegalPanel(), "Legal");
    //options.addTab (new LicensePanel(), "EULA");

	addAndMakeVisible (options);
    setUpdateTabAlertCount();
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

    paintProductInformations (g, area.removeFromTop (area.getHeight()/3).reduced (neova_dash::ui::MARGIN));
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

    else if (bttn == updateButton.get())
    {
        commandManager.invokeDirectly (neova_dash::commands::checkDashboardUpdate, true);
    }

    else if (bttn == sendReportButton.get())
    {
        commandManager.invokeDirectly (neova_dash::commands::openBugReportPanel, true);
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
    if (isVisible()) setUpdateTabAlertCount();
}

void OptionsPanel::update()
{
    if (!hubConfig.getHubIsConnected() || hubConfig.getHubIsCompatibleInt() > 0
                                       || (upgradeHandler.getHubReleaseVersion() <= hubConfig.getHubFirmwareVersionUint16()
                                             && upgradeHandler.getRingReleaseVersion() <= hubConfig.getRingFirmwareVersionUint16()))
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

    if (!updater.hasNewAvailableVersion())
    {
        updateButton->setInterceptsMouseClicks (false, false);
        updateButton->setOpaque (false);
        updateButton->setAlpha (0.5f);
    }
    else
    {
        updateButton->setInterceptsMouseClicks (true, false);
        updateButton->setAlpha (1.0f);
    }

    setUpdateTabAlertCount();
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
    upgradeButton->addListener (this);

    //Update Soft button
    updateButton = std::make_unique <TextButton> ("Check Updates");
    updateButton->addListener (this);

    //Send Report button
    sendReportButton = std::make_unique <TextButton> ("Send");
    sendReportButton->addListener (this);
}

void OptionsPanel::paintProductInformations(Graphics& g, juce::Rectangle<int> area)
{
  	using namespace neova_dash::ui;

    // Enhancia Logo
    auto enhanciaArea = area.removeFromLeft (area.getWidth()/2);
    g.drawImage (enhanciaLogo, enhanciaArea.toFloat(), RectanglePlacement::fillDestination);

    /*
    auto logoArea = enhanciaArea.removeFromTop (area.getHeight()/2)
                                .reduced (enhanciaArea.getWidth()/4, 0)
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
    g.drawText (String ("ENHANCIA "/* + String (CharPointer_UTF8 ("\xe2\x84\xa2"))*///), enhanciaArea.reduced (MARGIN*2),
    //               Justification::centredTop);*/

  	// Dash Text
  	auto dashTextArea = area.reduced (MARGIN*2, area.getHeight()/6);

  	g.setColour (neova_dash::colour::mainText);
    g.setFont(neova_dash::font::neovaFont.withHeight (19.2f).withExtraKerningFactor (0.4f));
  	g.drawText ("NEOVA", dashTextArea.removeFromTop (dashTextArea.getHeight()*2/5),
  								   Justification::centred);

    g.setFont(neova_dash::font::dashFontLight.withHeight (13.5f).withExtraKerningFactor (0.5f));
    g.drawText ("DASH TESTS", dashTextArea.removeFromTop (dashTextArea.getHeight()*2/3),
                                         //.withTrimmedTop (10),
                     Justification::centred);

  	g.setColour (neova_dash::colour::subText);
  	g.setFont (neova_dash::font::dashFont.withHeight (12));
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

void OptionsPanel::setUpdateTabAlertCount()
{
    int alertCount = 0;

    if ((upgradeHandler.getHubReleaseVersion() > hubConfig.getHubFirmwareVersionUint16())
            && hubConfig.getHubIsConnected())
    {
        alertCount++; // Hub upgrade
    }
    if ((upgradeHandler.getRingReleaseVersion() > hubConfig.getRingFirmwareVersionUint16())
            && hubConfig.getRingIsConnected())
    {
        alertCount++; // Ring upgrade
    }

    if (updater.hasNewAvailableVersion())
    {
        alertCount++; // soft update
    }
    
    options.setTabAlertCount ("Updates", alertCount);
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

    g.setColour (neova_dash::colour::topPanelBackground.brighter (0.03f));
    g.fillRect (panelArea);

    auto area = tabsArea;

    if (!tabs.isEmpty())
    {
        for (int i =0; i < tabs.size(); i++)
        {
            auto tabArea = tabs[i]->button->getBounds();

            if (i == selectedTab)
            {
                g.setColour (neova_dash::colour::topPanelBackground.brighter (0.03f));
                g.fillRect (tabArea);

                g.setColour (neova_dash::colour::mainText);
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

            if (tabs[i]->alertCount > 0)
            {
                auto alertArea = juce::Rectangle<int> (15, 15).withCentre ({tabArea.getRight() - 8 - neova_dash::ui::MARGIN * 2,
                                                                            tabArea.getCentreY()});
                
                g.setColour (neova_dash::colour::notificationBubble);
                g.fillEllipse (alertArea.toFloat());
             
                g.setColour (neova_dash::colour::mainText);
                g.setFont (neova_dash::font::dashFontBold.withHeight (13.0f));
                g.drawText (String (tabs[i]->alertCount), alertArea, Justification::centred);
            }
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
            tab->button->setBounds (tab == tabs.getLast() ? tabsAreaTemp
                                                          : (style == tabsVertical) ? tabsAreaTemp.removeFromTop (tabHeightOrWidth)
                                                                                    : tabsAreaTemp.removeFromLeft (tabHeightOrWidth));

            if (tab != tabs.getLast())
            {
                (style == tabsVertical) ? tabsAreaTemp.removeFromTop (MARGIN)
                                        : tabsAreaTemp.removeFromLeft (MARGIN);
            }

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
        findChildWithID ("panel0")->setVisible (true);
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

void OptionsPanel::TabbedOptions::setTabAlertCount (const int tabNumber, const int alertCount)
{
    if (tabNumber < 0 || tabNumber > tabs.size()) return;

    tabs[tabNumber]->alertCount = alertCount;
    repaint();
}

void OptionsPanel::TabbedOptions::setTabAlertCount (const String tabName, const int alertCount)
{
    for (int i = 0; i < tabs.size(); i++)
    {
        if (tabs[i]->name == tabName)
        {
            setTabAlertCount (i, alertCount);
            return;
        }
    }
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

//==============================================================================
OptionsPanel::TabbedOptions::Tab* OptionsPanel::TabbedOptions::getTabByName (const String tabNameToSearch)
{
    for (auto tab : tabs)
    {
        if (tab->name == tabNameToSearch) return tab;
    }

    return nullptr;
}


// ====================== ContactPanel =========================

ContactPanel::ContactPanel (TextButton& bugReportButton) : sendReportButton (bugReportButton)
{
    //Contact button
    contactButton = std::make_unique <TextButton> ("Contact");
    addAndMakeVisible (*contactButton);
    contactButton->addListener (this);
    
    //Contact button
    viewNotesButton = std::make_unique <TextButton> ("View");
    addAndMakeVisible (*viewNotesButton);
    viewNotesButton->addListener (this);

    addAndMakeVisible (sendReportButton);
}

ContactPanel::~ContactPanel()
{
}

void ContactPanel::paint (Graphics& g)
{
    g.setFont (neova_dash::font::dashFont.withHeight (14));
    g.setColour (neova_dash::colour::subText);

    g.drawText ("Contact Enhancia :", contactArea, Justification::centredRight);
    g.drawText ("Send Bug Report :", reportArea, Justification::centredRight);
    g.drawText ("Dashboard Release Notes :", viewNotesArea, Justification::centredRight);
}

void ContactPanel::resized()
{
    auto area = getLocalBounds().reduced (6*neova_dash::ui::MARGIN,
                                          2*neova_dash::ui::MARGIN)
                                .withTrimmedTop (neova_dash::ui::MARGIN);

    //aboutArea =     area.removeFromTop (20);
    contactArea =   area.removeFromTop (area.getHeight()/3);
    reportArea =    area.removeFromTop (area.getHeight()/2);
    viewNotesArea = area;

    auto resizeButtonToArea = [this](juce::Rectangle<int>& areaRef, Button& bttnRef)
    {
        bttnRef.setBounds (areaRef.removeFromRight (areaRef.getWidth()/2 + 2*neova_dash::ui::MARGIN)
                                  .withTrimmedLeft (4*neova_dash::ui::MARGIN)
                                  .withWidth (110)
                                  .withSizeKeepingCentre (110, 22));
    };

    resizeButtonToArea (contactArea, *contactButton);
    resizeButtonToArea (reportArea, sendReportButton);
    resizeButtonToArea (viewNotesArea, *viewNotesButton);
}

void ContactPanel::buttonClicked (Button* bttn)
{
    if (bttn == contactButton.get())
    {
        URL ("https://www.enhancia-music.com/contact/").launchInDefaultBrowser();
    }

    else if (bttn == viewNotesButton.get())
    {
        URL ("https://www.enhancia-music.com/dashboard-release-notes/").launchInDefaultBrowser();
    }
}

// ====================== UpdateAndUpgradePanel =========================

UpdateAndUpgradePanel::UpdateAndUpgradePanel (HubConfiguration& hubConfiguration, DashUpdater& updtr, UpgradeHandler& handler,
                                              TextButton& firmButton, TextButton& softButton)
  : hubConfig (hubConfiguration), upgradeButton (firmButton), updateButton (softButton), updater (updtr), upgradeHandler (handler)
{
    addAndMakeVisible (upgradeButton);
    addAndMakeVisible (updateButton);
}

UpdateAndUpgradePanel::~UpdateAndUpgradePanel()
{
    removeAllChildren();
}

void UpdateAndUpgradePanel::paint (Graphics& g)
{
    paintFirmwareArea (g);
    paintSoftwareArea (g);

    g.setColour (neova_dash::colour::subText);
    g.drawVerticalLine (softwareArea.getX(), getHeight() * 1.0 / 8,
                                             getHeight() * 6.0 / 8);
}

void UpdateAndUpgradePanel::paintFirmwareArea (Graphics& g)
{
    auto firmAreaTemp = firmwareArea.reduced (neova_dash::ui::MARGIN*2);

    auto ringArea = firmAreaTemp.removeFromLeft (firmAreaTemp.getWidth()/2);
    auto hubArea = firmAreaTemp;     


    const int notificationRadius = 10;
    g.setColour (neova_dash::colour::notificationBubble);
    if ((upgradeHandler.getHubReleaseVersion() > hubConfig.getHubFirmwareVersionUint16()) && hubConfig.getHubIsConnected())
    {
        g.fillEllipse (hubArea.withTrimmedTop (hubArea.getHeight()*2/3)
                               .withTrimmedLeft (hubArea.getWidth()*2/3)
                               .withSizeKeepingCentre (notificationRadius, notificationRadius)
                               .withY (hubArea.getY() + hubArea.getHeight()*2/3 + 6 - notificationRadius/2).toFloat());
    }
    
    if ((upgradeHandler.getRingReleaseVersion() > hubConfig.getRingFirmwareVersionUint16()) && hubConfig.getRingIsConnected())
    {
        g.fillEllipse (ringArea.withTrimmedTop (ringArea.getHeight()*2/3)
                               .withTrimmedLeft (ringArea.getWidth()*2/3)
                               .withSizeKeepingCentre (notificationRadius, notificationRadius)
                               .withY (ringArea.getY() + ringArea.getHeight()*2/3 + 6 - notificationRadius/2).toFloat());
    }

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

void UpdateAndUpgradePanel::paintSoftwareArea (Graphics& g)
{
    auto softAreaTemp = softwareArea.reduced (neova_dash::ui::MARGIN*2);

    const int notificationRadius = 10;
    g.setColour (neova_dash::colour::notificationBubble);
    if (updater.hasNewAvailableVersion())
    {
        g.fillEllipse (softAreaTemp.withTrimmedTop (softAreaTemp.getHeight()*2/3)
                                   .withTrimmedLeft (softAreaTemp.getWidth()*1/2)
                                   .withSizeKeepingCentre (notificationRadius, notificationRadius)
                                   .withY (softAreaTemp.getY() + softAreaTemp.getHeight()*2/3 + 6 - notificationRadius/2).toFloat());
    }

    g.setColour (neova_dash::colour::subText);
    g.setFont (neova_dash::font::dashFont.withHeight (15));
    g.drawFittedText ("v" + JUCEApplication::getInstance()->getApplicationVersion(),
                      softAreaTemp.removeFromBottom (softAreaTemp.getHeight()/3),
                      Justification::centredTop, 1);

    Path dashPath = neova_dash::path::createPath (neova_dash::path::dashIcon);

    dashPath.scaleToFit (softAreaTemp.reduced (softAreaTemp.getHeight()/4).getX(),
                         softAreaTemp.reduced (softAreaTemp.getHeight()/4).getY(),
                         softAreaTemp.reduced (softAreaTemp.getHeight()/4).getWidth(),
                         softAreaTemp.reduced (softAreaTemp.getHeight()/4).getHeight(),
                         true);

    g.strokePath (dashPath, {1.0f, PathStrokeType::curved});
}

void UpdateAndUpgradePanel::resized()
{
    auto area = getLocalBounds().reduced (0, neova_dash::ui::MARGIN);

    firmwareArea = area.removeFromLeft (area.getWidth()/2); 
    softwareArea = area;

    upgradeButton.setBounds (firmwareArea.withSizeKeepingCentre (upgradeButton.getBestWidthForHeight (30), 30)
                                         .withBottomY (getLocalBounds().getBottom() - neova_dash::ui::MARGIN));

    updateButton.setBounds (softwareArea.withSizeKeepingCentre (upgradeButton.getBestWidthForHeight (30), 30)
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
    licenseTextEdit.setFont (neova_dash::font::dashFontNorms.withHeight (13.0f));
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