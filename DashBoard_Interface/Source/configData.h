/*
  ==============================================================================

    ConfigData.h
    Created: 5 Sep 2019 10:16:02am
    Author:  Enhancia Dev

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "DashCommon.h"

ApplicationCommandManager& getCommandManager();

class ConfigData
{
public:
    //==============================================================================
    enum parameterID // name of different params from the HUB
    {
        selectedPreset =0,

        vibratoGain,
        vibratoThresh,
        
        tiltLow,
        tiltHigh,
        
        pitchBendLeftLow,
        pitchBendLeftHigh,
        pitchBendRightLow,
        pitchBendRightHigh,

        

        numData
    };

    //==============================================================================
	ConfigData();
	~ConfigData();

    //==============================================================================
    void setDataAndUploadToHub (parameterID parameterToSet, float valueToSet);
    //float getData (dataID dataToGet);
    
    //const String getHubFirm();
    //const String getRingFirm();

private:
    //==============================================================================
	ApplicationCommandManager& commandManager = getCommandManager();

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ConfigData)
};