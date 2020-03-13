/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#pragma once

#include "../../JuceLibraryCode/JuceHeader.h"
#include "../Common/DashCommon.h"
#include "LookAndFeel/DashBoardLookAndFeel.h"

#include "../DashUpdater/DashUpdater.h"
#include "../DataReader/DataReader.h"
#include "SubComponents/Gesture/GesturePanel.h"
#include "SubComponents/Top/NewGesturePanel.h"
#include "SubComponents/Top/UpdaterPanel.h"
#include "SubComponents/HeaderComponent.h"
#include "SubComponents/HubComponent/HubComponent.h"
#include "SubComponents/UploadButton.h"
#include "SubComponents/HubComponent/PresetSelectorComponent.h"
#include "SubComponents/Top/DashAlertPanel.h"

ApplicationCommandManager& getCommandManager();

//==============================================================================
/**
    \class  DashBoardInterface DashBoardInterface.h
 
    \brief  The Dashboard's Main UI Component.
 
    Manages all Dashboard's subcomponents, performs several commands to interact with the app,
    and manages the dashboard and HUB states from the UI side.
*/
class DashBoardInterface   : public Component,
                             public ApplicationCommandTarget
{
public:
    /**
        \enum InterfaceState
     
        \brief  The different states the interface can possess.
    */
    enum InterfaceState
    {
        connected =0,         /**< \brief HUB is connected and sends data. */
        waitingForConnection, /**< \brief No HUB is connected. */
        pause                 /**< \brief HUB is connected but paused. */
    };

    /**
        \enum PresetModeState

        \brief  The different states that affect the HUB component and its logic.
    */
    enum PresetModeState
    {
        normalState =0, /**< \brief HUB is in normal state. Pressing a HUB button mutes a gesture. */
        presetState,    /**< \brief HUB is in preset mode. Pressing a HUB button changes the active preset. */
        slaveState      /**< \brief HUB is in slave state. The interface's HUB follows the physical HUB and cannot be used. */
    };

    //==============================================================================

    /** 
        \brief Constructor.

        Creates the interface, with all subcomponents and initialized parameters.
    */
    DashBoardInterface (HubConfiguration& data, DataReader& reader, DashUpdater& updtr);

    /**
        \brief  Destructor.
    */
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
    /** 
        \brief  Interface state setter.

        This method will change the interface state, according to the newState parameter.
        Depending on the wanted state, interface subcomponents will be set visible or invisible,
        and be updated accordingly.

        \param newState Value from the InterfaceState enum, telling which state the interface should be set to.
    */
    void setInterfaceStateAndUpdate (const InterfaceState newState);

    //==============================================================================
    /** 
        \brief  Preset mode state getter.

        This method will return an in value that corresponds to the presetModeState enum, telling
        which preset mode state is currently in use.
    */
    int getPresetModeState();

    /** 
        \brief  Preset mode state setter.

        This method will change the preset mode state, according to the newState parameter.
        The method will then transfer the call to the HubComponent by calling lockHubToPresetMode() or
        setPresetStateToNormalMode() depending on the newState parameter.

        \param newState Value from the PresetModeState enum, telling which preset mode state the interface should be set to.
    */
    void setPresetModeState (const PresetModeState newState);
    /** 
        \brief  Callback on hub changing preset from outside sources.

        This method is called by other objects when the hub changed its preset number. This method will
        then chose to either stay in preset mode or go back to normal mode, depending on the user button presses
        and mouse position.
    */
    void hubChangedPreset();

    //==============================================================================
    /** 
        \brief  Interface update method.

        This method is called by outside sources when internal values are changed and this part
        of the interface needs to be updated to match those.
    */
    void update();

    //==============================================================================
    /** 
        \brief  Modal alert panel creator.

        This method will create a floating alert panel above the interface, that can be close either by
        calling closePendingAlertPanel() or by using its button, provided a button was created using the
        buttonText attribute.

        \param title      Title text, written on the upper part of the panel.
        \param message    Core text, written on the center of the panel.
        \param buttonText Text button. Provide an empty String to create no button instead.
    */
    void createAndShowAlertPanel (const String& title, const String& message,
                                                       const String& buttonText = String());

    /** 
        \brief  Modal alert panel destructor.

        Call this method to end the modal alert panel.
    */
    void closePendingAlertPanel();

private:
    //==============================================================================
    /** 
        \brief  Modal alert panel callback.

        Callback method upon clicking the button or closing the panel.
        Will properly end the modal loop and delete the component.

        \param modalResult   Result from the modal loop. Unused.
        \param interf        Pointer to the interface.
    */
    static void alertPanelCallback (int modalResult, DashBoardInterface* interf);

    /** 
        \brief  Helper method to paint simplified shadows under main components.

        This method will create replicas of the main components outlines under them, and blur
        those to create shadows that are faster to compute than the JUCE default shadows. This method
        will slow down the interface responsiveness on slower computers. Maybe could be simplified or removed
        in a later made "low latency mode".

        \param g  Reference to the Graphics class used to paint this component.
    */
    void paintShadows (Graphics& g);

    /** 
        \brief  Helper method to paint a message depending on the current interface state.

        The message is a welcome message for the disconnected state, and "PAUSE" when hub is paused.
        Does nothing in default connected mode

        \param g  Reference to the Graphics class used to paint this component.
    */
    void drawStateMessage (Graphics& g);

    //==============================================================================
    /** 
        \brief  Private setter for preset mode state.

        This method will set preset mode state to presetMode. This setter is private which ensures that no
        external object can set the preset mode to preset.
    */
    void setPresetStateToPresetMode();

    /** 
        \brief  Private setter for preset mode state.

        This method will set preset mode state to normalMode. This setter is private which ensures that no
        external object can set the preset mode to preset.
    */
    void setPresetStateToNormalMode();

    //==============================================================================
    std::unique_ptr<HeaderComponent> header; /**< \brief Interface's header. */
    std::unique_ptr<HubComponent> hubComponent; /**< \brief Interface's hub component. */
    std::unique_ptr<GesturePanel> gesturePanel; /**< \brief Interface's gesture panel. */
    std::unique_ptr<NewGesturePanel> newGesturePanel; /**< \brief Interface's gesture creator panel. */
    std::unique_ptr<UploadButton> uploadButton; /**< \brief Interface's upload button. */
    std::unique_ptr<PresetSelectorComponent> presetSelector; /**< \brief Interface's preset selector component. */
    std::unique_ptr<OptionsPanel> optionsPanel; /**< \brief Interface's option menu. */
    std::unique_ptr<UpdaterPanel> updaterPanel; /**< \brief Interface's update menu. */
    std::unique_ptr<DashAlertPanel> alertPanel; /**< \brief Interface's modal alert panel. */

    DashBoardLookAndFeel dashBoardLookAndFeel; /**< \brief Interface's look and feel. */
    DropShadowEffect shadowEffect; /**< \brief Interface's shadow effect. Unused. */
    
    //==============================================================================
    HubConfiguration& hubConfig; /**< \brief Reference to the internal HubConfiguration object. */
    DataReader& dataReader; /**< \brief Reference to the internal DataReader object.  */
    DashUpdater& updater; /**< \brief Reference to the internal DashUpdater object.  */

    bool commandKeyDown = ModifierKeys::currentModifiers.isCommandDown(); /**< \brief Boolean tracking the user's command key presses. */
    
    int state = int (connected); /**< Interface's current state. Corresponds to a InterfaceState enum value. */
    int presetModeState = int (normalState); /**< Interface's current preset mode state. Corresponds to a PresetModeState enum value. */

    //==============================================================================
    Image backgroundImage = ImageFileFormat::loadFrom (DashData::HUBBG_png, DashData::HUBBG_pngSize); /**< Interface's background image. */

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DashBoardInterface)
};