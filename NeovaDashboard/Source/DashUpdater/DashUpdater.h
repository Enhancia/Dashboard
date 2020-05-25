/*
  ==============================================================================

    DashUpdater.h
    Created: 9 Mar 2020 10:36:32am
    Author:  aleva

  ==============================================================================
*/

#pragma once

#include "../../JuceLibraryCode/JuceHeader.h"
#include "../Common/DashCommon.h"
#include "GitAssetDownloader.h"

/**
    \class  DashUpdater DashUpdater.h
 
    \brief  Manages checking for updates and downloading the newest installer.
*/
class DashUpdater : public URL::DownloadTask::Listener
{
public:
    //==============================================================================
    enum downloadState
    {
        noDownload = 0,
        inProgress,
        downloadFinished
    };

	//==============================================================================
	DashUpdater();
	~DashUpdater();

    //==============================================================================
    void finished (URL::DownloadTask* task, bool success) override;

    void progress (URL::DownloadTask* task,
                   int64 bytesDownloaded,
                   int64 totalLength ) override;

	//==============================================================================
	void startDownloadProcess();
    bool hasNewAvailableVersion();

    //==============================================================================
    downloadState getDownloadState();
    bool wasSuccessful();
    float& getDownloadProgressReference();
    String getLatestVersionString();
    File getDownloadedFile();

    //==============================================================================
    void launchInstaller();
    
private:
    //==============================================================================
    void checkForNewAvailableVersion();
    void initializeFileToDownloadString();
    var fetchRepoJSON();
    bool fetchFileURL (DynamicObject& jsonRef);
    
    //==============================================================================
	const String currentVersion;
	String latestVersion = "";

	//==============================================================================
    const String AUTH_TOKEN = "1ebaae86812185390234259e630e73b92c38da4a"; /*std::getenv ("MACHINE_ENHANCIA_OAUTH");*/
    const URL REPO_URL = URL ("https://api.github.com/repos/Enhancia/Dashboard_Releases/releases/latest"); /*std::getenv ("REALEASE_REPO_PATH");*/

    //==============================================================================
    String fileToDownloadString = "";
    String fileToDownloadURL = "";
    File downloadedFile;
    int fileSize = 0;

    //==============================================================================
    bool availableVersion = false;
    downloadState state = noDownload;
    bool successful = false;
    float downloadProgress = 0.0f;

    //==============================================================================
    std::unique_ptr<URL::DownloadTask> downloadTask;

    //==============================================================================
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DashUpdater)
};