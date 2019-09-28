/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic startup code for a JUCE application.

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "UI/DashBoardInterface.h"
#include "Common/DashCommon.h"
#include "DataReader/DataReader.h"
#include "DataReader/dashPipe.h"

//==============================================================================
class Neova_DashBoard_Interface  :	public JUCEApplication,
									private ChangeListener
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

        interface.reset (new DashBoardInterface (hubConfig));
        mainWindow.reset (new MainWindow (getApplicationName(), interface.get()));

        commandManager.registerAllCommandsForTarget (this);
        commandManager.registerAllCommandsForTarget (dynamic_cast <ApplicationCommandTarget*>
                                                        (mainWindow->getContentComponent()));
		dataReader = std::make_unique<DataReader>();
		dataReader->addChangeListener(this);

		dashPipe = std::make_unique<DashPipe>();
		dashPipe->addChangeListener(this);

		/* For testing */
		memcpy(data, "jeannine", sizeof("jeannine"));
		ctrl = 0x01;
		memcpy(data + 8, &ctrl, sizeof(uint32_t));
		dashPipe->sendString(data, 12);
    }

    void shutdown() override
    {
        interface = nullptr;
        mainWindow = nullptr;

        Logger::setCurrentLogger (nullptr);
        dashboardLogger = nullptr;

		dataReader = nullptr;
		dashPipe = nullptr;
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
	void changeListenerCallback(ChangeBroadcaster* source)
	{
		//updateAllValues();
		DBG("reception\n");
		dashPipe->getDataBuffer(data, 1024);
		uint32_t test = 0;
		switch (*(uint32_t*)(data+8))
		{
			case 0x02:
				DBG("config received\n");
				hubConfig.setConfig(data + 12);
				commandManager.invokeDirectly(neova_dash::commands::updateDashInterface, true);
				break;
			case 0x05:
				DBG("preset_active_received\n");
				hubConfig.setPreset(*(uint8_t*)(data+12)-1, false);
				commandManager.invokeDirectly(neova_dash::commands::updateDashInterface, true);
				break;
			default:
				break;
		}


	}

    //==============================================================================
    class MainWindow    : public DocumentWindow
    {
    public:
        MainWindow (String name , DashBoardInterface* dashInterface)
            : DocumentWindow (name, Desktop::getInstance().getDefaultLookAndFeel()
                                                          .findColour (ResizableWindow::backgroundColourId),
                                    DocumentWindow::allButtons)
        {
            setUsingNativeTitleBar (true);
            setContentOwned (dashInterface, true);
            
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
				memcpy(data, "jeannine", sizeof("jeannine"));
				ctrl = 0x04;
				memcpy(data + 8, &ctrl, sizeof(uint32_t));
				dashPipe->sendString(data, 12);
                return true;

            case uploadConfigToHub:
				hubConfig.getConfig(data+12, sizeof(data)-12);
				memcpy(data, "jeannine", sizeof("jeannine"));
				ctrl = 0x03;
				memcpy(data + 8, &ctrl, sizeof(uint32_t));
				dashPipe->sendString(data, 12 + hubConfig.CONFIGSIZE);
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
    std::unique_ptr<DashBoardInterface> interface;

private:
    std::unique_ptr<MainWindow> mainWindow;
    HubConfiguration hubConfig;

	std::unique_ptr<DataReader> dataReader;
	std::unique_ptr<DashPipe> dashPipe;

    ScopedPointer<FileLogger> dashboardLogger;

	uint8_t data[1024];
	uint32_t ctrl = 0x03;
};

//==============================================================================
static Neova_DashBoard_Interface& getApp() { return *dynamic_cast<Neova_DashBoard_Interface*> (JUCEApplication::getInstance()); }
ApplicationCommandManager& getCommandManager() { return getApp().commandManager; }

//==============================================================================
START_JUCE_APPLICATION (Neova_DashBoard_Interface)