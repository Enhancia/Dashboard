/*
  ==============================================================================

    BankSelectorComponent.h
    Created: 17 Sep 2019 3:57:45pm
    Author:  Enhancia Dev

  ==============================================================================
*/

#pragma once

#include "../../../../JuceLibraryCode/JuceHeader.h"
#include "../../../Common/HubConfiguration.h"
#include "../../../Common/DashCommon.h"
#include "../DashShapeButton.h"
#include "../../LookAndFeel/DashBoardLookAndFeel.h"

//==============================================================================
/*
*/
class BankSelectorComponent    : public Component,
                                   private Button::Listener
{
public:
    //==============================================================================
    BankSelectorComponent (HubConfiguration& config, ApplicationCommandManager& manager);
    ~BankSelectorComponent();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

    //==============================================================================
    void buttonClicked (Button* bttn) override;

    //==============================================================================
    void mouseUp (const MouseEvent& event) override;

    //==============================================================================
    void update();

private:
    //==============================================================================
    void createButtons();

    //==============================================================================
    void createBankMenu();
    static void menuCallback (int result, BankSelectorComponent* bsComp);

    void handleMenuResult (const int menuResult);

    //==============================================================================
    HubConfiguration& hubConfig;
    ApplicationCommandManager& commandManager;

    std::unique_ptr<DashShapeButton> leftArrowButton;
    std::unique_ptr<DashShapeButton> rightArrowButton;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BankSelectorComponent)
};
