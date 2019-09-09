/*
  ==============================================================================

    DashCommon.cpp
    Created: 3 Sep 2019 10:02:36am
    Author:  Enhancia Dev

  ==============================================================================
*/

#include "DashCommon.h"

namespace neova_dash
{
namespace gesture
{
	neova_dash::gesture::GestureType intToGestureType (int typeInt)
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

	const String getTypeString (neova_dash::gesture::GestureType type, bool withSpacingAndCase)
	{
		switch (type)
		{
			case neova_dash::gesture::vibrato:
				return withSpacingAndCase ? "vibrato" : "Vibrato" ;
			
			case neova_dash::gesture::pitchBend:
				return withSpacingAndCase ? "pitchBend" : "Pitch Bend" ;
			
			case neova_dash::gesture::tilt:
				return withSpacingAndCase ? "tilt" : "Tilt" ;
			
			case neova_dash::gesture::roll:
				return withSpacingAndCase ? "roll" : "Roll" ;
			
			case neova_dash::gesture::wave:
				return withSpacingAndCase ? "wave" : "Wave" ;

			default:
				return "none";
		}
	}

	const String getDescriptionString (neova_dash::gesture::GestureType type)
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

	const Colour getHighlightColour (neova_dash::gesture::GestureType type)
	{
		switch (type)
		{
			case neova_dash::gesture::vibrato:
				return neova_dash::colour::vibrato;
			
			case neova_dash::gesture::pitchBend:
				return neova_dash::colour::pitchBend;
			
			case neova_dash::gesture::tilt:
				return neova_dash::colour::tilt;
			
			case neova_dash::gesture::roll:
				return neova_dash::colour::roll;
			
			case neova_dash::gesture::wave:
				return neova_dash::colour::wave;

			default:
				return neova_dash::colour::inactiveGesture;
		}	
	}

    const Colour getHighlightColour (int typeInt)
    {
    	return getHighlightColour (intToGestureType (typeInt));
    }

}; // namespace gesture

namespace font
{
	//const Font dashFont = Font();
}; // namespace font

}; // namespace neova_dash