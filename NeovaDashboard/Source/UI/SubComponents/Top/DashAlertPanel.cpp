/*
  ==============================================================================

    DashAlertPanel.cpp
    Created: 25 Sep 2019 4:35:26pm
    Author:  Enhancia Dev

  ==============================================================================
*/

#include "DashAlertPanel.h"

//==============================================================================
DashAlertPanel::DashAlertPanel (const String& title, const String& message,
                                                     int returnValue,
                                                     const bool hasCloseButton,
                                                     const String& buttonText)
{
    modalReturnValue = returnValue;

    createAndAddTextEditor (message);
    createAndAddLabel (title);
    createAndAddButtons (buttonText, hasCloseButton);
}

DashAlertPanel::~DashAlertPanel()
{
}

void DashAlertPanel::paint (Graphics& g)
{
    using namespace neova_dash::colour;
    
    // options panel area
    g.setColour (topPanelBackground.withAlpha (0.95f));
    g.fillRoundedRectangle (panelArea.toFloat(), 10.0f);
    
    // options panel outline
    auto gradOut = ColourGradient::horizontal (Colour (0x10ffffff),
                                               float (panelArea.getX()), 
                                               Colour (0x10ffffff),
                                               float(panelArea.getRight()));
    gradOut.addColour (0.5, Colour (0x50ffffff));

    g.setGradientFill (gradOut);
    g.drawRoundedRectangle (panelArea.reduced (1).toFloat(), 10.0f, 1.0f);
}

void DashAlertPanel::resized()
{
    panelArea = getLocalBounds().withSizeKeepingCentre (getLocalBounds().getWidth()/2, getLocalBounds().getHeight()/2);
    
    auto area = panelArea.reduced (neova_dash::ui::MARGIN);

    titleLabel->setBounds (area.removeFromTop (area.getHeight()/5));

    if (showButton)
    {
        int buttonHeight = area.getHeight()/8;

        okButton->setBounds (area.removeFromBottom (buttonHeight)
                                    .withSizeKeepingCentre
                                        (okButton->getBestWidthForHeight (buttonHeight),
                                         buttonHeight));
    }

    #if JUCE_WINDOWS
    closeButton->setBounds (juce::Rectangle<int> (25, 25).withRightX (panelArea.getRight() - neova_dash::ui::MARGIN_SMALL)
                                                                     .withY (panelArea.getY() + neova_dash::ui::MARGIN_SMALL));
    #elif JUCE_MAC
    closeButton->setBounds (juce::Rectangle<int> (25, 25).withPosition (panelArea.getTopLeft()
                                                                                   .translated (neova_dash::ui::MARGIN_SMALL,
                                                                                                neova_dash::ui::MARGIN_SMALL)));
    #endif

    bodyText->setBounds (area.reduced (area.getWidth()/6, area.getHeight()/6));
}

void DashAlertPanel::buttonClicked (Button* bttn)
{
    if (bttn == closeButton.get())
    {
        exitModalState (0);
    }

    if (bttn == okButton.get())
    {
        exitModalState (modalReturnValue);
    }
}

void DashAlertPanel::createAndAddLabel (const String& textToSet)
{
    titleLabel.reset (new Label ("Title Label", textToSet));
    addAndMakeVisible (*titleLabel);

    titleLabel->setJustificationType (Justification::centred);
    titleLabel->setFont (neova_dash::font::dashFontBold.withHeight (20.0f).withExtraKerningFactor (0.03f));
}

void DashAlertPanel::createAndAddTextEditor (const String& textToSet)
{
    bodyText.reset (new Label ("Body Text", textToSet));
    addAndMakeVisible (*bodyText);

    bodyText->setJustificationType (Justification::centred);
}

void DashAlertPanel::createAndAddButtons (const String& buttonText, const bool addCloseButton)
{
    if (!buttonText.isEmpty()) showButton = true;

    okButton.reset (new TextButton ("Close Button"));
    addAndMakeVisible (*okButton);

    okButton->setButtonText (buttonText);
    okButton->addListener (this);

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

    closeButton->setVisible (addCloseButton);
}

DashAlertPanel* DashAlertPanel::createSpecificAlertPanel (SpecificReturnValue panelType)
{
    switch (panelType)
    {
        case outdatedFirmware:
            return new DashAlertPanel ("You Neova firmware is outdated!",
                                       "Please upgrade your Neova firmware "
                                       "to use it with this Dashboard Version.",
                                       int (panelType),
                                       true,
                                       "Upgrade Firmware");
        case noUploadQuitting:
            return new DashAlertPanel ("Upload changes to Neova?",
                                       "You have configuration changes that have not "
                                       "been uploaded to Neova.\n\nAre you sure you want to quit?",
                                       int (panelType),
                                       true,
                                       "Quit Anyways");
        default:
            return new DashAlertPanel ("Unknown Alert",
                                       "Something went wrong I guess?\nPlease contact Enhancia about your issue!",
                                       0,
                                       false,
                                       "Ok");
            return nullptr;
    }
}