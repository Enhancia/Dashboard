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
    	uint8 type = HubConfiguration::none;
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
    	GestureData gesture0;
    	GestureData gesture1;
    	GestureData gesture2;
    	GestureData gesture3;
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
    enum gestureType // Possible types of gestures
    {
    	none = 0,

    	vibrato,
    	pitchBend,
    	tilt,
    	roll,
    	wave
    };

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
    //float getData (dataID dataToGet);
    
    void setPreset (const int gestureNumberToSelect);
    const int getSelectedPreset();

    GestureData& getGestureData (const int gestureNumber, const int presetNumber = selectedPreset);

    //const String getHubFirm();
    //const String getRingFirm();

private:
    //==============================================================================
	ApplicationCommandManager& commandManager = getCommandManager();
	int selectedPreset = -1;
    
	ConfigData config;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HubConfiguration)
};