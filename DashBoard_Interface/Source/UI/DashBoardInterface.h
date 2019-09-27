/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#pragma once

#include "../../JuceLibraryCode/JuceHeader.h"
#include "../Common/DashCommon.h"
#include "LookAndFeel/DashBoardLookAndFeel.h"

#include "SubComponents/Gesture/GesturePanel.h"
#include "SubComponents/Top/NewGesturePanel.h"
#include "SubComponents/HeaderComponent.h"
#include "SubComponents/HubComponent/HubComponent.h"
#include "SubComponents/UploadButton.h"
#include "SubComponents/HubComponent/PresetSelectorComponent.h"
#include "SubComponents/Top/DashAlertPanel.h"

ApplicationCommandManager& getCommandManager();

//==============================================================================
class DashBoardInterface   : public Component,
                             public ApplicationCommandTarget
{
public:
    //==============================================================================
    DashBoardInterface (HubConfiguration& data);
    ~DashBoardInterface();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

    //==============================================================================
    void mouseEnter (const MouseEvent&);
    void mouseExit (const MouseEvent&);
    void mouseUp (const MouseEvent&);
    void modifierKeysChanged (const ModifierKeys&);

    //==============================================================================
    ApplicationCommandTarget* getNextCommandTarget() override;
    void getAllCommands (Array<CommandID> &commands) override;
    void getCommandInfo (CommandID commandID, ApplicationCommandInfo& result) override;
    bool perform (const InvocationInfo& info) override;

    //==============================================================================
    void createAndShowAlertPanel (const String& title, const String& message,
                                                        const String& buttonText = String());
    void closePendingAlertPanel();

private:
    //==============================================================================
    static void alertPanelCallback (int modalResult, DashBoardInterface* interface);

    //==============================================================================
    std::unique_ptr<HeaderComponent> header;
    std::unique_ptr<HubComponent> hubComponent;
    std::unique_ptr<GesturePanel> gesturePanel;
    std::unique_ptr<NewGesturePanel> newGesturePanel;
    std::unique_ptr<UploadButton> uploadButton;
    std::unique_ptr<PresetSelectorComponent> presetSelector;
    std::unique_ptr<OptionsPanel> optionsPanel;
    std::unique_ptr<DashAlertPanel> alertPanel;

    DashBoardLookAndFeel dashBoardLookAndFeel;
    
    //==============================================================================
    HubConfiguration& hubConfig;
    bool commandKeyDown = ModifierKeys::currentModifiers.isCommandDown();

    //==============================================================================
    Image backgroundImage = ImageFileFormat::loadFrom (DashData::HUBBG_png, DashData::HUBBG_pngSize);

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DashBoardInterface)
};
