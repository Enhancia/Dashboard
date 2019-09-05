/*
  ==============================================================================

    ConfigData.cpp
    Created: 5 Sep 2019 10:16:02am
    Author:  Enhancia Dev

  ==============================================================================
*/

#include "configData.h"

ConfigData::ConfigData()
{
}
ConfigData::~ConfigData()
{
}
void ConfigData::setDataAndUploadToHub (parameterID parameterToSet, float valueToSet)
{
	ignoreUnused (parameterToSet); ignoreUnused (valueToSet); // TO DELETE

	// sets the value accordingly

	commandManager.invokeDirectly (neova_dash::commands::uploadConfigToHub, true);
}