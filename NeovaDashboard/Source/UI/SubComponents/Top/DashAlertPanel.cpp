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
    g.setColour (topPanelBackground.withAlpha (0.85f));
    g.fillRoundedRectangle (panelArea.toFloat(), 10.0f);
    
    // options panel outline
    auto gradOut = ColourGradient::horizontal (Colour (0x10ffffff),
                                               0.0f, 
                                               Colour (0x10ffffff),
                                               float(getWidth()));
    gradOut.addColour (0.5, Colour (0x50ffffff));

    g.setGradientFill (gradOut);
    g.drawRoundedRectangle (panelArea.reduced (1).toFloat(), 10.0f, 1.0f);
}

void DashAlertPanel::resized()
{
    panelArea = getLocalBounds().withSizeKeepingCentre (getLocalBounds().getWidth()/3, getLocalBounds().getHeight()/3);
    
    auto area = panelArea.reduced (neova_dash::ui::MARGIN);

    titleLabel->setBounds (area.removeFromTop (area.getHeight()/5));

    if (showButton)
    {
        int buttonHeight = area.getHeight()/5;

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

    bodyText->setBounds (area.reduced (neova_dash::ui::MARGIN));
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