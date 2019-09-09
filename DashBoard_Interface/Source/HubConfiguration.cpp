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
	setDefaultConfig();
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

void HubConfiguration::setDefaultGestureValues (const int gestureNumber, const neova_dash::gesture::GestureType type,
																		 const int presetNumber)
{
	using namespace neova_dash::gesture;

	setGestureData (presetNumber, gestureNumber, 1, uint8 (type), 0, 127, 0);

	switch (type)
	{
		case vibrato:
			setGestureParameters (presetNumber, gestureNumber, 0.0f, 0.0f);
			break;
		case pitchBend:
			setGestureParameters (presetNumber, gestureNumber, 0.0f, 0.0f);
			break;
		case tilt:
			setGestureParameters (presetNumber, gestureNumber, 0.0f, 0.0f);
			break;
		case roll:
			setGestureParameters (presetNumber, gestureNumber, 0.0f, 0.0f);
			break;
		case wave:
			setGestureParameters (presetNumber, gestureNumber, 0.0f, 0.0f);
			break;

		default: DBG ("Invalid gesture type to set values for...");
	}

	commandManager.invokeDirectly (neova_dash::commands::uploadConfigToHub, true);
}

void HubConfiguration::setDefaultGestureValues (const int gestureNumber, const neova_dash::gesture::GestureType type)
{
	setDefaultGestureValues (gestureNumber, type, selectedPreset);
}

void HubConfiguration::setPreset (const int gestureNumberToSelect)
{
	if (gestureNumberToSelect < 0 || gestureNumberToSelect > 4 || gestureNumberToSelect == selectedPreset) return;

	selectedPreset = gestureNumberToSelect;
	commandManager.invokeDirectly (neova_dash::commands::uploadConfigToHub, true);	
}

const int HubConfiguration::getSelectedPreset()
{
	return selectedPreset;
}

HubConfiguration::GestureData& HubConfiguration::getGestureData (const int gestureNumber, const int presetNumber)
{
	PresetData* preset;

	switch (presetNumber)
	{
		case (1):
			preset = &config.presetData1;
			break;
		case (2):
			preset = &config.presetData2;
			break;
		case (3):
			preset = &config.presetData3;
			break;
		default:
			preset = &config.presetData0;
	}

	switch (gestureNumber)
	{
		case (1):
			return preset->gestureData1;

		case (2):
			return preset->gestureData2;
		
		case (3):
			return preset->gestureData3;
		
		default:
			return preset->gestureData0;
	}
}

HubConfiguration::GestureData& HubConfiguration::getGestureData (const int gestureNumber)
{
	return getGestureData (gestureNumber, selectedPreset);
}
/*
const neova_dash::gesuture::GestureType HubConfiguration::getGestureType (const int gestureNumber, const int presetNumber)
{
	PresetData preset;

	switch (presetNumber)
	{
		case (1):
			preset = config.presetData1;
			break;
		case (2):
			preset = config.presetData2;
			break;
		case (3):
			preset = config.presetData3;
			break;
		default:
			preset = config.presetData0;
	}

	GestureData gesture;

	switch (gestureNumber)
	{
		case (1):
			gesture = preset.gestureData1;
			break;
		case (2):
			gesture = preset.gestureData2;
			break;
		case (3):
			gesture = preset.gestureData3;
			break;
		default:
			gesture = preset.gestureData0;
	}

	switch (gesture.type)
	{
		case (1): 
	}
}

const neova_dash::gesuture::GestureType HubConfiguration::getGestureType (const int gestureNumber)
{

}*/

void HubConfiguration::setDefaultConfig()
{
	setGestureData (0, 0, 1, neova_dash::gesture::vibrato, 0, 127, 0);
	setGestureData (0, 1, 1, neova_dash::gesture::pitchBend, 0, 127, 0);
	setGestureData (0, 2, 1, neova_dash::gesture::tilt, 0, 127, 0);
	setGestureData (0, 3, 1, neova_dash::gesture::roll, 0, 127, 0);

	setGestureData (1, 0, 1, neova_dash::gesture::vibrato, 0, 127, 0);
	setGestureData (1, 1, 1, neova_dash::gesture::tilt, 0, 127, 0);
	setGestureData (1, 2, 0, neova_dash::gesture::none, 0, 127, 0);
	setGestureData (1, 3, 1, neova_dash::gesture::roll, 0, 127, 0);

	setGestureData (2, 0, 0, neova_dash::gesture::none, 0, 127, 0);
	setGestureData (2, 1, 0, neova_dash::gesture::none, 0, 127, 0);
	setGestureData (2, 2, 1, neova_dash::gesture::roll, 0, 127, 0);
	setGestureData (2, 3, 1, neova_dash::gesture::wave, 0, 127, 0);

	setGestureData (3, 0, 0, neova_dash::gesture::none, 0, 127, 0);
	setGestureData (3, 1, 0, neova_dash::gesture::none, 0, 127, 0);
	setGestureData (3, 2, 1, neova_dash::gesture::tilt, 0, 127, 0);
	setGestureData (3, 3, 0, neova_dash::gesture::none, 0, 127, 0);

	commandManager.invokeDirectly (neova_dash::commands::uploadConfigToHub, true);
}

void HubConfiguration::setGestureData (int presetNum, int gestureNum,
                                   				      uint8 newOn,
                                   				      uint8 newType,
                                   				      uint8 newMidiLow,
                                   				      uint8 newMidiHigh,
                                   				      uint8 newCc,
                                   				      bool uploadToHub)
{
	GestureData& gesture = getGestureData (gestureNum, presetNum);

	gesture.on       = newOn;
	gesture.type     = newType;
	gesture.midiLow  = newMidiLow;
	gesture.midiHigh = newMidiHigh;
	gesture.cc       = newCc;

	if (uploadToHub) commandManager.invokeDirectly (neova_dash::commands::uploadConfigToHub, true);
}

void HubConfiguration::setGestureParameters (int presetNum, int gestureNum, float value0,
                                                          					float value1,
                                                          					float value2,
                                                          					float value3,
                                                          					float value4,
                                                          					float value5,
                                                          					bool uploadToHub)
{
	GestureData& gesture = getGestureData (gestureNum, presetNum);

	gesture.gestureParam0 = value0;
	gesture.gestureParam1 = value1;
	gesture.gestureParam2 = value2;
	gesture.gestureParam3 = value3;
	gesture.gestureParam4 = value4;
	gesture.gestureParam5 = value5;

	if (uploadToHub) commandManager.invokeDirectly (neova_dash::commands::uploadConfigToHub, true);
}