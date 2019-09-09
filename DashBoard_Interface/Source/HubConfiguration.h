/*
  ==============================================================================

    HubConfiguration.h
    Created: 5 Sep 2019 10:16:02am
    Author:  Enhancia Dev

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "DashCommon.h"

ApplicationCommandManager& getCommandManager();

class HubConfiguration
{
public:
    //==============================================================================
    struct GestureData // values for each gesture
    {
    	uint8 on = 1;
    	uint8 type = neova_dash::gesture::none;
    	uint8 midiLow = 0;
    	uint8 midiHigh = 127;
    	uint8 cc = 0;

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
    	uint8 midiChannel = 0;

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
    	cc
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
    void setMidiChannelAndUpload (const uint8 newMidiChannel);
    void setUint8ValueAndUpload (const int gestureNumber, const uint8DataId dataId, const uint8 newUint8Value);
    void setFloatValueAndUpload (const int gestureNumber, const floatDataId dataId, const float newFloatValue);

    void setDefaultGestureValues (const int gestureNumber, const neova_dash::gesture::GestureType, const int presetNumber);
    void setDefaultGestureValues (const int gestureNumber, const neova_dash::gesture::GestureType);
    
    //==============================================================================
    void setPreset (const int gestureNumberToSelect);
    const int getSelectedPreset();

    GestureData& getGestureData (const int gestureNumber, const int presetNumber);
	GestureData& getGestureData (const int gestureNumber);

    //const neova_dash::gesuture::GestureType getGestureType (const int gestureNumber, const int presetNumber);
    //const neova_dash::gesuture::GestureType getGestureType (const int gestureNumber);

    //const String getHubFirm();
    //const String getRingFirm();

private:
    //==============================================================================
    void setDefaultConfig(); // TEST PURPOSE!! Maybe TO DELETE since the dashboard should match the hub anyways..

    void setGestureData (int presetNum, int gestureNum,
                                        uint8 newOn,
                                        uint8 newType,
                                        uint8 newMidiLow,
                                        uint8 newMidiHigh,
                                        uint8 newCc,
                                        bool uploadToHub = false);

    void setGestureParameters (int presetNum, int gestureNum, float value0,
                                                              float value1,
                                                              float value2 = 0.0f,
                                                              float value3 = 0.0f,
                                                              float value4 = 0.0f,
                                                              float value5 = 0.0f,
                                                              bool uploadToHub = false);

    //==============================================================================
	ApplicationCommandManager& commandManager = getCommandManager();
	int selectedPreset = -1;
    
	ConfigData config;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HubConfiguration)
};