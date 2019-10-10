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
	static constexpr int CONFIGSIZE = 516;
	
	struct GestureData // values for each gesture
    {
        GestureData() = default;
        GestureData (GestureData& other);

		uint16_t align_to_word; //to align the size of GestureData to a multiple of word

		uint8_t on = 1;
		uint8_t type = neova_dash::gesture::none;
		uint8_t midiLow = 0;
		uint8_t midiHigh = 127;
		uint8_t cc = 0;
		uint8_t midiType = uint8_t ((type == neova_dash::gesture::vibrato || type == neova_dash::gesture::pitchBend)
                                    ? neova_dash::gesture::pitchMidi : neova_dash::gesture::ccMidi);

        float gestureParam0 = 0.0f;
        float gestureParam1 = 0.0f;
        float gestureParam2 = 0.0f;
        float gestureParam3 = 0.0f;
        float gestureParam4 = 0.0f;
        float gestureParam5 = 0.0f;
    };

    struct PresetData
    {
    	GestureData gestureData0;
    	GestureData gestureData1;
    	GestureData gestureData2;
    	GestureData gestureData3;
    };

    struct ConfigData
    {
		uint16_t align_to_word; //to align the size of ConfigData to a multiple of word

		uint8_t active_preset = 0; //Ne sert à rien pour l'instant juste pour s'aligner au buffer du zub
		uint8_t midiChannel = 0;

    	PresetData presetData0;
    	PresetData presetData1;
    	PresetData presetData2;
    	PresetData presetData3;
    };

    //==============================================================================
    enum uint8DataId
    {
    	on = 0,
    	type,
    	midiLow,
    	midiHigh,
    	cc,
        midiType
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

    //==============================================================================
	HubConfiguration();
	~HubConfiguration();

    //==============================================================================
	void setConfig(uint8_t * data);
	void getConfig(uint8_t * data, int buffer_size);

    //==============================================================================
    void flashHub();
    bool wasConfigChangedSinceLastFlash();
    void notifyConfigWasChanged();

    //==============================================================================
    void setMidiChannel (const uint8 newMidiChannel, bool uploadToHub = true);

    void setUint8Value (const int gestureNumber, const uint8DataId dataId,
                                                 const uint8 newUint8Value,
                                                 bool uploadToHub = true);

    void setFloatValue (const int gestureNumber, const floatDataId dataId,
                                                 const float newFloatValue,
                                                 bool uploadToHub = true);

    void setDefaultGestureValues (const int gestureNumber, const neova_dash::gesture::GestureType, const int presetNumber);
    void setDefaultGestureValues (const int gestureNumber, const neova_dash::gesture::GestureType);
    
    void setSavedGestureValues (const int gestureNumber, const neova_dash::gesture::GestureType, const int presetNumber);
    void setSavedGestureValues (const int gestureNumber, const neova_dash::gesture::GestureType);

    void moveGestureToId (const int idToMoveFrom, const int idToMoveTo);
    void duplicateGesture (const int idToDuplicateFrom, const bool prioritizeHigherId = true);
    bool canDuplicate();
    void swapGestures (const int id1, const int id2);
    
    //==============================================================================
    void setPreset (const int gestureNumberToSelect);
	void setPreset(const int gestureNumberToSelect, bool uploadToHub);
    const int getSelectedPreset();

    PresetData& getPresetData (const int presetNumber);
    PresetData& getPresetData();

    //==============================================================================
    GestureData& getGestureData (const int gestureNumber, const int presetNumber);
	GestureData& getGestureData (const int gestureNumber);

    bool isGestureActive (const int gestureNumber, const int presetNumber);
    bool isGestureActive (const int gestureNumber);

    //==============================================================================
    void setSelectedGesture (const int gestureToSelect);
    void selectFirstExistingGesture();
    const int getSelectedGesture();

    //==============================================================================
    //const neova_dash::gesuture::GestureType getGestureType (const int gestureNumber, const int presetNumber);
    //const neova_dash::gesuture::GestureType getGestureType (const int gestureNumber);

    //const String getHubFirm();
    //const String getRingFirm();

    //==============================================================================
    void setRingIsCharging (bool isCharging);
    bool getRingIsCharging();

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
    bool isIdAvailable (const int idToCheck);
    const int findAvailableUndefinedCC();

    //==============================================================================
	ApplicationCommandManager& commandManager = getCommandManager();
	int selectedPreset = 0;
    int selectedGesture = -1;

    //==============================================================================
    bool ringIsCharging = false;

    //==============================================================================
	ConfigData config;

    //==============================================================================
    OwnedArray<GestureData> lastGestureConfig;
    void initialiseLastGestureConfigs();
    void saveGestureConfig (const GestureData& gestureDataToSave);
    bool configWasChangedSinceLastFlash = false;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HubConfiguration)
};