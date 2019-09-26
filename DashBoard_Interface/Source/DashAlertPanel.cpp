/*
  ==============================================================================

    DashAlertPanel.cpp
    Created: 25 Sep 2019 4:35:26pm
    Author:  Enhancia Dev

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "DashAlertPanel.h"

//==============================================================================
DashAlertPanel::DashAlertPanel (const String& title, const String& message,
                                                     const String& buttonText)
{
    createAndAddTextEditor (message);
    createAndAddLabel (title);
    createAndAddCloseButton (buttonText);
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

        closeButton->setBounds (area.removeFromBottom (buttonHeight)
                                    .withSizeKeepingCentre
                                        (closeButton->getBestWidthForHeight (buttonHeight),
                                         buttonHeight));
    }

    bodyText->setBounds (area.reduced (neova_dash::ui::MARGIN));
}

void DashAlertPanel::buttonClicked (Button* bttn)
{
    if (bttn == closeButton.get())
    {
        exitModalState (0);
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

void DashAlertPanel::createAndAddCloseButton (const String& buttonText)
{
    if (!buttonText.isEmpty()) showButton = true;

    closeButton.reset (new TextButton ("Close Button"));
    addAndMakeVisible (*closeButton);

    closeButton->setButtonText (buttonText);
    closeButton->addListener (this);
}