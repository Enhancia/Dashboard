/*
  ==============================================================================

    DashUpdater.cpp
    Created: 9 Mar 2020 10:36:32am
    Author:  aleva

  ==============================================================================
*/

#include "DashUpdater.h"

DashUpdater::DashUpdater() : currentVersion (JUCEApplication::getInstance()->getApplicationVersion())
{
    initializeFileToDownloadString();
    checkForNewAvailableVersion();
}

DashUpdater::~DashUpdater()
{
}

void DashUpdater::checkForNewAvailableVersion()
{
    auto json (fetchRepoJSON());
    auto* repo (json.getDynamicObject());

    if (repo != nullptr)
    {
        latestVersion = repo->getProperty ("tag_name").toString();

        if (latestVersion.containsOnly (".0123456789"))
        {
            String currentVerTemp = currentVersion;
            String latestVerTemp = latestVersion;

            for (int subdivision=0; subdivision < 3; subdivision++)
            {
                if (int (currentVerTemp[0]) < int (latestVerTemp[0]))
                {
                    availableVersion = fetchFileURL (*repo);
                }

                currentVerTemp = currentVerTemp.fromFirstOccurrenceOf (".", false, false);
                latestVerTemp = latestVerTemp.fromFirstOccurrenceOf (".", false, false);
            }
        }
    }
}

void DashUpdater::finished (URL::DownloadTask* task, bool success)
{
    state = downloadFinished;
    successful = success;

    if (!success)
    {
        downloadedFile.deleteFile();
    }

	DBG ("Download finished " << (successful ? "Successfully" : "Unsuccessfully"));
}

void DashUpdater::progress (URL::DownloadTask* task,
                            int64 bytesDownloaded,
                            int64 totalLength )
{
    DBG ("Downloading ... " << float (bytesDownloaded)/1000 << " / "
                            << float (totalLength)/1000 << " Ko\n");

    if (totalLength != 0) downloadProgress = float (bytesDownloaded)/totalLength;
}

void DashUpdater::startDownloadProcess()
{
    if (fileToDownloadURL.isEmpty())
    {
        state = downloadFinished;
        successful = false;
        return;
    }

    URL assetURL (fileToDownloadURL + "?access_token=" + AUTH_TOKEN);
    DBG ("Attempting to download file : " << assetURL.getFileName());
    jassert (assetURL.isWellFormed());

    state = inProgress;
    downloadProgress = 0.0f;

    downloadedFile = File::getSpecialLocation (File::userHomeDirectory).getChildFile ("Downloads")
                                                                       .getNonexistentChildFile(fileToDownloadString.upToFirstOccurrenceOf (".", false, true),
                                                                                                fileToDownloadString.fromFirstOccurrenceOf (".", true, true));
    
    downloadTask.reset (assetURL.downloadToFile (downloadedFile, "\r\nAccept: application/octet-stream\r\n", this));
}

bool DashUpdater::hasNewAvailableVersion()
{
    return availableVersion;
}

DashUpdater::downloadState DashUpdater::getDownloadState()
{
    return state;
}

bool DashUpdater::wasSuccessful()
{
    return successful;
}

float& DashUpdater::getDownloadProgressReference()
{
    return downloadProgress;
}

String DashUpdater::getLatestVersionString()
{
    return latestVersion;
}

File DashUpdater::getDownloadedFile()
{
    if (successful) return downloadedFile;

    return File();
}

void DashUpdater::initializeFileToDownloadString()
{
    #if JUCE_WINDOWS
	fileToDownloadString = "Dashboard-Neova.Setup.msi";
    #elif JUCE_MAC
    fileToDownloadString = "Dashboard-Neova.Setup.dmg";
    #endif
}

var DashUpdater::fetchRepoJSON()
{
    // Creating input stream to get file link
    int status;
    std::unique_ptr<InputStream> urlInStream (REPO_URL.createInputStream (false, nullptr, nullptr,
                                                                         "Authorization: Bearer " + AUTH_TOKEN,
                                                                         1000, nullptr, &status));

    if (urlInStream == nullptr || status != 200)
    {
        DBG ("Failed to create input stream...\nHTTP status code : " << String (status));
        return var();
    }

    // Converting into JSON to get properties
    String streamString = urlInStream->readEntireStreamAsString();
    return JSON::parse (streamString);
}

bool DashUpdater::fetchFileURL (DynamicObject& jsonRef)
{
    // Getting assets
    auto* assets = jsonRef.getProperty ("assets").getArray();

    if (assets != nullptr)
    {
        // Finding url for the File to download
        for (auto asset : *assets)
        {
            if (asset.getProperty ("name", var()).toString() == fileToDownloadString)
            {
                fileToDownloadURL = asset.getProperty ("url", var()).toString();
                fileSize = asset.getProperty ("size", var(0));

                return true;
            }
        }
    }

    DBG ("Failed to find assets or file URL...");

    return false;
}
