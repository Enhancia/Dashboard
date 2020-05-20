/*
  ==============================================================================

    GitAssetDownloader.h
    Created: 18 May 2020 7:14:06pm
    Author:  aleva

  ==============================================================================
*/

#pragma once

#include "../../JuceLibraryCode/JuceHeader.h"
#include "../Common/DashCommon.h"

/**
    \class  GitAssetDownloader GitAssetDownloader.h
 
    \brief  Helper static class with a method to download an asset from a private enhancia git repo.
 
    Manages fetching a asset from a private git repo. This class is necessary to avoid an issue from gitHub, when
    trying to download an asset from a private repo using an OAuth Token, stored inside the class.
*/
class GitAssetDownloader
{
public:
    /**
        \brief  Main method from the class. Launches the asset download.
     
        Instead of trying to download the file with a single URL with both "Authentification : token xxx" and
        "accept : application/octet-stream" HTTP headers, this method will only use this URL to get the redirect
        URL to the actual location of the file.
        It then downloads the file using the redirect URL instead, with no HTTP headers.
        
    */
    static URL::DownloadTask* downloadAsset (const URL& assetURL, const File& fileToDownloadTo, URL::DownloadTask::Listener* listenerPtr);
    
private:
    /**
        \brief  private helper method to get the redirect URL.
     
        This method will fetch the redirect URL to the asset by creating an input stream from the github asset URL, with no redirect.
    */
	static bool getRedirectURL (const URL& assetURL, URL& redirectURL);

    /**
        \brief  Private constructor, so the class remains static.
    */
    GitAssetDownloader() {};
};