/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "DashCommon.h"

//#include "GesturePanel.h"
#include "NewGesturePanel.h"
#include "HeaderComponent.h"
#include "PresetSelectorComponent.h"
#include "UploadButton.h"

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
    void mouseDown (const MouseEvent&);

    //==============================================================================
    ApplicationCommandTarget* getNextCommandTarget() override;
    void getAllCommands (Array<CommandID> &commands) override;
    void getCommandInfo (CommandID commandID, ApplicationCommandInfo& result) override;
    bool perform (const InvocationInfo& info) override;


private:
    //==============================================================================
    std::unique_ptr<HeaderComponent> header;
    std::unique_ptr<PresetSelectorComponent> presetSelector;
    //std::unique_ptr<GesturePanel> gesturePanel;
    std::unique_ptr<NewGesturePanel> newGesturePanel;
    std::unique_ptr<UploadButton> uploadButton;
    
    //==============================================================================
    HubConfiguration& hubConfig;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DashBoardInterface)
};
