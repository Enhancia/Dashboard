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

/**
    \class  DashUpdater DashUpdater.h
 
    \brief  Manages checking for updates and downloading the newest installer.
*/
class DashUpdater
{
public:
	//==============================================================================
	DashUpdater();
	~DashUpdater();

	//==============================================================================
	bool checkForNewAvailableVersion();
	void callUpdateWindow();

private:
    initializeFileToDownload();
    
	const String currentVersion;
	String latestVersion = "";

    const String AUTH_TOKEN ("1ebaae86812185390234259e630e73b92c38da4a");
    const URL REPO_URL ("https://api.github.com/repos/Enhancia/Dashboard_Releases/releases/latest");
    const String fileToDownloadString;
	//==============================================================================
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DashUpdater)
};