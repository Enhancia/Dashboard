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
    enum InterfaceState
    {
        connected =0,
        waitingForConnection,
        pause
    };

    //==============================================================================
    DashBoardInterface (HubConfiguration& data);
    ~DashBoardInterface();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

    //==============================================================================
    void mouseEnter (const MouseEvent&) override;
    void mouseExit (const MouseEvent&) override;
    void mouseUp (const MouseEvent&) override;
    void modifierKeysChanged (const ModifierKeys&) override;
    bool keyPressed (const KeyPress& key) override;

    //==============================================================================
    ApplicationCommandTarget* getNextCommandTarget() override;
    void getAllCommands (Array<CommandID> &commands) override;
    void getCommandInfo (CommandID commandID, ApplicationCommandInfo& result) override;
    bool perform (const InvocationInfo& info) override;

    //==============================================================================
    void setInterfaceStateAndUpdate (const InterfaceState newState);

    //==============================================================================
    void update();

    //==============================================================================
    void createAndShowAlertPanel (const String& title, const String& message,
                                                        const String& buttonText = String());
    void closePendingAlertPanel();

private:
    //==============================================================================
    static void alertPanelCallback (int modalResult, DashBoardInterface* interface);
    void paintShadows (Graphics& g);
    void drawStateMessage (Graphics& g);

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
    DropShadowEffect shadowEffect;
    
    //==============================================================================
    HubConfiguration& hubConfig;
    bool commandKeyDown = ModifierKeys::currentModifiers.isCommandDown();

    InterfaceState state = connected;

    //==============================================================================
    Image backgroundImage = ImageFileFormat::loadFrom (DashData::HUBBG_png, DashData::HUBBG_pngSize);

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DashBoardInterface)
};
