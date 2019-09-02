/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic startup code for a JUCE application.

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "MainComponent.h"

//==============================================================================
class DashBoard_Interface  : public JUCEApplication
{
public:
    //==============================================================================
    DashBoard_Interface() {}

    const String getApplicationName() override       { return "Plume Dash Board [Interface]"; }
    const String getApplicationVersion() override    { return ProjectInfo::versionString; }
    bool moreThanOneInstanceAllowed() override       { return false; }

    //==============================================================================
    void initialise (const String& commandLine) override
    {
        mainWindow.reset (new MainWindow (getApplicationName(), bgColour));

        commandManager.registerAllCommandsForTarget (this);
        commandManager.registerAllCommandsForTarget (dynamic_cast <ApplicationCommandTarget*>
                                                        (mainWindow->getContentComponent()));

		for (int i = 0; i <= commandManager.getNumCommands(); i++)
		{
			DBG (i << " " << commandManager.getNameOfCommand (i));
		}
    }

    void shutdown() override
    {
        mainWindow = nullptr;
    }

    //==============================================================================
    void systemRequestedQuit() override
    {
        quit();
    }

    void anotherInstanceStarted (const String& commandLine) override
    {
    }

    //==============================================================================
    class MainWindow    : public DocumentWindow
    {
    public:
        MainWindow (String name, Colour& col)
            : DocumentWindow (name, Desktop::getInstance().getDefaultLookAndFeel()
                                                          .findColour (ResizableWindow::backgroundColourId),
                                    DocumentWindow::allButtons)
        {
            setUsingNativeTitleBar (true);
            setContentOwned (new MainComponent (col), true);
            
            setResizable (false, false);
            centreWithSize (getWidth(), getHeight());

            setVisible (true);
        }

        void closeButtonPressed() override
        {
            JUCEApplication::getInstance()->systemRequestedQuit();
        }

    private:
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainWindow)
    };

    //==============================================================================
    void getAllCommands (Array<CommandID> &commands) override
    {
        int commandIDs[] = { 1, // SetBlue
                             2  // SetRed
                            };

        commands.addArray (commandIDs, numElementsInArray (commandIDs));
    }

    void getCommandInfo (CommandID commandID, ApplicationCommandInfo& result) override
    {
        switch (commandID)
        {
            case 1:
                result.setInfo ("Set Blue", "Sets bg colour to blue", "Colour", 0);
                result.addDefaultKeypress ('b', ModifierKeys (ModifierKeys::noModifiers));
                break;
            case 2:
				result.setInfo("Set Red", "Sets bg colour to red", "Colour", 0);
				result.addDefaultKeypress('r', ModifierKeys(ModifierKeys::noModifiers));
                break;
            default:
                break;
        }
    }

    bool perform (const InvocationInfo& info) override
    {
        switch (info.commandID)
        {
            case (1):
                DBG ("Executes 1");
                bgColour = Colour (0xff202080);
				commandManager.invokeDirectly (3, true);
				return true;
            case (2):
                DBG ("Executes 2");
                bgColour = Colour (0xff802020);
				commandManager.invokeDirectly (3, true);
				return true;
            default:
                return false;
        }
    }

	Colour& getColourRef() { return bgColour; }

    //==============================================================================
    ApplicationCommandManager commandManager;

private:
    Colour bgColour = Colour (0xff808080);

    std::unique_ptr<MainWindow> mainWindow;
};

//==============================================================================
static DashBoard_Interface& getApp() { return *dynamic_cast<DashBoard_Interface*> (JUCEApplication::getInstance()); }
ApplicationCommandManager& getCommandManager() { return getApp().commandManager; }

//==============================================================================
START_JUCE_APPLICATION (DashBoard_Interface)
