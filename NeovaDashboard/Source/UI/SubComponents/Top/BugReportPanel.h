/*
  ==============================================================================

    BugReportPanel.h
    Created: 15 Oct 2020 4:35:55pm
    Author:  alexl

  ==============================================================================
*/

#pragma once

#include "../../../../JuceLibraryCode/JuceHeader.h"
#include "../../../Common/DashCommon.h"
#include "../DashShapeButton.h"

class BugReportPanel : public Component,
                       private Button::Listener,
                       private Label::Listener
{
public:
    //==============================================================================
    enum ReportStep
    {
        newReport = 0,
        incorrectReport,
        reportSentOk,
        reportSentError
    };

    //==============================================================================
    BugReportPanel (ApplicationCommandManager& manager);
    ~BugReportPanel();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

    //==============================================================================
    void buttonClicked (Button* bttn) override;
    void labelTextChanged (Label* lbl) override;

    //==============================================================================
    void resetAndOpenPanel();
    void closeAndResetPanel();
    
private:
    //==============================================================================
    void createLabels();
    void createButtons();
	void updateComponentsForSpecificStep (ReportStep stepToUpgradeTo);

    //==============================================================================
    void checkFormEntry();
    bool formSeemsCorrect();

    //==============================================================================
    void sendTicketAndUpdate();
    URL createURLForTicket();
    String createMultipartData (const String& mimeBoundary, const Array<File>& txtFilesToAttach);

    //==============================================================================
    juce::Rectangle<int> panelArea;
    std::unique_ptr<Label> titleLabel;
    std::unique_ptr<Label> nameLabel;
    std::unique_ptr<Label> mailLabel;
    std::unique_ptr<TextEditor> messageEditor;
    std::unique_ptr<DashShapeButton> closeButton;
    std::unique_ptr<TextButton> bottomButton;

    //==============================================================================
    ReportStep currentStep = newReport;
    bool nameIsCorrect = false;
    bool mailIsCorrect = false;

    //==============================================================================
    ApplicationCommandManager& commandManager;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BugReportPanel)
};