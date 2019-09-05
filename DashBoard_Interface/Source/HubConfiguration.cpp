/*
  ==============================================================================

    HubConfiguration.cpp
    Created: 5 Sep 2019 10:16:02am
    Author:  Enhancia Dev

  ==============================================================================
*/

#include "HubConfiguration.h"

HubConfiguration::HubConfiguration()
{
}
HubConfiguration::~HubConfiguration()
{
}

void HubConfiguration::setMidiChannelAndUpload (const uint8 newMidiChannel)
{
	ignoreUnused (newMidiChannel); // TO DELETE
	
	// Sets Value

	commandManager.invokeDirectly (neova_dash::commands::uploadConfigToHub, true);
}
void HubConfiguration::setUint8ValueAndUpload (const int gestureNumber, const uint8DataId dataId, const uint8 newUint8Value)
{
	ignoreUnused (gestureNumber); ignoreUnused (dataId); ignoreUnused (newUint8Value); // TO DELETE
	
	// Sets Value

	commandManager.invokeDirectly (neova_dash::commands::uploadConfigToHub, true);
}
void HubConfiguration::setFloatValueAndUpload (const int gestureNumber, const floatDataId dataId, const float newFloatValue)
{
	ignoreUnused (gestureNumber); ignoreUnused (dataId); ignoreUnused (newFloatValue); // TO DELETE
	
	// Sets Value

	commandManager.invokeDirectly (neova_dash::commands::uploadConfigToHub, true);
}

void HubConfiguration::setPreset (const int gestureNumberToSelect)
{
	if (gestureNumberToSelect < 0 || gestureNumberToSelect > 4 || gestureNumberToSelect == selectedPreset) return;

	selectedPreset = gestureNumberToSelect;
	commandManager.invokeDirectly (neova_dash::commands::uploadConfigToHub, true);	
}