/*
  ==============================================================================

    FirmDownloader.cpp
    Created: 27 Apr 2020 9:38:29am
    Author:  aleva

  ==============================================================================
*/

#include "FirmDownloader.h"

FirmDownloader::FirmDownloader (ApplicationCommandManager& manager) : commandManager (manager)
{
	downloadFolder = File::getSpecialLocation (File::userApplicationDataDirectory).getFullPathName() + releaseRelativeLocation;

    checkForNewAvailableVersion();
}

FirmDownloader::~FirmDownloader()
{
}

void FirmDownloader::checkForNewAvailableVersion()
{
	getCurrentInstalledVersions();

    auto json (fetchRepoJSON());
    auto* repo (json.getDynamicObject());

    if (repo != nullptr)
    {
        if (fetchFilesURLAndVersions (*repo))
        {
        	if (!latestVersionHub.isEmpty() && latestVersionHub.containsOnly (".0123456789"))
        	{
				if (!currentVersionHub.isEmpty() && currentVersionHub.containsOnly(".0123456789"))
				{
					availableHub = latestVersionHub.removeCharacters(".").getIntValue() > currentVersionHub.removeCharacters(".").getIntValue();
				}
				else availableHub = true;
        	}

        	if (!latestVersionRing.isEmpty() && latestVersionRing.containsOnly (".0123456789"))
        	{
        		if (!currentVersionRing.isEmpty() && currentVersionRing.containsOnly (".0123456789"))
        		{
        			availableRing = latestVersionRing.removeCharacters(".").getIntValue() > currentVersionRing.removeCharacters(".").getIntValue();
        		}
				else availableRing = true;
        	}
        }
    }

    if (availableHub || availableRing) startDownloadProcess();
}

void FirmDownloader::getCurrentInstalledVersions()
{
	if (downloadFolder.exists())
	{
		auto files = downloadFolder.findChildFiles (2, false, "*");

		for (auto& file : files)
		{
			if (file.getFileNameWithoutExtension().startsWith ("hub"))
			{
				currentHubFile = file;
				currentVersionHub = file.getFileName().fromFirstOccurrenceOf ("_", false, false)
													  .upToLastOccurrenceOf (".", false, false);
			}

			else if (file.getFileNameWithoutExtension().startsWith ("ring"))
			{
				currentRingFile = file;
				currentVersionRing = file.getFileName().fromFirstOccurrenceOf ("_", false, false)
													   .upToLastOccurrenceOf (".", false, false);
			}
		}
	}

	else
	{
		downloadFolder.createDirectory();
	}
}

void FirmDownloader::finished (URL::DownloadTask* task, bool success)
{
    state = downloadFinished;
    successful = success;

    if (!success)
    {
        downloadedFile.deleteFile();
    }
    else
    {
    	deleteCurrentFileforDevice (device);
    }

	DBG ("Download finished " << (successful ? "Successfully" : "Unsuccessfully"));

	if (device == hub) // Lauches download for ring
	{
		startTimer (400);
	}

    commandManager.invokeDirectly (neova_dash::commands::checkAndUpdateNotifications, true);
}

void FirmDownloader::progress (URL::DownloadTask* task,
                            int64 bytesDownloaded,
                            int64 totalLength )
{
    DBG ("Downloading ... " << float (bytesDownloaded)/1000 << " / "
                            << float (totalLength)/1000 << " Ko\n");

    if (totalLength != 0) downloadProgress = float (bytesDownloaded)/totalLength;
}

void FirmDownloader::timerCallback()
{
	if (device == hub && state == downloadFinished)
	{
		stopTimer();
		device = ring;
		downloadForDevice (device);
	}
}

void FirmDownloader::startDownloadProcess()
{
    if (!availableHub && !availableRing)
    {
        state = downloadFinished;
        successful = false;
        return;
    }

    device = availableHub ? hub : ring;

    downloadForDevice (device);
}


void FirmDownloader::downloadForDevice (deviceFirmware& deviceToDownloadFor)
{
    state = inProgress;

	String fileToDownloadString;
	URL fileToDownloadURL;

	if (deviceToDownloadFor == hub)
	{
		fileToDownloadString = "hub_" + latestVersionHub + ".zip";
		fileToDownloadURL = fileToDownloadHubURL;	
		
		if (!availableHub || fileToDownloadHubURL.isEmpty())
		{
			device = ring;
			downloadForDevice (device);
		}
	}

	if (deviceToDownloadFor == ring)
	{
		fileToDownloadString = "ring_" + latestVersionRing + ".zip";
		fileToDownloadURL = fileToDownloadRingURL;	

		if (!availableRing || fileToDownloadRingURL.isEmpty())
		{
		    state = downloadFinished;
		    successful = false;
		    return;
		}
	}
	DBG ("Attempting to download file : " << fileToDownloadURL.getFileName());

    state = inProgress;
    downloadProgress = 0.0f;

    downloadedFile = downloadFolder.getNonexistentChildFile (fileToDownloadString, "");
    //downloadTask.reset (assetURL.downloadToFile (downloadedFile, "\r\nAccept: application/octet-stream\r\n", this));
    downloadTask.reset (GitAssetDownloader::downloadAsset (fileToDownloadURL, downloadedFile, this));
}

bool FirmDownloader::hasNewAvailableVersion()
{
    return availableRing || availableHub;
}

FirmDownloader::downloadState FirmDownloader::getDownloadState()
{
    return state;
}

bool FirmDownloader::wasSuccessful()
{
    return successful;
}

float& FirmDownloader::getDownloadProgressReference()
{
    return downloadProgress;
}

String FirmDownloader::getLatestVersionString()
{
    return latestVersion;
}

var FirmDownloader::fetchRepoJSON()
{
    // Creating input stream to get file link
    int status;
    std::unique_ptr<InputStream> urlInStream (REPO_URL.createInputStream (false, nullptr, nullptr,
                                                                         "Authorization: token " + neova_dash::auth::MACHINE_TOKEN,
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

bool FirmDownloader::fetchFilesURLAndVersions (DynamicObject& jsonRef)
{
    // Getting assets
    auto* assets = jsonRef.getProperty ("assets").getArray();
    bool hubExists = false;
    bool ringExists = false;

    if (assets != nullptr)
    {
        // Finding url for the File to download
        for (auto asset : *assets)
        {
            if (asset.getProperty ("name", var()).toString().startsWith ("hub_"))
            {
                fileToDownloadHubURL = asset.getProperty ("url", var()).toString();
                fileHubSize = asset.getProperty ("size", var(0));
                latestVersionHub = asset.getProperty ("name", var()).toString()
                													.fromFirstOccurrenceOf ("_", false, false)
                													.upToLastOccurrenceOf (".", false, false);

                hubExists = true;
            }

            else if (asset.getProperty ("name", var()).toString().startsWith ("ring_"))
            {
                fileToDownloadRingURL = asset.getProperty ("url", var()).toString();
                fileRingSize = asset.getProperty ("size", var(0));
                latestVersionRing = asset.getProperty ("name", var()).toString()
                													 .fromFirstOccurrenceOf ("_", false, false)
                													 .upToLastOccurrenceOf (".", false, false);

                ringExists = true;
            }
        }
    }

    return hubExists || ringExists;
}

bool FirmDownloader::deleteCurrentFileforDevice (deviceFirmware& device)
{
	File fileToDelete;

	if (device == hub && !currentVersionHub.isEmpty())
	{
		fileToDelete = currentHubFile;
	}
	else if (device == ring && !currentVersionRing.isEmpty())
	{
		fileToDelete = currentRingFile;
	}

	if (fileToDelete.exists())
	{
		fileToDelete.deleteFile();
		return true;
	}

	return false;
}