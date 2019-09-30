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

void HubConfiguration::setConfig(uint8_t * data)
{
	/*
	config.align_to_word = *(uint16_t *)data;
	config.active_preset = *(uint8_t *)(data+2);
	config.midiChannel = *(uint8_t *)(data + 3);
	config.presetData0.gestureData0.align_to_word = *(uint16_t *)(data + 4);
	config.presetData0.gestureData0.on = *(uint8_t *)(data + 6);
	config.presetData0.gestureData0.type = *(uint8_t *)(data + 7);
	config.presetData0.gestureData0.midiLow = *(uint8_t *)(data + 8);
	config.presetData0.gestureData0.midiHigh = *(uint8_t *)(data + 9);
	config.presetData0.gestureData0.cc = *(uint8_t *)(data + 10);
	config.presetData0.gestureData0.midiType = *(uint8_t *)(data + 11);
	config.presetData0.gestureData0.gestureParam0 = *(float *)(data + 12);
	config.presetData0.gestureData0.gestureParam1 = *(float *)(data + 16);
	config.presetData0.gestureData0.gestureParam2 = *(float *)(data + 20);
	config.presetData0.gestureData0.gestureParam3 = *(float *)(data + 24);
	config.presetData0.gestureData0.gestureParam4 = *(float *)(data + 28);
	config.presetData0.gestureData0.gestureParam5 = *(float *)(data + 32);
	*/
	memcpy(&config, data, sizeof(ConfigData));	
}

void HubConfiguration::getConfig(uint8_t * data, int buffer_size)
{
	if (buffer_size >=  sizeof(ConfigData))
	{
		memcpy(data, &config, sizeof(ConfigData));
	}
}

void HubConfiguration::setMidiChannel (const uint8 newMidiChannel, bool uploadToHub)
{
	config.midiChannel = newMidiChannel;

	if (uploadToHub)
	{
		commandManager.invokeDirectly (neova_dash::commands::uploadConfigToHub, true);
	}
}

void HubConfiguration::setUint8Value (const int gestureNumber, const uint8DataId dataId,
													           const uint8 newUint8Value,
													           bool uploadToHub)
{
	GestureData& gesture = getGestureData (gestureNumber);
	uint8* valToUpdatePtr;

	switch (dataId)
	{
		case on:       valToUpdatePtr = &gesture.on;       break;
		case type:     valToUpdatePtr = &gesture.type;     break;
		case midiLow:  valToUpdatePtr = &gesture.midiLow;  break;
		case midiHigh: valToUpdatePtr = &gesture.midiHigh; break;
		case cc:       valToUpdatePtr = &gesture.cc;       break;
		case midiType: valToUpdatePtr = &gesture.midiType; break;

		default: return;
	}

	*valToUpdatePtr = newUint8Value;

	if (uploadToHub)
	{
		commandManager.invokeDirectly (neova_dash::commands::uploadConfigToHub, true);
	}
}

void HubConfiguration::setFloatValue (const int gestureNumber, const floatDataId dataId,
													           const float newFloatValue,
													           bool uploadToHub)
{
	GestureData& gesture = getGestureData (gestureNumber);
	float* valToUpdatePtr;

	switch (dataId)
	{
		case gestureParam0:  valToUpdatePtr = &gesture.gestureParam0;  break;
		case gestureParam1:  valToUpdatePtr = &gesture.gestureParam1;  break;
		case gestureParam2:  valToUpdatePtr = &gesture.gestureParam2;  break;
		case gestureParam3:  valToUpdatePtr = &gesture.gestureParam3;  break;
		case gestureParam4:  valToUpdatePtr = &gesture.gestureParam4;  break;
		case gestureParam5:  valToUpdatePtr = &gesture.gestureParam5;  break;

		default: return;
	}

	*valToUpdatePtr = newFloatValue;

	if (uploadToHub)
	{
		commandManager.invokeDirectly (neova_dash::commands::uploadConfigToHub, true);
	}
}

void HubConfiguration::setDefaultGestureValues (const int gestureNumber, const neova_dash::gesture::GestureType type,
																		 const int presetNumber)
{
	using namespace neova_dash::gesture;

	setGestureData (presetNumber, gestureNumber, 1, uint8 (type), 0, 127, 0, neova_dash::gesture::ccMidi);

	switch (type)
	{
		case vibrato:
			setGestureParameters (presetNumber, gestureNumber, 40.0f, 400.0f);
			break;
		case pitchBend:
			setGestureParameters (presetNumber, gestureNumber, -50.0f, -20.0f, 30.0f, 60.0f);
			break;
		case tilt:
			setGestureParameters (presetNumber, gestureNumber, 0.0f, 50.0f);
			break;
		case roll:
			setGestureParameters (presetNumber, gestureNumber, -50.0f, 50.0f);
			break;
		case wave:
			setGestureParameters (presetNumber, gestureNumber, -50.0f, 50.0f);
			break;

		default:
			setGestureParameters (presetNumber, gestureNumber, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
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
	selectFirstExistingGesture();
	
	commandManager.invokeDirectly (neova_dash::commands::uploadConfigToHub, true);	
}

void HubConfiguration::setPreset(const int gestureNumberToSelect, bool uploadToHub)
{
	if (gestureNumberToSelect < 0 || gestureNumberToSelect > 4 || gestureNumberToSelect == selectedPreset) return;

	selectedPreset = gestureNumberToSelect;
	selectFirstExistingGesture();

	if (uploadToHub)
	{
		commandManager.invokeDirectly(neova_dash::commands::uploadConfigToHub, true);
	}
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
		case (1): return preset->gestureData1;
		case (2): return preset->gestureData2;
		case (3): return preset->gestureData3;
		default:  return preset->gestureData0;
	}
}

HubConfiguration::GestureData& HubConfiguration::getGestureData (const int gestureNumber)
{
	return getGestureData (gestureNumber, selectedPreset);
}

HubConfiguration::PresetData& HubConfiguration::getPresetData (const int presetNumber)
{
	switch (presetNumber)
	{
		case (1): return config.presetData1;
		case (2): return config.presetData2;
		case (3): return config.presetData3;
		default:  return config.presetData0;
	}
}

HubConfiguration::PresetData& HubConfiguration::getPresetData()
{
	return getPresetData (selectedPreset);
}

bool HubConfiguration::isGestureActive (const int gestureNumber, const int presetNumber)
{
	return getGestureData (gestureNumber, presetNumber).on != 0;
}

bool HubConfiguration::isGestureActive (const int gestureNumber)
{
	return isGestureActive (gestureNumber, selectedPreset);
}

void HubConfiguration::setSelectedGesture (const int gestureToSelect)
{
	if (gestureToSelect < -1 || gestureToSelect > neova_dash::gesture::NUM_GEST
							 || gestureToSelect == selectedGesture) return;

	selectedGesture = gestureToSelect;
}

const int HubConfiguration::getSelectedGesture()
{
	return selectedGesture;
}

void HubConfiguration::setDefaultConfig()
{
	setGestureData       (0, 0, 1, neova_dash::gesture::vibrato, 0, 127, 0, neova_dash::gesture::ccMidi);
	setGestureParameters (0, 0, 400.0f, 40.0f);
	setGestureData       (0, 1, 1, neova_dash::gesture::pitchBend, 0, 127, 0, neova_dash::gesture::ccMidi);
	setGestureParameters (0, 1, -50.0f, -20.0f, 30.0f, 60.0f);
	setGestureData       (0, 2, 0, neova_dash::gesture::tilt, 0, 127, 0, neova_dash::gesture::ccMidi);
	setGestureParameters (0, 2, 0.0f, 50.0f);
	setGestureData       (0, 3, 0, neova_dash::gesture::roll, 0, 127, 0, neova_dash::gesture::ccMidi);
	setGestureParameters (0, 3, -30.0f, 30.0f);

	setGestureData       (1, 0, 1, neova_dash::gesture::vibrato, 0, 127, 0, neova_dash::gesture::ccMidi);
	setGestureParameters (1, 0, 450.0f, 20.0f);
	setGestureData       (1, 1, 1, neova_dash::gesture::tilt, 0, 127, 0, neova_dash::gesture::ccMidi);
	setGestureParameters (1, 1, 0.0f, 80.0f);
	setGestureData       (1, 2, 0, neova_dash::gesture::none, 0, 127, 0, neova_dash::gesture::ccMidi);
	setGestureData       (1, 3, 1, neova_dash::gesture::roll, 0, 127, 0, neova_dash::gesture::ccMidi);
	setGestureParameters (1, 3, -50.0f, 20.0f);

	setGestureData       (2, 0, 0, neova_dash::gesture::none, 0, 127, 0, neova_dash::gesture::ccMidi);
	setGestureData       (2, 1, 0, neova_dash::gesture::none, 0, 127, 0, neova_dash::gesture::ccMidi);
	setGestureData       (2, 2, 1, neova_dash::gesture::roll, 0, 127, 0, neova_dash::gesture::ccMidi);
	setGestureParameters (2, 2, -10.0f, 90.0f);
	setGestureData       (2, 3, 1, neova_dash::gesture::wave, 0, 127, 0, neova_dash::gesture::ccMidi);
	setGestureParameters (2, 3, -50.0f, 20.0f);

	setGestureData       (3, 0, 0, neova_dash::gesture::none, 0, 127, 0, neova_dash::gesture::ccMidi);
	setGestureData       (3, 1, 0, neova_dash::gesture::none, 0, 127, 0, neova_dash::gesture::ccMidi);
	setGestureData       (3, 2, 1, neova_dash::gesture::tilt, 0, 127, 0, neova_dash::gesture::ccMidi);
	setGestureParameters (3, 2, -10.0f, -5.0f);
	setGestureData       (3, 3, 0, neova_dash::gesture::none, 0, 127, 0, neova_dash::gesture::ccMidi);

	//commandManager.invokeDirectly (neova_dash::commands::uploadConfigToHub, true);
}

void HubConfiguration::setGestureData (int presetNum, int gestureNum,
                                   				      uint8 newOn,
                                   				      uint8 newType,
                                   				      uint8 newMidiLow,
                                   				      uint8 newMidiHigh,
                                   				      uint8 newCc,
                                        			  uint8 newMidiType,
                                   				      bool uploadToHub)
{
	GestureData& gesture = getGestureData (gestureNum, presetNum);

	gesture.on       = newOn;
	gesture.type     = newType;
	gesture.midiLow  = newMidiLow;
	gesture.midiHigh = newMidiHigh;
	gesture.cc       = newCc;
	gesture.midiType = ( newType == neova_dash::gesture::vibrato ||
						 newType == neova_dash::gesture::pitchBend ) ? neova_dash::gesture::pitchMidi
																	 : newMidiType;

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

HubConfiguration::GestureData::GestureData (GestureData& other)
{
    on = other.on; type = other.type;
    midiLow = other.midiLow; midiHigh = other.midiHigh; cc = other.cc;

    gestureParam0 = other.gestureParam0; gestureParam1 = other.gestureParam1;
    gestureParam2 = other.gestureParam2; gestureParam3 = other.gestureParam3;
    gestureParam4 = other.gestureParam4; gestureParam5 = other.gestureParam5;
}

void HubConfiguration::moveGestureToId (const int idToMoveFrom, const int idToMoveTo)
{
    if (getGestureData (idToMoveFrom).type == neova_dash::gesture::none ||
    	getGestureData (idToMoveTo).type   != neova_dash::gesture::none   ) return;

	getGestureData (idToMoveTo) = getGestureData (idToMoveFrom);
    setDefaultGestureValues (idToMoveFrom, neova_dash::gesture::none);
    
    commandManager.invokeDirectly (neova_dash::commands::uploadConfigToHub, true);
}

void HubConfiguration::duplicateGesture (const int idToDuplicateFrom, const bool prioritizeHigherId)
{
    int idToDuplicateTo = findClosestIdToDuplicate (idToDuplicateFrom, prioritizeHigherId);

    if (isIdAvailable (idToDuplicateFrom) || idToDuplicateTo == -1) return;

	getGestureData (idToDuplicateTo) = getGestureData (idToDuplicateFrom);

    commandManager.invokeDirectly (neova_dash::commands::uploadConfigToHub, true);
}

void HubConfiguration::swapGestures (const int firstId, const int secondId)
{
    if (firstId == secondId || firstId  < 0 || firstId  >= neova_dash::gesture::NUM_GEST ||
                               secondId < 0 || secondId >= neova_dash::gesture::NUM_GEST)
        return;

    GestureData secondGestureCopy (getGestureData (secondId));
    
    // Replaces second gesture with first
    getGestureData (secondId) = getGestureData (firstId);

    // Copies second gesture to first Id
    getGestureData (firstId) = secondGestureCopy;

    commandManager.invokeDirectly (neova_dash::commands::updateInterfaceLEDs, true);
}

int HubConfiguration::findClosestIdToDuplicate (int idToDuplicateFrom, bool prioritizeHigherId)
{
    int lowerAvailableId = -1;

    for (int offset=1;
        (idToDuplicateFrom + offset < neova_dash::gesture::NUM_GEST || idToDuplicateFrom - offset >= 0);
        offset++)
    {
        if (isIdAvailable (idToDuplicateFrom + offset))
        {
            return idToDuplicateFrom + offset;
        }

        if (lowerAvailableId == -1 && isIdAvailable (idToDuplicateFrom - offset))
        {
            if (!prioritizeHigherId)
            {
                return idToDuplicateFrom - offset;
            }

            lowerAvailableId = idToDuplicateFrom - offset;
        }
    }

    return lowerAvailableId;
}

bool HubConfiguration::canDuplicate()
{
	for (int slot = 0; slot < neova_dash::gesture::NUM_GEST; slot++)
	{
		if (isIdAvailable (slot)) return true;
	}

	return false;
}

bool HubConfiguration::isIdAvailable (const int idToCheck)
{
	if (idToCheck < 0 || idToCheck >= neova_dash::gesture::NUM_GEST) return false;

	return (getGestureData (idToCheck).type == neova_dash::gesture::none);
}

void HubConfiguration::selectFirstExistingGesture()
{
	selectedGesture = -1;

    for (int slot = 0; slot < neova_dash::gesture::NUM_GEST; slot++)
    {
        if (getGestureData (slot).type != neova_dash::gesture::none)
        {
            selectedGesture = slot;
            return;
        }
    }
}