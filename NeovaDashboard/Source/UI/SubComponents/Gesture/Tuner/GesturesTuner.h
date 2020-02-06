/*
  ==============================================================================

    GesturesTuner.h
    Created: 16 Sep 2019 10:23:51am
    Author:  Enhancia Dev

  ==============================================================================
*/

#pragma once

#include "../../../../../JuceLibraryCode/JuceHeader.h"
#include "../../../../Common/DashCommon.h"
#include "../../../LookAndFeel/DashBoardLookAndFeel.h"
#include "../../../../Common/HubConfiguration.h"
#include "../../../../DataReader/DataReader.h"

#include "Tuner.h"
#include "TwoRangeTuner.h"
#include "OneRangeTuner.h"
#include "VibratoTuner.h"

//==============================================================================
class PitchBendTuner: public TwoRangeTuner
{
public:
    PitchBendTuner (HubConfiguration& config, DataReader& reader, const int gestureId)
        :   TwoRangeTuner (config, gestureId, reader.getFloatValueReference (neova_dash::data::roll), 
        				   NormalisableRange<float> (neova_dash::gesture::PITCHBEND_MIN, neova_dash::gesture::PITCHBEND_MAX),
			               Range<float> (neova_dash::ui::PITCHBEND_DISPLAY_MIN, neova_dash::ui::PITCHBEND_DISPLAY_MAX),
                           String (CharPointer_UTF8 ("\xc2\xb0"))),
            tiltValue (reader.getFloatValueReference (neova_dash::data::tilt))
    {}
    
    ~PitchBendTuner()
    {}

    void drawValueCursor (Graphics& g) override
    {
        if (/*tiltValue > && tiltValue < */ true)
        {
            TwoRangeTuner::drawValueCursor (g);
        }

        else { DBG ("Blocked PB paint"); }
    }

private:
    const float& tiltValue;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PitchBendTuner)
};


//==============================================================================
class TiltTuner: public OneRangeTuner
{
public:
    TiltTuner (HubConfiguration& config, DataReader& reader, const int gestureId)
        :   OneRangeTuner (config, gestureId, reader.getFloatValueReference (neova_dash::data::tilt), 
        				   NormalisableRange<float> (neova_dash::gesture::TILT_MIN, neova_dash::gesture::TILT_MAX),
			               Range<float> (neova_dash::ui::TILT_DISPLAY_MIN, neova_dash::ui::TILT_DISPLAY_MAX),
                           String (CharPointer_UTF8 ("\xc2\xb0")), OneRangeTuner::tilt),

            rollValue (reader.getFloatValueReference (neova_dash::data::roll))
    {}
    
    ~TiltTuner()
    {}

    void drawValueCursor (Graphics& g) override
    {
        if (rollValue < 140.0f && rollValue > -140.0f)
        {
            OneRangeTuner::drawValueCursor (g);
        }

        else { DBG ("Blocked Tilt paint"); }
    }

private:
    const float& rollValue;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TiltTuner)
};

/*
//==============================================================================
class WaveTuner: public OneRangeTuner
{
public:
    WaveTuner (HubConfiguration& config, DataReader& reader, const int gestureId)
        :   OneRangeTuner (config, gestureId, reader.getFloatValueReference (neova_dash::data::tilt),
        				   NormalisableRange<float> (neova_dash::gesture::WAVE_MIN, neova_dash::gesture::WAVE_MAX),
			               Range<float> (neova_dash::ui::WAVE_DISPLAY_MIN, neova_dash::ui::WAVE_DISPLAY_MAX),
                           String (CharPointer_UTF8 ("\xc2\xb0")), OneRangeTuner::wave)
    {}
    
    ~WaveTuner()
    {}

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveTuner)
};
*/

//==============================================================================
class RollTuner: public OneRangeTuner
{
public:
    RollTuner (HubConfiguration& config, DataReader& reader, const int gestureId)
        :   OneRangeTuner (config, gestureId, reader.getFloatValueReference (neova_dash::data::roll), 
        				   NormalisableRange<float> (neova_dash::gesture::ROLL_MIN, neova_dash::gesture::ROLL_MAX),
			               Range<float> (neova_dash::ui::ROLL_DISPLAY_MIN, neova_dash::ui::ROLL_DISPLAY_MAX),
                           String (CharPointer_UTF8 ("\xc2\xb0")), OneRangeTuner::roll),
            tiltValue (reader.getFloatValueReference (neova_dash::data::tilt))
    {}
    
    ~RollTuner()
    {}

    void drawValueCursor (Graphics& g) override
    {
        if (/*tiltValue > && tiltValue <*/ true)
        {
            OneRangeTuner::drawValueCursor (g);
        }

        else { DBG ("Blocked Roll paint"); }
    }

private:
    const float& tiltValue;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RollTuner)
};