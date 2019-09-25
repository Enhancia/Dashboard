/*
  ==============================================================================

    DashAlertPanel.h
    Created: 25 Sep 2019 4:35:26pm
    Author:  Enhancia Dev

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "DashCommon.h"

//==============================================================================
/*
*/
class DashAlertPanel    : public Component
{
public:
    //==============================================================================
    DashAlertPanel (const String& title, const String& message,
                                         const String& buttonText = String());
    ~DashAlertPanel();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

private:
    //==============================================================================
    void createAndAddTextEditor (const String& textToSet);
    void createAndAddLabel (const String& textToSet);
    void createAndAddCloseButton (const String& buttonText);

    //==============================================================================
    Rectangle<int> panelArea;

    //==============================================================================
    std::unique_ptr<Label> bodyText;
    std::unique_ptr<Label> titleLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DashAlertPanel)
};
