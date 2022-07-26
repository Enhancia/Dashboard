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
#include "DashUpdater/DashUpdater.h"
#include "DashUpdater/FirmDownloader.h"

#if JUCE_WINDOWS
    #include "UpgradeHandler/upgradeHandler_Win.h"
#elif JUCE_MAC
    #include "UpgradeHandler/upgradeHandler_MacOS.h"
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
    void initialise (const String&) override
    {
        dashboardLogger.reset (FileLogger::createDefaultAppLogger ("Enhancia/NeovaDashboard/Logs/",
                                                              "neovaDashLog.txt",
                                                              "Neova Dashboard Log | OS : " + SystemStats::getOperatingSystemName() +
                                                              " | Neova Dashboard v" + getApplicationVersion()
                                                                                    + " \n"));
    
		
        Logger::setCurrentLogger (dashboardLogger.get());

		dataReader = std::make_unique<DataReader>(commandManager, hubConfig);
		dataReader->addChangeListener(this);

		dashPipe = std::make_unique<DashPipe>();
		dashPipe->addChangeListener(this);

		upgradeHandler = std::make_unique<UpgradeHandler>(*dashPipe, hubConfig, commandManager, *dataReader);
		updater = std::make_unique<DashUpdater>();
		firmDownloader = std::make_unique<FirmDownloader> (commandManager);

		/* Test if hub is already connected */
		/* For testing */
		memcpy(data, "jeannine", sizeof("jeannine"));
		ctrl = 0x01;
		memcpy(data + 8, &ctrl, sizeof(uint32_t));
		dashPipe->sendString(data, 12);
    
    	dashInterface.reset (new DashBoardInterface (hubConfig, *dataReader, *updater, *upgradeHandler));

    	mainWindow.reset (new MainWindow (getApplicationName(), dashInterface.get(), hubConfig));
    	dashInterface->grabKeyboardFocus();
    
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
		upgradeHandler = nullptr;
		updater = nullptr;
    }

    //==============================================================================
    void systemRequestedQuit() override
    {
        quit();
    }

    void anotherInstanceStarted (const String&) override
    {
    }

	//==============================================================================
	void changeListenerCallback(ChangeBroadcaster* source) override
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
				
				if (hubPowerState != POWER_OFF)
				{
					if (hubConfig.isWaitingForRingCompatibility() && ((hubConfig.getRingFirmwareVersionUint16() & 0xFF00) >> 8) > 0)
					{
						// Dash was waiting for a valid ring firmware version, which it got !
						hubConfig.stopWaitingForRingCompatibility();

						if (dashInterface->hasKeyboardFocus (true)) commandManager.invokeDirectly(neova_dash::commands::setStateAndUpdateDashInterface, true);
						else 										dashInterface->setInterfaceStateAndUpdate();
					}
					else
					{
						hubConfig.notifyConfigWasChanged();
					}
				}

				if (hubPowerState == POWER_OFF)
				{
					hubPowerState = POWER_ON;
					DBG("POWER STATE : " + String(hubPowerState) + " \n");
					//TODO => mettre interface en mode POWER_ON
					hubConfig.setHubIsConnected (true);
					upgradeHandler->checkForSuccessiveUpgrade();
					
					if (dashInterface->hasKeyboardFocus (true)) commandManager.invokeDirectly(neova_dash::commands::setStateAndUpdateDashInterface, true);
					else 										dashInterface->setInterfaceStateAndUpdate();
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

				DBG("KBD FOCUS ? " << (dashInterface->hasKeyboardFocus(true) ? "YES" : "NO"));

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
						hubConfig.setHubIsConnected (true);
						upgradeHandler->checkForSuccessiveUpgrade();
						
						if (dashInterface->hasKeyboardFocus (true)) commandManager.invokeDirectly(neova_dash::commands::setStateAndUpdateDashInterface, true);
						else 										dashInterface->setInterfaceStateAndUpdate();
					}
					else if (hubPowerState == POWER_OFF)
					{
						hubConfig.setHubIsConnected (false);
						
						if (dashInterface->hasKeyboardFocus (true)) commandManager.invokeDirectly(neova_dash::commands::setStateAndUpdateDashInterface, true);
						else 										dashInterface->setInterfaceStateAndUpdate();
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
			{
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
			case 0xFF:
				DBG("upgrade firm appeared\n");
				uint8_t type_of_firm = *(uint8_t*)(data + 12);
				
                if (type_of_firm == UpgradeHandler::upgradeFirmHub && upgradeHandler->get_upgradeCommandReceived())
				{
					DBG("hub upgrade firm connected\n");
					upgradeHandler->set_upgradeCommandReceived(false);
					upgradeHandler->launchNrfutil(UpgradeHandler::upgradeFirmHub, data + 13);
				}
				else if (type_of_firm == UpgradeHandler::upgradeFirmRing && upgradeHandler->get_upgradeCommandReceived())
				{
					DBG("ring upgrade firm connected\n");
					upgradeHandler->set_upgradeCommandReceived(false);
					upgradeHandler->launchNrfutil(UpgradeHandler::upgradeFirmRing, data + 13);
				}
				else if (type_of_firm == UpgradeHandler::err_two_hub)
				{
					DBG("two hub connected\n");
                	dashInterface->createAndShowAlertPanel ("Error", "Two hubs detected. Only the first one will be used."
                													 "(Disconnect all the hubs and reconnect the latter one to use it)",
                													 "Ok", true, 0);
					//AlertWindow::showMessageBox(AlertWindow::WarningIcon, "Erreur", , "Sorry, Thanks", nullptr);
				}
				else
				{
					DBG("Should not be there");
				}
				break;
			}
		}
	}

	bool isFirmwareUpgrading()
	{
		return (upgradeHandler->getUpgradeState() == UpgradeHandler::waitingForUpgradeFirm ||
        		upgradeHandler->getUpgradeState() == UpgradeHandler::upgradeFirmConnected  ||
        		upgradeHandler->getUpgradeState() == UpgradeHandler::upgradeInProgress);
	}

    //==============================================================================
    class MainWindow    : public DocumentWindow
    {
    public:
        MainWindow (String name , DashBoardInterface* dashInterface, HubConfiguration& config)
            : DocumentWindow (name, Desktop::getInstance().getDefaultLookAndFeel()
                                                          .findColour (ResizableWindow::backgroundColourId),
                                    DocumentWindow::allButtons),
              dashboardInterface (*dashInterface), hubConfig (config)
        {
        	neova_dash::log::writeToLog ("Creating Dashboard main window.", neova_dash::log::general);

            setUsingNativeTitleBar (true);
            setContentOwned (dashInterface, true);
            
            setResizable (false, false);
            centreWithSize (getWidth(), getHeight());

            setVisible (true);
        }

        void closeButtonPressed() override
        {
        	if (hubConfig.wasConfigChangedSinceLastFlash())
        	{
        		dashboardInterface.createAndShowAlertPanel (DashAlertPanel::noUploadQuitting);
        	}
        	else if (dynamic_cast<Neova_DashBoard_Interface*> (JUCEApplication::getInstance())->isFirmwareUpgrading())
        	{
        		dashboardInterface.createAndShowAlertPanel (DashAlertPanel::upgradePending);
        	}
        	else
        	{
            	JUCEApplication::getInstance()->systemRequestedQuit();
        	}
        }

    private:
    	DashBoardInterface& dashboardInterface;
    	HubConfiguration& hubConfig;

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
                              updatePresetModeState,
                              checkDashboardUpdate,
							  factoryReset
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
            case updatePresetModeState:
                result.setInfo ("Update Preset Mode State", "Updates Preset Mode To Fit Interface",
                                                            "Hub State Set", 0);
            	break;
            case checkDashboardUpdate:
                result.setInfo ("Check Dashboard Update", "Checks the Dashboard for new updates",
                                                          "Dashbaord Update", 0);
                break;
            case checkAndUpdateNotifications:
                result.setInfo ("Check Dashboard Update", "Checks the Dashboard for new updates",
                                                          "Dashbaord Update", 0);
                break;
			case factoryReset:
				result.setInfo ("Factory Reset", "Perform a factory reset",
					"Hub Reset", 0);
				break;
            default:
                break;
        }
    }

    bool perform (const InvocationInfo& info) override
    {
        using namespace neova_dash::commands;

        if (info.commandID != updatePresetModeState)
        	neova_dash::log::writeToLog ("Executing Backend Command : " + String (commandManager.getNameOfCommand (info.commandID)), neova_dash::log::general);

        switch (info.commandID)
        {
            case flashHub:
				memcpy(data, "jeannine", sizeof("jeannine"));
				ctrl = 0x04;
				memcpy(data + 8, &ctrl, sizeof(uint32_t));
				dashPipe->sendString(data, 12);
				return true;

            case uploadConfigToHub:
            	if (hubConfig.getConfigWasInitialized())
            	{
					hubConfig.getConfig(data+12, sizeof(data)-12);
					memcpy(data, "jeannine", sizeof("jeannine"));
					ctrl = 0x03;
					memcpy(data + 8, &ctrl, sizeof(uint32_t));
					dashPipe->sendString(data, 12 + hubConfig.CONFIGSIZE);
				}
				return true;

            case upgradeHub:
                upgradeHandler->launchUpgradeProcedure();
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

            case checkDashboardUpdate:
            	updater->checkForNewAvailableVersion();
            	commandManager.invokeDirectly (neova_dash::commands::openDashboardUpdatePanel, true);
            	return true;

			case factoryReset:
				hubConfig.setDefaultConfig ();
				dashInterface->update ();
				return true;

            default:
                return false;
        }
    }

    //==============================================================================
    ApplicationCommandManager commandManager;
    std::unique_ptr<DashBoardInterface> dashInterface;

private:
    //==============================================================================
    std::unique_ptr<MainWindow> mainWindow;
    HubConfiguration hubConfig;

	std::unique_ptr<DataReader> dataReader;
	std::unique_ptr<DashPipe> dashPipe;

	std::unique_ptr<DashUpdater> updater;
	std::unique_ptr<FirmDownloader> firmDownloader;

    std::unique_ptr<FileLogger> dashboardLogger;

    //==============================================================================
	uint8_t hubPowerState = POWER_OFF;

	uint8_t data[1024];
	uint32_t ctrl = 0x03;

	std::unique_ptr<UpgradeHandler> upgradeHandler;
};

//==============================================================================
static Neova_DashBoard_Interface& getApp() { return *dynamic_cast<Neova_DashBoard_Interface*> (JUCEApplication::getInstance()); }
ApplicationCommandManager& getCommandManager() { return getApp().commandManager; }

//==============================================================================
START_JUCE_APPLICATION (Neova_DashBoard_Interface)
