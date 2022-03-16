/*
  ==============================================================================

    HubConfiguration.h
    Created: 5 Sep 2019 10:16:02am
    Author:  Enhancia Dev

  ==============================================================================
*/

#pragma once

#include "../../JuceLibraryCode/JuceHeader.h"
#include "DashCommon.h"



ApplicationCommandManager& getCommandManager();

class HubConfiguration
{
public:
    //==============================================================================
	static constexpr int CONFIGSIZE = 536;
	
	struct GestureData // values for each gesture
    {
        GestureData() = default;
        GestureData (GestureData& other);

        uint8_t typeFunc = 0;
        uint8_t reverse = 0;
		uint8_t on = 0;
		uint8_t type = neova_dash::gesture::none;
		uint8_t midiLow = 0;
		uint8_t midiHigh = 127;
		uint8_t cc = 0;
		uint8_t midiType = static_cast<uint8_t>((type == neova_dash::gesture::vibrato || type == neova_dash::gesture::pitchBend)
                                                    ? neova_dash::gesture::pitchMidi
                                                    : neova_dash::gesture::ccMidi);

        float gestureParam0 = 0.0f;
        float gestureParam1 = 0.0f;
        float gestureParam2 = 0.0f;
        float gestureParam3 = 0.0f;
        float gestureParam4 = 0.0f;
        float gestureParam5 = 0.0f;
    };

    struct PresetData
    {
        PresetData() = default;
        PresetData (PresetData& other);

        uint16_t midiChannelsIn = 0xffff;
        uint16_t midiChannelsOut = 1;

    	GestureData gestureData0;
    	GestureData gestureData1;
    	GestureData gestureData2;
    	GestureData gestureData3;
    };

    struct ConfigData
    {
		uint16_t align_to_word; //to align the size of ConfigData to a multiple of word

		uint16_t hub_firmware_version; ////8 MSB => Major version / 8 => LSB Minor version. ex : 0x0001 = v0.1
		uint16_t ring_firmware_version;

		uint8_t active_preset = 0; //Ne sert à rien pour l'instant juste pour s'aligner au buffer du zub
		uint8_t midi_thru = static_cast<int>(thruOff);

    	PresetData presetData0;
    	PresetData presetData1;
    	PresetData presetData2;
    	PresetData presetData3;
    };

    //==============================================================================
    enum uint8DataId
    {
    	typeFunc =0,
        reverse,
        on,
    	type,
    	midiLow,
    	midiHigh,
    	cc,
        midiType,
    };

    enum floatDataId
    {
    	gestureParam0 = 0,
    	gestureParam1,
    	gestureParam2,
    	gestureParam3,
    	gestureParam4,
    	gestureParam5
    };

    enum midiThruId
    {
        thruAdd= 0,
        thruOff,
        thruPure
    };

    //==============================================================================
	HubConfiguration();
	~HubConfiguration();

    //==============================================================================
	void setConfig(uint8_t * data);
	void getConfig(uint8_t * data, int buffer_size) const;

    //==============================================================================
    void flashHub();
    bool wasConfigChangedSinceLastFlash() const;
    bool getConfigWasInitialized() const;
    void notifyConfigWasChanged();
    void resetConfigWasChanged();

    //==============================================================================
    void setUint8Value (int gestureNumber, uint8DataId dataId,
                        uint8 newUint8Value,
                        bool uploadToHub = true);

    void setFloatValue (int gestureNumber, floatDataId dataId,
                        float newFloatValue,
                        bool uploadToHub = true);

    void setDefaultGestureValues (int gestureNumber, neova_dash::gesture::GestureType, int presetNumber);
    void setDefaultGestureValues (int gestureNumber, neova_dash::gesture::GestureType);
    
    void setSavedGestureValues (int gestureNumber, neova_dash::gesture::GestureType, int presetNumber);
    void setSavedGestureValues (int gestureNumber, neova_dash::gesture::GestureType);

    void moveGestureToId (int idToMoveFrom, int idToMoveTo);
    void duplicateGesture (int idToDuplicateFrom, bool prioritizeHigherId = true);
    bool canDuplicate();
    void swapGestures (int id1, int id2);
    
    //==============================================================================
    void setPreset (int presetNumberToSelect);
	void setPreset (int presetNumberToSelect, bool uploadToHub);
    const int getSelectedPreset() const;
    void clearPreset (int presetNumberToClear, bool uploadToHub = true);
    void duplicatePreset (int idToDuplicateFrom, int idToDuplicateTo, bool uploadToHub = true);

    PresetData& getPresetData (int presetNumber);
    PresetData& getPresetData();

    //==============================================================================
    GestureData& getGestureData (int gestureNumber, int presetNumber);
	GestureData& getGestureData (int gestureNumber);

    bool isGestureActive (int gestureNumber, int presetNumber);
    bool isGestureActive (int gestureNumber);

    //==============================================================================
    void setSelectedGesture (int gestureToSelect);
    int selectFirstExistingGesture();
    int selectLastExistingGesture();
    int selectGestureLeft  (bool loop = false);
    int selectGestureRight (bool loop = false);
    int selectGestureUp    (bool loop = false);
    int selectGestureDown  (bool loop = false);
    const int getSelectedGesture() const;

    //==============================================================================
    void setMidiChannel (int channelNumber, bool shouldChannelBeOn, bool isMidiInput = false, bool uploadToHub = true);
    void setMidiChannelExclusive (int channelNumber, bool isMidiInput = false, bool uploadToHub = true);
    void toggleMidiChannel (int channelNumber, bool isMidiInput = false, bool uploadToHub = true);
    int getMidiChannels (bool isMidiInput = false);
    const int getNumActiveMidiChannels (bool isMidiInput = false);
    void setMidiThrough (bool shouldUseThrough, bool uploadToHub = true);
    int getMidiThrough() const;


    //==============================================================================
    //const neova_dash::gesuture::GestureType getGestureType (const int gestureNumber, const int presetNumber);
    //const neova_dash::gesuture::GestureType getGestureType (const int gestureNumber);

    const String getFirmwareVersionString();
    const String getHubFirmwareVersionString();
    const String getRingFirmwareVersionString();
    uint16_t getHubFirmwareVersionUint16() const;
    uint16_t getRingFirmwareVersionUint16() const;

    //==============================================================================
    void setHubIsConnected (bool isConnected);
    bool getHubIsConnected() const;
    void setRingIsCharging (bool isCharging);
    bool getRingIsCharging() const;
    void setRingIsConnected (bool isConnected);
    bool getRingIsConnected() const;

    bool getHubIsCompatible() const;
    int  getHubIsCompatibleInt() const;
    
    bool isWaitingForRingCompatibility() const;
    void stopWaitingForRingCompatibility();

private:
    //==============================================================================
    void setDefaultConfig(); // TEST PURPOSE!! Maybe TO DELETE since the dashboard should match the hub anyways..

    void setGestureData (int presetNum, int gestureNum,
                                        uint8 newOn,
                                        uint8 newType,
                                        uint8 newMidiLow,
                                        uint8 newMidiHigh,
                                        uint8 newCc,
                                        uint8 newMidiType,
                                        bool uploadToHub = false);


    void setGestureParameters (int presetNum, int gestureNum, float value0,
                                                              float value1,
                                                              float value2 = 0.0f,
                                                              float value3 = 0.0f,
                                                              float value4 = 0.0f,
                                                              float value5 = 0.0f,
                                                              bool uploadToHub = false);

    int findClosestIdToDuplicate (int idToDuplicateFrom, bool prioritizeHigherId);
    bool isIdAvailable (int idToCheck);
    const int findNextAvailableCC();

    //==============================================================================
	ApplicationCommandManager& commandManager = getCommandManager();
	int selectedPreset = 0;
    int selectedGesture = -1;

    //==============================================================================
    bool hubIsConnected = false;
    bool ringIsCharging = false;
    bool ringIsConnected = false;

    //==============================================================================
    int hubIsCompatible = 0;
    bool waitsForRingCompatibilityCheck = false;

    //==============================================================================
	ConfigData config;
    OwnedArray<GestureData> lastGestureConfig;
    void initialiseLastGestureConfigs();
    void saveGestureConfig (const GestureData& gestureDataToSave) const;
    bool configWasChangedSinceLastFlash = false;
    bool configWasInitialized = false;

    //==============================================================================
    void checkHUBCompatibility();

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HubConfiguration)
};