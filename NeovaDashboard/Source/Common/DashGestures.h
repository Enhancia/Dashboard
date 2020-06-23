/*
  ==============================================================================

    DashGestures.h
    Created: 7 Oct 2019 2:58:27pm
    Author:  Enhancia Dev

  ==============================================================================
*/

#pragma once
#include "../../JuceLibraryCode/JuceHeader.h"

namespace neova_dash
{
	namespace gesture
    {
        const int NUM_GEST = 4;
        const int NUM_PRESETS = 4;

        enum GestureType
        {
            vibrato =0,
            pitchBend,
            tilt,
            roll,
            wave,

            numTypes,
            none
        };

        enum CurveType
        {
            linear,
            log,
            exp,
            pow2,
            pow3
        };

        enum MidiType
        {
            pitchMidi =0,
            ccMidi,
            afterTouchMidi
        };

        // gesture max range values
        const float VIBRATO_RANGE_MAX  = 100.0f;
        const float VIBRATO_THRESH_MAX = 500.0f;
        const float PITCHBEND_MIN = -180.0f;
        const float PITCHBEND_MAX =  180.0f;
        const float TILT_MIN = -180.0f;
        const float TILT_MAX =  180.0f;
        const float ROLL_MIN = -180.0f;
        const float ROLL_MAX =  180.0f;
        const float WAVE_MIN = -180.0f;
        const float WAVE_MAX =  180.0f;

        // Gesture default values
        const float VIBRATO_RANGE_DEFAULT  = 80.0f;
        const float VIBRATO_THRESH_DEFAULT = 40.0f;
        const float PITCHBEND_DEFAULT_LEFTMIN = -50.0f;
        const float PITCHBEND_DEFAULT_LEFTMAX = -20.0f;
        const float PITCHBEND_DEFAULT_RIGHTMIN = 30.0f;
        const float PITCHBEND_DEFAULT_RIGHTMAX = 60.0f;
        const float TILT_DEFAULT_MIN =  0.0f;
        const float TILT_DEFAULT_MAX =  50.0f;
        const float ROLL_DEFAULT_MIN = -50.0f;
        const float ROLL_DEFAULT_MAX =  50.0f;
        const float WAVE_DEFAULT_MIN = -50.0f;
        const float WAVE_DEFAULT_MAX =  50.0f;

        // Helper methods for gestures
        extern GestureType intToGestureType (const int typeInt);
        extern const String getTypeString (const GestureType type, const bool withSpacingAndCase);
        extern const String getTypeString (const int typeInt, const bool withSpacingAndCase);
        extern const String getDescriptionString (const GestureType type);

        extern int computeMidiValue (int type, float value, int rangeLow,
                                                            int rangeHigh,
                                                            int reverse,
                                                            float parameter0,
		 													float parameter1,
		 													float parameter2 = 0.0f,
		 													float parameter3 = 0.0f,
		 													float parameter4 = 0.0f,
		 													float parameter5 = 0.0f);

        extern bool isValueOutOfGestureRange (int type, float value);

        static int map (float val, float minVal, float maxVal, int minNew, int maxNew);
        static int mapInt (int val, int minVal, int maxVal, int minNew, int maxNew);
    };
}