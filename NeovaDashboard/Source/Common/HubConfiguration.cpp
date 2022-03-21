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
    initialiseLastGestureConfigs();
}
HubConfiguration::~HubConfiguration()
{
}

void HubConfiguration::setConfig(uint8_t * data)
{
    memcpy(&config, data, sizeof(ConfigData));  
    setPreset(config.active_preset);

    if (!configWasInitialized) configWasInitialized = true;
}

void HubConfiguration::getConfig(uint8_t * data, int buffer_size)
{
    if (buffer_size >=  sizeof(ConfigData))
    {
        memcpy(data, &config, sizeof(ConfigData));
    }
}

void HubConfiguration::flashHub()
{
    configWasChangedSinceLastFlash = false;

    commandManager.invokeDirectly (neova_dash::commands::flashHub, true);
}

bool HubConfiguration::wasConfigChangedSinceLastFlash()
{
    return configWasChangedSinceLastFlash;
}

bool HubConfiguration::getConfigWasInitialized()
{
    return configWasInitialized;
}

void HubConfiguration::notifyConfigWasChanged()
{
    if (!configWasChangedSinceLastFlash)
    {
        configWasChangedSinceLastFlash = true;
        commandManager.invokeDirectly (neova_dash::commands::allowUserToFlashHub, true);
    }
}

void HubConfiguration::resetConfigWasChanged()
{
    if (configWasChangedSinceLastFlash)
    {
        configWasChangedSinceLastFlash = false;
    }
}

void HubConfiguration::setMidiChannel (const int channelNumber, bool shouldChannelBeOn, bool isMidiInput, bool uploadToHub)
{
    uint16_t& midiChannels = isMidiInput ? getPresetData().midiChannelsIn
                                         : getPresetData().midiChannelsOut;

    if (shouldChannelBeOn)
    {
        midiChannels |= (1 << channelNumber);
    }
    else
    {
        midiChannels &= ~(1 << channelNumber);
    }

    if (uploadToHub)
    {
        commandManager.invokeDirectly (neova_dash::commands::uploadConfigToHub, true);
        notifyConfigWasChanged();
    }

}

void HubConfiguration::setMidiChannelExclusive (const int channelNumber, bool isMidiInput, bool uploadToHub)
{
    uint16_t& midiChannels = isMidiInput ? getPresetData().midiChannelsIn
                                         : getPresetData().midiChannelsOut;
    
    midiChannels = (1 << channelNumber);
    
    if (uploadToHub)
    {
        commandManager.invokeDirectly (neova_dash::commands::uploadConfigToHub, true);
        notifyConfigWasChanged();
    }

}

void HubConfiguration::toggleMidiChannel (const int channelNumber, bool isMidiInput, bool uploadToHub)
{
    uint16_t& midiChannels = isMidiInput ? getPresetData().midiChannelsIn
                                         : getPresetData().midiChannelsOut;

	if (getNumActiveMidiChannels (isMidiInput) == 1 && std::log2 (midiChannels) == channelNumber)
	{
		// TODO ajout message d'erreur log (décommenter & tester)
		// neova_dash::log::writeToLog ("Cannot have 0 active midi channels.", neova_dash::log::hubConfiguration);
		return;
	}
    
    midiChannels ^= (1 << channelNumber);

    if (uploadToHub)
    {
        commandManager.invokeDirectly (neova_dash::commands::uploadConfigToHub, true);
        notifyConfigWasChanged();
    }
}

int HubConfiguration::getMidiChannels (bool isMidiInput)
{
    return isMidiInput ? getPresetData().midiChannelsIn
                       : getPresetData().midiChannelsOut;
    
}

const int HubConfiguration::getNumActiveMidiChannels (bool isMidiInput)
{
    const uint16_t midiChannels = isMidiInput ? getPresetData().midiChannelsIn
                                              : getPresetData().midiChannelsOut;
    int count = 0;

	for (int numChannel = 0; numChannel < 16; numChannel++)
	{
		count += (midiChannels >> numChannel) & 1;
	}

	return count;
}

void HubConfiguration::setMidiThrough (bool shouldUseThrough, bool uploadToHub)
{
    config.midi_thru = (shouldUseThrough ? int (thruAdd) : int (thruOff));

    if (uploadToHub)
    {
        commandManager.invokeDirectly (neova_dash::commands::uploadConfigToHub, true);
        notifyConfigWasChanged();
    }
}


int HubConfiguration::getMidiThrough()
{
    return config.midi_thru;
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
        case reverse:  valToUpdatePtr = &gesture.reverse;  break;

        default: return;
    }

    *valToUpdatePtr = newUint8Value;
    saveGestureConfig (getGestureData (gestureNumber));

    if (uploadToHub)
    {
        commandManager.invokeDirectly (neova_dash::commands::uploadConfigToHub, true);
        notifyConfigWasChanged();
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
    saveGestureConfig (getGestureData (gestureNumber));

    if (uploadToHub)
    {
        commandManager.invokeDirectly (neova_dash::commands::uploadConfigToHub, true);
        notifyConfigWasChanged();
    }
}

void HubConfiguration::setDefaultGestureValues (const int gestureNumber, const neova_dash::gesture::GestureType type,
                                                                         const int presetNumber)
{
    using namespace neova_dash::gesture;

    setGestureData (presetNumber, gestureNumber,
                                  1,
                                  uint8_t (type),
                                  0,
                                  127,
                                  uint8_t ((type == neova_dash::gesture::none) ? 0 : findNextAvailableCC()),
                                  neova_dash::gesture::ccMidi);

    switch (type)
    {
        case vibrato:
            setGestureParameters (presetNumber, gestureNumber, VIBRATO_RANGE_DEFAULT, VIBRATO_THRESH_DEFAULT);
            break;
        case pitchBend:
            setGestureParameters (presetNumber, gestureNumber, PITCHBEND_DEFAULT_LEFTMIN,
                                                               PITCHBEND_DEFAULT_LEFTMAX,
                                                               PITCHBEND_DEFAULT_RIGHTMIN,
                                                               PITCHBEND_DEFAULT_RIGHTMAX);
            break;
        case tilt:
            setGestureParameters (presetNumber, gestureNumber, TILT_DEFAULT_MIN, TILT_DEFAULT_MAX);
            break;
        case roll:
            setGestureParameters (presetNumber, gestureNumber, ROLL_DEFAULT_MIN, ROLL_DEFAULT_MAX);
            break;
        case wave:
            setGestureParameters (presetNumber, gestureNumber, WAVE_DEFAULT_MIN, WAVE_DEFAULT_MAX);
            break;

        default:
            setGestureParameters (presetNumber, gestureNumber, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
    }

    saveGestureConfig (getGestureData (gestureNumber));
    notifyConfigWasChanged();
    commandManager.invokeDirectly (neova_dash::commands::uploadConfigToHub, true);
}

void HubConfiguration::setDefaultGestureValues (const int gestureNumber, const neova_dash::gesture::GestureType type)
{
    setDefaultGestureValues (gestureNumber, type, selectedPreset);
}

void HubConfiguration::setSavedGestureValues (const int gestureNumber, const neova_dash::gesture::GestureType type,
                                                                       const int presetNumber)
{
    if (type == neova_dash::gesture::none || lastGestureConfig[type]->on == 0) 
    {
        setDefaultGestureValues (gestureNumber, type, presetNumber);
        return;
    }

    setGestureData (presetNumber, gestureNumber,
                                  1,
                                  uint8_t (type),
                                  lastGestureConfig[type]->midiLow,
                                  lastGestureConfig[type]->midiHigh,
                                  uint8_t (findNextAvailableCC()),
                                  neova_dash::gesture::ccMidi);

    setGestureParameters (presetNumber, gestureNumber, lastGestureConfig[type]->gestureParam0,
                                                       lastGestureConfig[type]->gestureParam1,
                                                       lastGestureConfig[type]->gestureParam2,
                                                       lastGestureConfig[type]->gestureParam3,
                                                       lastGestureConfig[type]->gestureParam4,
                                                       lastGestureConfig[type]->gestureParam5);

    notifyConfigWasChanged();
    commandManager.invokeDirectly (neova_dash::commands::uploadConfigToHub, true);
}

void HubConfiguration::setSavedGestureValues (const int gestureNumber, const neova_dash::gesture::GestureType type)
{
    setSavedGestureValues (gestureNumber, type, selectedPreset);
}

void HubConfiguration::setPreset (const int presetNumberToSelect)
{
	setPreset (presetNumberToSelect, true);
}

void HubConfiguration::setPreset (const int presetNumberToSelect, bool uploadToHub)
{
	neova_dash::log::writeToLog ("Bank #" + String (presetNumberToSelect) + " : Selecting", neova_dash::log::config);

    if (presetNumberToSelect < 0 || presetNumberToSelect > neova_dash::gesture::NUM_PRESETS || presetNumberToSelect == selectedPreset) return;

    selectedPreset = presetNumberToSelect;
    selectFirstExistingGesture();
    config.active_preset = uint8_t (presetNumberToSelect);

    if (uploadToHub)
    {
        commandManager.invokeDirectly(neova_dash::commands::uploadConfigToHub, true);
        notifyConfigWasChanged();
    }
}

const int HubConfiguration::getSelectedPreset()
{
    return selectedPreset;
}

void HubConfiguration::clearPreset (const int presetNumberToClear, const bool uploadToHub)
{
    PresetData& presetToOverwrite = getPresetData (presetNumberToClear);
    presetToOverwrite = PresetData();

    if (uploadToHub)
    {
        commandManager.invokeDirectly (neova_dash::commands::uploadConfigToHub, true);
        notifyConfigWasChanged();
    }
}

void HubConfiguration::duplicatePreset (const int idToDuplicateFrom, const int idToDuplicateTo, const bool uploadToHub)
{
    const PresetData& presetToDuplicate = getPresetData (idToDuplicateFrom);
    PresetData& presetToOverwrite = getPresetData (idToDuplicateTo);

    presetToOverwrite = presetToDuplicate;

    if (uploadToHub)
    {
        commandManager.invokeDirectly (neova_dash::commands::uploadConfigToHub, true);
        notifyConfigWasChanged();
    }
}

HubConfiguration::GestureData& HubConfiguration::getGestureData (const int gestureNumber, const int presetNumber)
{
    PresetData& preset = getPresetData (presetNumber);

    switch (gestureNumber)
    {
        case (1): return preset.gestureData1;
        case (2): return preset.gestureData2;
        case (3): return preset.gestureData3;
        default:  return preset.gestureData0;
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

const String HubConfiguration::getFirmwareVersionString()
{
    return String ("HUB  : ") + getHubFirmwareVersionString() +
           String ("\nRING : ") + getRingFirmwareVersionString();
}

const String HubConfiguration::getHubFirmwareVersionString ()
{
    String hubFirm = "-";

    if (hubIsConnected)
    {
        hubFirm = "v" + String ((config.hub_firmware_version & 0xFF00) >> 8)
                      + "." + String (config.hub_firmware_version & 0x00FF);
    }

    return hubFirm;
}

const String HubConfiguration::getRingFirmwareVersionString ()
{
    String ringFirm = "-";

    if (hubIsConnected)
    {
        if (ringIsConnected)
        {
            ringFirm = "v" + String ((config.ring_firmware_version & 0xFF00) >> 8)
                           + "." + String (config.ring_firmware_version & 0x00FF);
        }
        else
        {
            ringFirm = "Disconnected";
        }
    }

    return ringFirm;
}

uint16_t HubConfiguration::getHubFirmwareVersionUint16()
{
    return config.hub_firmware_version;
}

uint16_t HubConfiguration::getRingFirmwareVersionUint16()
{
    return config.ring_firmware_version;
}

void HubConfiguration::setRingIsCharging (bool isCharging)
{
    ringIsCharging = isCharging;
}

bool HubConfiguration::getRingIsCharging()
{
    return ringIsCharging;
}

void HubConfiguration::setHubIsConnected (bool isConnected)
{
    hubIsConnected = isConnected;

    if (isConnected)
    {
        checkHUBCompatibility();
        neova_dash::log::writeToLog ("Hub connected : " + getHubFirmwareVersionString(),
                                     neova_dash::log::hubCommunication);
    }
    else
    {
        neova_dash::log::writeToLog ("Hub disconnected", neova_dash::log::hubCommunication);
    }
}

bool HubConfiguration::getHubIsConnected()
{
    return hubIsConnected;
}

void HubConfiguration::setRingIsConnected (bool isConnected)
{
    ringIsConnected = isConnected;

    if (isConnected)
    {
        checkHUBCompatibility();
    }
}

bool HubConfiguration::getRingIsConnected()
{
    return ringIsConnected;
}

bool HubConfiguration::getHubIsCompatible()
{
    return hubIsCompatible == 0;
}

int HubConfiguration::getHubIsCompatibleInt()
{
    return hubIsCompatible;
}

void HubConfiguration::setDefaultConfig()
{
    using namespace neova_dash::gesture;

    //reset preset
    setPreset(0);

    //reset gestures
    {
        // BANK 1 ====================================================================================================
        setGestureData       (0, 0, 1, vibrato, 0, 127, 0, ccMidi);
        setGestureParameters (0, 0, VIBRATO_RANGE_DEFAULT, VIBRATO_THRESH_DEFAULT);

        setGestureData       (0, 1, 1, pitchBend, 0, 127, 0, ccMidi);
	    setGestureParameters (0, 1, PITCHBEND_DEFAULT_LEFTMIN, PITCHBEND_DEFAULT_LEFTMAX, PITCHBEND_DEFAULT_RIGHTMIN, PITCHBEND_DEFAULT_RIGHTMAX);

        setGestureData       (0, 2, 1, tilt, 0, 127, 0, ccMidi);
        setGestureParameters (0, 2, TILT_DEFAULT_MIN, TILT_DEFAULT_MAX);

        setGestureData       (0, 3, 1, roll, 0, 127, 0, ccMidi);
        setGestureParameters (0, 3, ROLL_DEFAULT_MIN, ROLL_DEFAULT_MAX);

        // BANK 2, 3, 4 ====================================================================================================
        for (int i = 1; i < 4; i++ )
            for (int j = 0; j < 4; j++)
                setGestureData (i, j, 0, none, 0, 127, 0, ccMidi);
    }

    //reset MIDI in/out
    setMidiChannel(0, true, false);
    setMidiChannel(0, true, true);
    for(int i = 1; i < 16; i++)
    {
        setMidiChannel(i, false, false);
        setMidiChannel(i, false, true);
    }

    //reset midi thru
    setMidiThrough (true, false);

    commandManager.invokeDirectly (neova_dash::commands::uploadConfigToHub, true);
    resetConfigWasChanged();
    commandManager.invokeDirectly (neova_dash::commands::disallowUserToFlashHub, true);
}

void HubConfiguration::setGestureData (int presetNum, int gestureNum,
                                                      uint8 newOn,
                                                      uint8 newType,
                                                      uint8 newMidiLow,
                                                      uint8 newMidiHigh,
                                                      uint8 newCc,
                                                      uint8 newMidiType,
                                                      bool newReverse,
                                                      bool uploadToHub)
{
    GestureData& gesture = getGestureData (gestureNum, presetNum);

    gesture.on       = (newType == neova_dash::gesture::none) ? 0 : newOn;
    gesture.type     = newType;
    gesture.midiLow  = newMidiLow;
    gesture.midiHigh = newMidiHigh;
    gesture.cc       = newCc;
    gesture.midiType = ( newType == neova_dash::gesture::vibrato ||
                         newType == neova_dash::gesture::pitchBend ) ? neova_dash::gesture::pitchMidi
                                                                     : newMidiType;
    gesture.reverse = newReverse;

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
    midiLow = other.midiLow; midiHigh = other.midiHigh;
    cc = other.cc; midiType = other.midiType;

    gestureParam0 = other.gestureParam0; gestureParam1 = other.gestureParam1;
    gestureParam2 = other.gestureParam2; gestureParam3 = other.gestureParam3;
    gestureParam4 = other.gestureParam4; gestureParam5 = other.gestureParam5;
}

HubConfiguration::PresetData::PresetData (PresetData& other)
{
    midiChannelsIn  = other.midiChannelsIn;
    midiChannelsOut = other.midiChannelsOut;

    gestureData0 = other.gestureData0; gestureData1 = other.gestureData1;
    gestureData2 = other.gestureData2; gestureData3 = other.gestureData3;
}

void HubConfiguration::moveGestureToId (const int idToMoveFrom, const int idToMoveTo)
{
    if (getGestureData (idToMoveFrom).type == neova_dash::gesture::none ||
        getGestureData (idToMoveTo).type   != neova_dash::gesture::none   ) return;

	  neova_dash::log::writeToLog ("Gesture " + neova_dash::gesture::getTypeString (getGestureData (idToMoveFrom).type, true)
	  																				+ " (Id " + String (idToMoveFrom) + ") : Moving to id "
	  																				+ String (idToMoveTo), neova_dash::log::config);

    getGestureData (idToMoveTo) = getGestureData (idToMoveFrom);
    setDefaultGestureValues (idToMoveFrom, neova_dash::gesture::none);
    
    notifyConfigWasChanged();
    commandManager.invokeDirectly (neova_dash::commands::uploadConfigToHub, true);
}

void HubConfiguration::duplicateGesture (const int idToDuplicateFrom, const bool prioritizeHigherId)
{
    int idToDuplicateTo = findClosestIdToDuplicate (idToDuplicateFrom, prioritizeHigherId);
	  
	  neova_dash::log::writeToLog ("Gesture " + neova_dash::gesture::getTypeString (getGestureData (idToDuplicateFrom).type, true)
	  																				+ " (Id " + String (idToDuplicateFrom) + ") : Duplicating to id "
	  																				+ String (idToDuplicateTo), neova_dash::log::config);

    if (isIdAvailable (idToDuplicateFrom) || idToDuplicateTo == -1) return;

    getGestureData (idToDuplicateTo) = getGestureData (idToDuplicateFrom);

    notifyConfigWasChanged();
    commandManager.invokeDirectly (neova_dash::commands::uploadConfigToHub, true);
}

void HubConfiguration::swapGestures (const int firstId, const int secondId)
{
    if (firstId == secondId || firstId  < 0 || firstId  >= neova_dash::gesture::NUM_GEST ||
                               secondId < 0 || secondId >= neova_dash::gesture::NUM_GEST)
        return;

	  neova_dash::log::writeToLog ("Gesture " + neova_dash::gesture::getTypeString (getGestureData (firstId).type, true)
	  																				+ " (Id " + String (firstId) + ") : Swapping with Gesture "
                                       			+  neova_dash::gesture::getTypeString (getGestureData (secondId).type, true)
                                       			+ " (Id " + String (secondId) + ")", neova_dash::log::config);

    GestureData secondGestureCopy (getGestureData (secondId));
    
    // Replaces second gesture with first
    getGestureData (secondId) = getGestureData (firstId);

    // Copies second gesture to first Id
    getGestureData (firstId) = secondGestureCopy;

    notifyConfigWasChanged();
    commandManager.invokeDirectly (neova_dash::commands::uploadConfigToHub, true);
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

int HubConfiguration::selectFirstExistingGesture()
{
    selectedGesture = -1;

    for (int slot = 0; slot < neova_dash::gesture::NUM_GEST; slot++)
    {
        if (getGestureData (slot).type != neova_dash::gesture::none)
        {
            selectedGesture = slot;
            return selectedGesture;
        }
    }

    return selectedGesture;
}

int HubConfiguration::selectLastExistingGesture()
{
    selectedGesture = -1;

    for (int slot = neova_dash::gesture::NUM_GEST - 1; slot >= 0 ; slot--)
    {
        if (getGestureData (slot).type != neova_dash::gesture::none)
        {
            selectedGesture = slot;
            return selectedGesture;
        }
    }

    return selectedGesture;
}

int HubConfiguration::selectGestureLeft (bool loop)
{
    if (selectedGesture == -1)
    {
        selectFirstExistingGesture();
        return selectedGesture;
    }

    int newSlot = selectedGesture - neova_dash::gesture::NUM_GEST/2;

    if (newSlot >= 0 && getGestureData (newSlot).type != neova_dash::gesture::none)
    {
        selectedGesture = newSlot;
        return selectedGesture;
    }

    if (loop && newSlot < 0 && getGestureData (newSlot + neova_dash::gesture::NUM_GEST).type != neova_dash::gesture::none)
    {
        selectedGesture = newSlot + neova_dash::gesture::NUM_GEST;
        return selectedGesture;
    }

    return selectedGesture;
}

int HubConfiguration::selectGestureRight (bool loop)
{
    if (selectedGesture == -1)
    {
        selectLastExistingGesture();
        return selectedGesture;
    }

    int newSlot = selectedGesture + neova_dash::gesture::NUM_GEST/2;

    if (newSlot < neova_dash::gesture::NUM_GEST && getGestureData (newSlot).type != neova_dash::gesture::none)
    {
        selectedGesture = newSlot;
        return selectedGesture;
    }

    if (loop && newSlot >= neova_dash::gesture::NUM_GEST && getGestureData (newSlot - neova_dash::gesture::NUM_GEST).type != neova_dash::gesture::none)
    {
        selectedGesture = newSlot - neova_dash::gesture::NUM_GEST;
        return selectedGesture;
    }

    return selectedGesture;
}

int HubConfiguration::selectGestureUp (bool loop)
{
    if (selectedGesture == -1)
    {
        selectLastExistingGesture();
        return selectedGesture;
    }

    const int currentSlotRow = selectedGesture / (neova_dash::gesture::NUM_GEST/2);

    int newSlot = selectedGesture - 1;
    int newSlotRow = newSlot / (neova_dash::gesture::NUM_GEST/2);

    while (currentSlotRow == newSlotRow && newSlot >= 0)
    {
        if (getGestureData (newSlot).type != neova_dash::gesture::none)
        {
            selectedGesture = newSlot;
            return selectedGesture;
        }

        newSlot--;
        newSlotRow = newSlot / (neova_dash::gesture::NUM_GEST/2);
    }

    if (loop && (currentSlotRow != newSlotRow || newSlot < 0) && getGestureData (newSlot + neova_dash::gesture::NUM_GEST).type != neova_dash::gesture::none)
    {
        newSlot = (currentSlotRow + 1) * (neova_dash::gesture::NUM_GEST/2) - 1;

        while (getGestureData (newSlot).type == neova_dash::gesture::none)
        { 
            newSlot--;
        }

        selectedGesture = newSlot;
        return selectedGesture;
    }

    return selectedGesture;
}

int HubConfiguration::selectGestureDown (bool loop)
{
    if (selectedGesture == -1)
    {
        selectLastExistingGesture();
        return selectedGesture;
    }

    const int currentSlotRow = selectedGesture / (neova_dash::gesture::NUM_GEST/2);

    int newSlot = selectedGesture + 1;
    int newSlotRow = newSlot / (neova_dash::gesture::NUM_GEST/2);

    while (currentSlotRow == newSlotRow)
    {
        if (getGestureData (newSlot).type != neova_dash::gesture::none)
        {
            selectedGesture = newSlot;
            return selectedGesture;
        }

        newSlot++;
        newSlotRow = newSlot / (neova_dash::gesture::NUM_GEST/2);
    }

    if (loop && currentSlotRow != newSlotRow && getGestureData (newSlot - neova_dash::gesture::NUM_GEST).type != neova_dash::gesture::none)
    {
        newSlot = currentSlotRow * (neova_dash::gesture::NUM_GEST/2);

        while (getGestureData (newSlot).type == neova_dash::gesture::none)
        { 
            newSlot++;
        }

        selectedGesture = newSlot;
        return selectedGesture;
    }

    return selectedGesture;
}

const int HubConfiguration::findNextAvailableCC()
{
    Array<int> ccArray (neova_dash::midi::defaultCCs, sizeof(neova_dash::midi::defaultCCs));

    for (int defaultCc : ccArray)
    {
        bool isCcUnused = true;
        const int preset = selectedPreset;
        int gesture = -1;

        while (++gesture < neova_dash::gesture::NUM_GEST && isCcUnused)
        {
            if (isGestureActive (gesture, preset) && getGestureData (gesture, preset).cc == defaultCc)
            {
                isCcUnused = false;
            }
        }

        if (isCcUnused) return defaultCc;
    }

    return ccArray.getLast();
}

void HubConfiguration::initialiseLastGestureConfigs()
{
    for (int gestType =0; gestType < neova_dash::gesture::numTypes; gestType++)
    {
        lastGestureConfig.add (new GestureData());

        lastGestureConfig.getLast()->on = 0;
        lastGestureConfig.getLast()->type = uint8_t (gestType);
    }
}

void HubConfiguration::saveGestureConfig (const GestureData& gestureDataToSave)
{
    if (!neova_dash::gesture::isValidGestureType (gestureDataToSave.type)) return;

    *(lastGestureConfig[gestureDataToSave.type]) = gestureDataToSave;

    lastGestureConfig[gestureDataToSave.type]->on = 1;
}

void HubConfiguration::checkHUBCompatibility()
{
    if (!hubIsConnected) return;

    const int hubMajor = (config.hub_firmware_version & 0xFF00) >> 8;
    const int ringMajor = ringIsConnected ? (config.ring_firmware_version & 0xFF00) >> 8 : hubMajor;

    if (neova_dash::compatibility::COMPATIBLE_FIRM == hubMajor && ringMajor == hubMajor)
    {
        hubIsCompatible = 0;
    }

    else if (hubMajor > neova_dash::compatibility::COMPATIBLE_FIRM ||
             ringMajor > neova_dash::compatibility::COMPATIBLE_FIRM)
    {
        hubIsCompatible = 1;
    }

    else if (hubMajor < neova_dash::compatibility::COMPATIBLE_FIRM ||
             ringMajor < neova_dash::compatibility::COMPATIBLE_FIRM)
    {       
        if (ringIsConnected && ringMajor == 0 && hubMajor == neova_dash::compatibility::COMPATIBLE_FIRM)
        {
            // Specific case : HUB sent data before the ring could send its version. The check is
            // postponed until receiving a configuration with a valid ring version number.
            waitsForRingCompatibilityCheck = true;
        }
        else
        {
            hubIsCompatible = -1;
        }
    }
}

bool HubConfiguration::isWaitingForRingCompatibility()
{
    return waitsForRingCompatibilityCheck;
}

void HubConfiguration::stopWaitingForRingCompatibility()
{
    waitsForRingCompatibilityCheck = false;

    checkHUBCompatibility();
}