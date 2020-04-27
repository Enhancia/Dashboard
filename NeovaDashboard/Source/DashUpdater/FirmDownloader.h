/*
  ==============================================================================

    FirmDownloader.h
    Created: 27 Apr 2020 9:38:29am
    Author:  aleva

  ==============================================================================
*/

#pragma once

#include "../../JuceLibraryCode/JuceHeader.h"
#include "../Common/DashCommon.h"

/**
    \class  FirmDownloader FirmDownloader.h
 
    \brief  Manages checking for updates and downloading the newest installer.
*/
class FirmDownloader : public URL::DownloadTask::Listener,
					   public Timer
{
public:
    //==============================================================================
    enum downloadState
    {
        noDownload = 0,
        inProgress,
        downloadFinished
    };

    enum deviceFirmware
    {
    	none =0,
    	hub,
    	ring
    };

	//==============================================================================
	FirmDownloader();
	~FirmDownloader();

    //==============================================================================
    void finished (URL::DownloadTask* task, bool success) override;

    void progress (URL::DownloadTask* task,
                   int64 bytesDownloaded,
                   int64 totalLength ) override;

    void timerCallback() override;

	//==============================================================================
	void startDownloadProcess();
    bool hasNewAvailableVersion();

    //==============================================================================
    downloadState getDownloadState();
    bool wasSuccessful();
    float& getDownloadProgressReference();
    String getLatestVersionString();
    File getDownloadedRingFile();
    File getDownloadedHubFile();
    
private:
    //==============================================================================
    void checkForNewAvailableVersion();
    void getCurrentInstalledVersions();
    var fetchRepoJSON();
    bool fetchFilesURLAndVersions (DynamicObject& jsonRef);
    void downloadForDevice (deviceFirmware& device);
    bool deleteCurrentFileforDevice (deviceFirmware& device);
    
    //==============================================================================
	const String currentRingVersion;
	const String currentHubVersion;
	String latestVersion = "";

	//==============================================================================
    const String AUTH_TOKEN = "1ebaae86812185390234259e630e73b92c38da4a"; /*std::getenv ("MACHINE_ENHANCIA_OAUTH");*/
    const URL REPO_URL = URL ("https://api.github.com/repos/Enhancia/Firmware_Releases/releases/latest"); /*std::getenv ("REALEASE_REPO_PATH");*/

    //==============================================================================
    String fileToDownloadHubURL = "";
    int fileHubSize = 0;
    String currentVersionHub = "";
    String latestVersionHub = "";
    File currentHubFile;

    String fileToDownloadRingURL = "";
    int fileRingSize = 0;
    String currentVersionRing = "";
    String latestVersionRing = "";
    File currentRingFile;

    #if JUCE_WINDOWS
    String releaseRelativeLocation = "\\Enhancia\\NeovaDashboard\\Releases\\";
    #elif JUCE_MAC
    String releaseRelativeLocation = "/Application Support/Enhancia/FirmwareReleases/";
    #endif

    //==============================================================================
    bool availableRing = false;
    bool availableHub = false;

    downloadState state = noDownload;
    deviceFirmware device = none;

    bool successful = false;
    float downloadProgress = 0.0f;
    File downloadFolder;
    File downloadedFile;

    //==============================================================================
    std::unique_ptr<URL::DownloadTask> downloadTask;

    //==============================================================================
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FirmDownloader)
};