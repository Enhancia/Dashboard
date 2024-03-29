/*
  ==============================================================================

    GitAssetDownloader.cpp
    Created: 18 May 2020 7:14:06pm
    Author:  aleva

  ==============================================================================
*/

#include "GitAssetDownloader.h"

std::unique_ptr<URL::DownloadTask> GitAssetDownloader::downloadAsset (const URL& assetURL, const File& fileToDownloadTo,
																  	   URL::DownloadTask::Listener* listenerPtr)
{
	URL serverURL;
	if (getRedirectURL (assetURL, serverURL))
	{
		jassert (!serverURL.isEmpty() && serverURL.isWellFormed());

		return serverURL.downloadToFile (fileToDownloadTo, URL::DownloadTaskOptions()
			.withListener(listenerPtr)
		);
	}

	return nullptr;
}


bool GitAssetDownloader::getRedirectURL (const URL& assetURL, URL& redirectURL)
{
	redirectURL = URL();
	String headers = "\r\nAuthorization: token " + neova_dash::auth::MACHINE_TOKEN + "\r\nAccept: application/octet-stream\r\n";

	int status;
	StringPairArray responseHeaders;

	// Num redirects to follow is set to 0: no redirect is followed and response headers tell what the redirect URL was.
    const std::unique_ptr<InputStream> gitAssetStream (assetURL.createInputStream (URL::InputStreamOptions (URL::ParameterHandling::inAddress)
        .withProgressCallback (nullptr)
        .withExtraHeaders (headers)
        .withConnectionTimeoutMs (1000)
        .withResponseHeaders (&responseHeaders)
        .withStatusCode (&status)
        .withNumRedirectsToFollow (0)
    ));

	if (gitAssetStream == nullptr || status != 302) // Error or no redirect
	{
		if (status == 200)
		{
			DBG ("No redirect. URL was likely not a valid gitHub URL to a downloadable asset: " << redirectURL.toString (true));
		}
		else
		{
			DBG ("Failed to get asset on git... HTTP Status code : " + String (status));
		}
	}

	else if (status == 302) // Redirect was caught
	{
		std::cout << "\nRedirect! New location : \n" << responseHeaders.getValue ("location", "");

		URL serverURL (responseHeaders.getValue ("location", String()));

		if (serverURL.isWellFormed())
		{
			redirectURL = serverURL;
			return true;
		}
	}

	return false;
}
