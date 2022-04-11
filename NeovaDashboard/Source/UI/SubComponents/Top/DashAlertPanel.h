/*
  ==============================================================================

    DashAlertPanel.h
    Created: 25 Sep 2019 4:35:26pm
    Author:  Enhancia Dev

  ==============================================================================
*/

#pragma once

#include "../../../../JuceLibraryCode/JuceHeader.h"
#include "../../../Common/DashCommon.h"
#include "../DashShapeButton.h"

//==============================================================================
/*
*/
class DashAlertPanel    : public Component,
                          private Button::Listener
{
public:
    //==============================================================================
    enum SpecificReturnValue
    {
        outdatedFirmware = 1,
        noUploadQuitting,
        upgradePending,
        factoryReset,
        unknown
    };

    //==============================================================================
    DashAlertPanel (const String& title, const String& message,
                                         int returnValue = 0,
                                         const bool hasCloseButton = true,
                                         const String& buttonText = String());
    ~DashAlertPanel();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

    //==============================================================================
    void buttonClicked (Button* bttn) override;
    bool keyPressed (const KeyPress& key) override;

    //==============================================================================
    static DashAlertPanel* createSpecificAlertPanel (SpecificReturnValue panelType);

private:
    //==============================================================================
    void createAndAddTextEditor (const String& textToSet);
    void createAndAddLabel (const String& textToSet);
    void createAndAddButtons (const String& buttonText, const bool addCloseButton = true);

    //==============================================================================
    juce::Rectangle<int> panelArea;
    bool showButton = false;

    //==============================================================================
    std::unique_ptr<Label> bodyText;
    std::unique_ptr<Label> titleLabel;
    std::unique_ptr<TextButton> okButton;
    std::unique_ptr<DashShapeButton> closeButton;

    int modalReturnValue = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DashAlertPanel)
};


