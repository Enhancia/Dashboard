/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic startup code for a JUCE application.

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "DashBoardInterface.h"
#include "DashCommon.h"

//==============================================================================
class Neova_DashBoard_Interface  : public JUCEApplication
{
public:
    //==============================================================================
    Neova_DashBoard_Interface() {}

    const String getApplicationName() override       { return "Neova Dash Board [Interface]"; }
    const String getApplicationVersion() override    { return ProjectInfo::versionString; }
    bool moreThanOneInstanceAllowed() override       { return false; }

    //==============================================================================
    void initialise (const String& commandLine) override
    {
        dashboardLogger = FileLogger::createDefaultAppLogger ("Enhancia/NeovaDashboard/Logs/",
                                                              "neovaDashLog.txt",
                                                              "Neova Dashboard Log | OS :"
                                                            #if JUCE_MAC
                                                              " MAC "
                                                            #elif JUCE_WINDOWS
                                                              " Windows "
                                                            #endif
                                                              "| Neova Dashboard v" + getApplicationVersion()
                                                                                    + " \n");
    
        Logger::setCurrentLogger (dashboardLogger);

        mainWindow.reset (new MainWindow (getApplicationName(), hubConfig));

        commandManager.registerAllCommandsForTarget (this);
        commandManager.registerAllCommandsForTarget (dynamic_cast <ApplicationCommandTarget*>
                                                        (mainWindow->getContentComponent()));

		for (int i = neova_dash::commands::flashHub; i <= commandManager.getNumCommands(); i++)
		{
            String s = commandManager.getNameOfCommand (i);

			if (!s.isEmpty()) DBG (i << " " << s);
		}
    }

    void shutdown() override
    {
        mainWindow = nullptr;

        Logger::setCurrentLogger (nullptr);
        dashboardLogger = nullptr;
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
        MainWindow (String name , HubConfiguration& data)
            : DocumentWindow (name, Desktop::getInstance().getDefaultLookAndFeel()
                                                          .findColour (ResizableWindow::backgroundColourId),
                                    DocumentWindow::minimiseButton + DocumentWindow::closeButton)
        {
            setUsingNativeTitleBar (true);
            setContentOwned (new DashBoardInterface (data), true);
            
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
        using namespace neova_dash::commands;

        commands.addArray ({  
                              flashHub,
                              upgradeHub,
                              upgradeRing,
                              uploadConfigToHub
                           });
    }

    void getCommandInfo (CommandID commandID, ApplicationCommandInfo& result) override
    {
        using namespace neova_dash::commands;

        switch (commandID)
        {
            case flashHub:
                result.setInfo ("Flash Hub", "Saves Hub's temp config in its flash memory", "Hub Data Set", 0);
                break;
            case uploadConfigToHub:
                result.setInfo ("Upload Config To Hub", "Sets the hub to the dashboard's config", "Hub Data Set", 0);
                break;
            case upgradeHub:
				result.setInfo ("Upgrade Hub Firmware", "Updgrades the hub firmware to the most recent version",
                                                        "Firm Update", 0);
                break;
            case upgradeRing:
                result.setInfo ("Upgrade Ring Firmware", "Updgrades the ring firmware to the most recent version",
                                                         "Firm Update", 0);
                break;
            default:
                break;
        }
    }

    bool perform (const InvocationInfo& info) override
    {
        using namespace neova_dash::commands;
        Logger::writeToLog ("Back performs : " + String (commandManager.getNameOfCommand (info.commandID)));

        switch (info.commandID)
        {
            case flashHub:
                return true;

            case uploadConfigToHub:
                return true;

            case upgradeHub:
                return true;

            case upgradeRing:
                return true;

            default:
                return false;
        }
    }

    //==============================================================================
    ApplicationCommandManager commandManager;

private:
    std::unique_ptr<MainWindow> mainWindow;
    HubConfiguration hubConfig;

    ScopedPointer<FileLogger> dashboardLogger;
};

//==============================================================================
static Neova_DashBoard_Interface& getApp() { return *dynamic_cast<Neova_DashBoard_Interface*> (JUCEApplication::getInstance()); }
ApplicationCommandManager& getCommandManager() { return getApp().commandManager; }

//==============================================================================
START_JUCE_APPLICATION (Neova_DashBoard_Interface)