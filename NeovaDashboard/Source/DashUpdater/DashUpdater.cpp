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
	initializeFileToDownload();
}

DashUpdater::~DashUpdater()
{
}

bool DashUpdater::checkForNewAvailableVersion()
{
	return false;
}

void DashUpdater::callUpdateWindow()
{
}

void DashUpdater::initializeFileToDownload()
{
  #if JUCE_WINDOWS
	
  #elif JUCE_MAC
    
  #endif
}