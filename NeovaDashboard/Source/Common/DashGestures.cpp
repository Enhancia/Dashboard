/*
  ==============================================================================

    DashGestures.cpp
    Created: 7 Oct 2019 3:01:29pm
    Author:  Enhancia Dev

  ==============================================================================
*/

#include "../../JuceLibraryCode/JuceHeader.h"
#include "DashGestures.h"

namespace neova_dash
{
namespace gesture
{
    int computeMidiValue (int type, float value, int rangeLow,
                                                 int rangeHigh,
                                                 int reverse,
                                                 float parameter0,
                                                 float parameter1,
                                                 float parameter2,
                                                 float parameter3,
                                                 float parameter4,
                                                 float parameter5)
    {
        if (type == numTypes) return 0;

        int midiVal = 0;
        bool reversed = reverse != 0;

        if (type == int (vibrato))
        {
            const float gain = parameter0;
            const float threshold = parameter1;
            const float intensity = parameter2;
            const float maxRange = float (VIBRATO_RANGE_MAX);

            midiVal = (intensity < threshold) ? 64
                                              : map (value, -(maxRange - gain), (maxRange + 0.01f - gain), 0, 127);

            /*
            DBG ("Vibrato : " << " Send ? " << (intensity < threshold ? "N":"Y")
                              << " | Val " << value
                              << " | Int " << intensity
                              << " | Midi Val : " << midiVal);*/
        }
        else if (type == int (pitchBend))
        {
            // Right side
            if (value >= parameter2 && value < 140.0f)
            {
                midiVal = (parameter2 == parameter3) ? 64
                                                     : map (value, parameter2, parameter3, 64, 127);
            }
            
            // Left side
            else if (value < parameter1 && value > -140.0f)
            {
                midiVal = (parameter0 == parameter1) ? 64
                                                     : map (value, parameter0, parameter1, 0, 63);
            }

            else midiVal = 64;
        }
        else if (type == int (tilt))
        {
            midiVal = map (value, parameter0, parameter1, 0, 127);
        }
        else if (type == int (roll))
        {
            midiVal = map (value, parameter0, parameter1, 0, 127);            
        }
        else if (type == int (wave))
        {
            midiVal = 0;
        }

        if (reversed) midiVal = 127 - midiVal;

        return mapInt (midiVal, 0, 127, rangeLow, rangeHigh);
    }

    bool isValueOutOfGestureRange (int type, float value)
    {
        if (type == int (vibrato))
        {
            return (value < -VIBRATO_RANGE_MAX || value > VIBRATO_RANGE_MAX);
        }
        if (type == int (pitchBend))
        {
            return (value < PITCHBEND_MIN || value > PITCHBEND_MAX);
        }
        if (type == int (tilt))
        {
            return (value < TILT_MIN || value > TILT_MAX);
        }
        if (type == int (roll))
        {
            return (value < ROLL_MIN || value > ROLL_MAX);
        }
        if (type == int (wave))
        {
            return (value < WAVE_MIN || value > WAVE_MAX);
        }

        return true; // default
    }

    int map (float val, float minVal, float maxVal, int minNew, int maxNew)
    {
        if (minVal == maxVal && val == minVal) return minNew;

        if (val < minVal) return minNew;
        if (val > maxVal) return maxNew;

        return (minNew + int ((maxNew - minNew)*(val - minVal)/(maxVal-minVal)));
    }

    int mapInt (int val, int minVal, int maxVal, int minNew, int maxNew)
    {
        if (minVal == maxVal && val == minVal) return minNew;
        else if (minVal == minNew && maxVal == maxNew) return val;

        if (val < minVal) return minNew;
        if (val > maxVal) return maxNew;

        return (minNew + (maxNew - minNew)*(val - minVal)/(maxVal-minVal));
    }

    neova_dash::gesture::GestureType intToGestureType (const int typeInt)
    {
        switch (typeInt)
        {
            case int (neova_dash::gesture::vibrato):   return neova_dash::gesture::vibrato;
            case int (neova_dash::gesture::pitchBend): return neova_dash::gesture::pitchBend;
            case int (neova_dash::gesture::tilt):      return neova_dash::gesture::tilt;
            case int (neova_dash::gesture::roll):      return neova_dash::gesture::roll;
            case int (neova_dash::gesture::wave):      return neova_dash::gesture::wave;
            default:                                   return neova_dash::gesture::none;
        }
    }

    bool isValidGestureType (const int typeInt)
    {
        return (typeInt >= 0 && typeInt < int (neova_dash::gesture::numTypes));
    }

    bool isValidGestureType (const neova_dash::gesture::GestureType type)
    {
        return (type != neova_dash::gesture::none && type != neova_dash::gesture::numTypes);
    }

    const String getTypeString (const neova_dash::gesture::GestureType type, const bool withSpacingAndCase)
    {
        switch (type)
        {
            case neova_dash::gesture::vibrato:
                return withSpacingAndCase ? "Vibrato"  : "vibrato";
            
            case neova_dash::gesture::pitchBend:
                return withSpacingAndCase ? "Pitch Bend" : "pitchBend" ;
            
            case neova_dash::gesture::tilt:
                return withSpacingAndCase ? "Tilt"  : "tilt";
            
            case neova_dash::gesture::roll:
                return withSpacingAndCase ? "Roll"  : "roll";
            
            case neova_dash::gesture::wave:
                return withSpacingAndCase ? "Wave"  : "wave";

            default:
                return "none";
        }
    }
    const String getTypeString (const int typeInt, const bool withSpacingAndCase)
    {
        return getTypeString (intToGestureType (typeInt), withSpacingAndCase);
    }

    const String getDescriptionString (const neova_dash::gesture::GestureType type)
    {
        switch (type)
        {
            case neova_dash::gesture::vibrato:
                return "Quickly move your finger back and forth "
                       "to change the pitch in a sine shape.\n";
            
            case neova_dash::gesture::pitchBend:
                return "Lean your hand to either side "
                       "to change the pitch of your note\n";
            
            case neova_dash::gesture::tilt:
                return "Tilt your hand upwards or downwards to modulate your sound.\n";
            
            case neova_dash::gesture::roll:
                return "Rotate your hand sideways to modulate your sound.\n";
            
            case neova_dash::gesture::wave:
                return "TBD.\n";

            default:
                return "-";
        }
    }
}; // namespace gesture
} // namespace neova_dash