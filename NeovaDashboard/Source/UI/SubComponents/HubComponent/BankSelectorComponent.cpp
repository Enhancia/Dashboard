/*
  ==============================================================================

    BankSelectorComponent.cpp
    Created: 17 Sep 2019 3:57:45pm
    Author:  Enhancia Dev

  ==============================================================================
*/

#include "BankSelectorComponent.h"

//==============================================================================
BankSelectorComponent::BankSelectorComponent (HubConfiguration& config, ApplicationCommandManager& manager)
    : hubConfig (config), commandManager (manager)
{
    createButtons();
}

BankSelectorComponent::~BankSelectorComponent()
{
}

void BankSelectorComponent::paint (Graphics& g)
{
    g.setColour (neova_dash::colour::mainText);
    g.setFont (neova_dash::font::dashFont.withHeight (15.0f));

    g.drawText (String ("Bank " + String (hubConfig.getSelectedPreset() + 1)),
                getLocalBounds().withSizeKeepingCentre (getWidth()/2, getHeight()),
                Justification::centred);
}

void BankSelectorComponent::resized()
{
    auto area = getLocalBounds();

    leftArrowButton->setBounds (area.removeFromLeft (20).reduced (0, 3));
    rightArrowButton->setBounds (area.removeFromRight (20).reduced (0, 3));
}

void BankSelectorComponent::buttonClicked (Button* bttn)
{
    if (bttn == leftArrowButton.get())
    {
        hubConfig.setPreset (hubConfig.getSelectedPreset() - 1);
    }
    else if (bttn == rightArrowButton.get())
    {
        hubConfig.setPreset (hubConfig.getSelectedPreset() + 1);
    }

    commandManager.invokeDirectly (neova_dash::commands::updateDashInterface, true);
}

void BankSelectorComponent::mouseUp (const MouseEvent& event)
{
    if (event.mods.isPopupMenu() && !leftArrowButton->isMouseOver()
                                 && !rightArrowButton->isMouseOver()) // TODO CHECK IF CLICK WASNT OVER ARROWS
    {
        createBankMenu();
    }
}

void BankSelectorComponent::createBankMenu()
{
    PopupMenu bankMenu, duplicateMenu;

    for (int bankNum = 1; bankNum <= neova_dash::gesture::NUM_PRESETS; bankNum++)
    {
        if (bankNum - 1 != hubConfig.getSelectedPreset())
            duplicateMenu.addItem (bankNum, String ("Bank " + String (bankNum)), true);
    }

    bankMenu.addItem (neova_dash::gesture::NUM_PRESETS+1, "Clear bank", true);
    bankMenu.addSubMenu ("Duplicate to :", duplicateMenu, true);
    
    bankMenu.showMenuAsync (PopupMenu::Options().withParentComponent (getParentComponent())
                                                                //.withMaximumNumColumns (3)
                                                                .withPreferredPopupDirection (PopupMenu::Options::PopupDirection::downwards)
                                                                .withStandardItemHeight (20)
                                                                .withTargetComponent (this),
                            ModalCallbackFunction::forComponent (menuCallback, this));
}

void BankSelectorComponent::menuCallback (int result, BankSelectorComponent* bsComp)
{
    if (bsComp != nullptr)
    {
        bsComp->handleMenuResult(result);
    }
}

void BankSelectorComponent::handleMenuResult (const int menuResult)
{
    switch (menuResult)
    {
        case 0: // No choice
            break;
            
        case neova_dash::gesture::NUM_PRESETS+1: // Clear
            hubConfig.clearPreset (hubConfig.getSelectedPreset());
            commandManager.invokeDirectly (neova_dash::commands::updateDashInterface, true);
            break;

        default: // Duplicate to menuResult
            hubConfig.duplicatePreset (hubConfig.getSelectedPreset(), menuResult-1);
            commandManager.invokeDirectly (neova_dash::commands::updateDashInterface, true);
    }
}

void BankSelectorComponent::update()
{
    leftArrowButton->setVisible (hubConfig.getSelectedPreset() == 0 ? false : true);
    rightArrowButton->setVisible (hubConfig.getSelectedPreset() == neova_dash::gesture::NUM_GEST-1 ? false : true);
    
    repaint();
}

void BankSelectorComponent::createButtons()
{
    Path arrowLeft;
    arrowLeft.startNewSubPath (5.0f, 0.0f);
    arrowLeft.lineTo (0.0f, 5.0f);
    arrowLeft.lineTo (5.0f, 10.0f);

    leftArrowButton = std::make_unique<DashShapeButton> ("Change Preset Left Button",
                                                         Colour(0x00000000),
                                                         neova_dash::colour::mainText);

    addAndMakeVisible (*leftArrowButton);

    leftArrowButton->setShape (arrowLeft, false, true, false);
    leftArrowButton->addListener (this);
    leftArrowButton->addMouseListener (this, false);

    Path arrowRight;
    arrowRight.startNewSubPath (0.0f, 0.0f);
    arrowRight.lineTo (5.0f, 5.0f);
    arrowRight.lineTo (0.0f, 10.0f);

    rightArrowButton = std::make_unique<DashShapeButton> ("Change Preset Right Button",
                                                          Colour(0x00000000),
                                                          neova_dash::colour::mainText);

    addAndMakeVisible (*rightArrowButton);

    rightArrowButton->setShape (arrowRight, false, true, false);
    rightArrowButton->addListener (this);
    rightArrowButton->addMouseListener (this, false);

    if (hubConfig.getSelectedPreset() == 0)
    {
        leftArrowButton->setVisible (false);
    }
    else if (hubConfig.getSelectedPreset() == neova_dash::gesture::NUM_GEST-1)
    {
        rightArrowButton->setVisible (false);
    }
}

