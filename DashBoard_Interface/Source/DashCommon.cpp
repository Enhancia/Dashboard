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


static const Colour getHighlightColour (neova_dash::gesture::GestureType type)
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

}; // namespace neova_dash
}; // namespace gesture