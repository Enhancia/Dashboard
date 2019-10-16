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

#if JUCE_WINDOWS
    #include <windows.h>
    #include <stdio.h>
    #include <tchar.h>
#endif //JUCE_WINDOWS

//==============================================================================
class Neova_DashBoard_Interface  :	public JUCEApplication,
									private ChangeListener
{
public:

	enum PowerState
	{
		POWER_ON = 0,
		POWER_OFF,
		POWER_PAUSE
	};

    //==============================================================================
    Neova_DashBoard_Interface() {}

    const String getApplicationName() override       { return "Dashboard - Neova"; }
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

		dataReader = std::make_unique<DataReader>(commandManager, hubConfig);
		dataReader->addChangeListener(this);

		dashPipe = std::make_unique<DashPipe>();
		dashPipe->addChangeListener(this);

		/* For testing */
		memcpy(data, "jeannine", sizeof("jeannine"));
		ctrl = 0x01;
		memcpy(data + 8, &ctrl, sizeof(uint32_t));
		dashPipe->sendString(data, 12);
    
    	dashInterface.reset (new DashBoardInterface (hubConfig, *dataReader));
    	mainWindow.reset (new MainWindow (getApplicationName(), dashInterface.get()));
    	dashInterface->grabKeyboardFocus();
    
    	//dashInterface->setInterfaceStateAndUpdate (DashBoardInterface::waitingForConnection);
		DBG("POWER STATE : " + String(hubPowerState) + " \n");
    
    	commandManager.registerAllCommandsForTarget (this);
    	commandManager.registerAllCommandsForTarget (dynamic_cast <ApplicationCommandTarget*>
    	                                                    (mainWindow->getContentComponent()));
    }

    void shutdown() override
    {
        dashInterface = nullptr;
        mainWindow = nullptr;

        Logger::setCurrentLogger (nullptr);
        dashboardLogger = nullptr;

        dataReader->removeChangeListener(this);
        dataReader->connectionLost();
        dashPipe->removeChangeListener(this);
        dashPipe->connectionLost();
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
		if (source == dashPipe.get())
		{
			//updateAllValues();
			DBG("reception\n");
			dashPipe->getDataBuffer(data, 1024);
			uint32_t test = *(uint32_t*)(data + 8);
			switch (test)
			{
			case 0x03:
				DBG("config received\n");
				hubConfig.setConfig(data + 12);

				if (hubPowerState != POWER_OFF) hubConfig.notifyConfigWasChanged();

				if (hubPowerState == POWER_OFF)
				{
					hubPowerState = POWER_ON;
					DBG("POWER STATE : " + String(hubPowerState) + " \n");
					//TODO => mettre interface en mode POWER_ON
                    dashInterface->setInterfaceStateAndUpdate (DashBoardInterface::connected);
				}
				if (!dashInterface->hasKeyboardFocus (true))
				{
					dashInterface->grabKeyboardFocus();
					dashInterface->update();
				}
				else
				{
					commandManager.invokeDirectly(neova_dash::commands::updateDashInterface, true);
				}
				break;
			case 0x05:
				DBG("preset_active_received\n");
				hubConfig.setPreset(*(uint8_t*)(data + 12), false);
				if (dashInterface->getPresetModeState() == DashBoardInterface::presetState)
				{
					dashInterface->hubChangedPreset();
				}
				if (!dashInterface->hasKeyboardFocus(true))
				{
					dashInterface->grabKeyboardFocus();
					dashInterface->update();
				}
				else
				{
					commandManager.invokeDirectly(neova_dash::commands::updateDashInterface, true);
				}
				break;

			case 0x06:
				DBG("hub_power_state received\n");
				if (hubPowerState == POWER_OFF && *(uint8_t*)(data + 12) == POWER_ON)
				{
					/*2 cas possibles :	- hub branché après lancement dashBoard et driver envoie POWER_ON CMD à sa connexion
					/					- hub branché avant lancement dashBoard mais en pause (=> ne renvoie pas sa config) et hub envoie POWER_ON CMD à sa sortie de Pause	
					/ => on demande la config et on laisse l'état actuel à POWER_OFF, il passera à POWER_ON une fois la config reçue 
					*/
					memcpy(data, "jeannine", sizeof("jeannine"));
					ctrl = 0x01;
					memcpy(data + 8, &ctrl, sizeof(uint32_t));
					dashPipe->sendString(data, 12);
					DBG("Previous hubPowerState was POWER_OFF => ask config\n");
				}
				else
				{
					hubPowerState = *(uint8_t*)(data + 12);
					//if (!dashInterface->hasKeyboardFocus(true)) dashInterface->grabKeyboardFocus();

					if (hubPowerState == POWER_ON)
					{
						dashInterface->setInterfaceStateAndUpdate(DashBoardInterface::connected);
					}
					else if (hubPowerState == POWER_OFF)
					{
						dashInterface->setInterfaceStateAndUpdate(DashBoardInterface::waitingForConnection);
					}
					else
					{
						dashInterface->setInterfaceStateAndUpdate(DashBoardInterface::pause);
					}
					//TODO => mettre interface en mode new POWER_STATE
                    
					//commandManager.invokeDirectly(neova_dash::commands::updateDashInterface, true);
					DBG("POWER STATE : " + String(hubPowerState) + " \n");
				}
				break;
			case 0x07 : 
				DBG("preset_state_received\n");
				uint8_t state_received = *(uint8_t*)(data + 12);
				if (state_received == 2)
				{
					dashInterface->setPresetModeState(DashBoardInterface::slaveState);
				}
				else if (state_received == 0)
				{
					dashInterface->setPresetModeState(DashBoardInterface::normalState);
				}
				else
				{
					DBG("Should not be there");
				}
				break;
			}
		}
	}

#if JUCE_WINDOWS
	void launch_nrfutil()
	{
		STARTUPINFO si;
		PROCESS_INFORMATION pi;

		ZeroMemory(&si, sizeof(si));
		si.cb = sizeof(si);
		ZeroMemory(&pi, sizeof(pi));


		// Start the child process. 
		if (!CreateProcess((LPCSTR)"nrfutil.exe",   // No module name (use command line)
			"" ,        // Command line
			NULL,           // Process handle not inheritable
			NULL,           // Thread handle not inheritable
			FALSE,          // Set handle inheritance to FALSE
			CREATE_NO_WINDOW,              // No creation flags
			NULL,           // Use parent's environment block
			NULL,           // Use parent's starting directory 
			&si,            // Pointer to STARTUPINFO structure
			&pi)           // Pointer to PROCESS_INFORMATION structure
			)
		{
			Logger::writeToLog("CreateProcess failed (%d).\n" + String(GetLastError()));
			return;
		}

		// Wait until child process exits.
		WaitForSingleObject(pi.hProcess, INFINITE);
		// Close process and thread handles. 
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
	}
#endif //JUCE_WINDOWS

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
                              uploadConfigToHub,
                              updatePresetModeState
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
            case updatePresetModeState:
                result.setInfo ("Update Preset Mode State", "Updates Preset Mode To Fit Interface",
                                                            "Hub State Set", 0);
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
				//launch_nrfutil();
                return true;

			case updatePresetModeState:
			{
				uint8_t newState = (uint8_t)dashInterface->getPresetModeState();
				memcpy(data, "jeannine", sizeof("jeannine"));
				ctrl = 0x07;
				memcpy(data + 8, &ctrl, sizeof(uint32_t));
				memcpy(data + 12, &newState, sizeof(uint8_t));
				dashPipe->sendString(data, 12 + 1);
			}
				return true;

            case upgradeRing:
                return true;

            default:
                return false;
        }
    }

    //==============================================================================
    ApplicationCommandManager commandManager;
    std::unique_ptr<DashBoardInterface> dashInterface;

private:
    std::unique_ptr<MainWindow> mainWindow;
    HubConfiguration hubConfig;

	std::unique_ptr<DataReader> dataReader;
	std::unique_ptr<DashPipe> dashPipe;

	uint8_t hubPowerState = POWER_OFF;


    ScopedPointer<FileLogger> dashboardLogger;

	uint8_t data[1024];
	uint32_t ctrl = 0x03;

};

//==============================================================================
static Neova_DashBoard_Interface& getApp() { return *dynamic_cast<Neova_DashBoard_Interface*> (JUCEApplication::getInstance()); }
ApplicationCommandManager& getCommandManager() { return getApp().commandManager; }

//==============================================================================
START_JUCE_APPLICATION (Neova_DashBoard_Interface)
