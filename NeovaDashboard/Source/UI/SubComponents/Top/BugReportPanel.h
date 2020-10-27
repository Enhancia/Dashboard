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
    URL createURLForTicket (const String& boundary);
    String createMultipartData (const String& boundary, const Array<File>& txtFilesToAttach);
    void getFilestoAttach (Array<File>& fileArrayToFill);

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
    const String key = "655b11ebbe874e48b754aca2a68f6af1";
    const String code = "25cbca823bb449a0960ee5f4500095bc";
    const String credentials;
    const String mimeBoundary = "8b030e29628a41fc816c7ba481f509c3";

    //==============================================================================
    ApplicationCommandManager& commandManager;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BugReportPanel)
};