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

namespace ui
{
	void paintTiledPath (Graphics& g, Path& pathToTile, juce::Rectangle<float> bounds,
                                      const float columnWidth, const float rowHeight,
                                      const Colour fillColour, const Colour strokeColour,
                                      const float strokeThickness)
    {
        // Making sure the bounds are sensible
        jassert (bounds.getWidth() > 0.0f && bounds.getHeight() > 0.0f);
        jassert (columnWidth > 0.0f && rowHeight > 0.0f);

        if (bounds.getWidth() <= 0.0f || bounds.getHeight() <= 0.0f
               || columnWidth <= 0.0f || rowHeight <= 0.0f)
        {
            return;
        }
        
        Path pathRescaled = pathToTile;
        Path tiledPath;

        // sets the right initial position and size for the path
        pathRescaled.scaleToFit (pathToTile.getBounds().getWidth() > columnWidth
                                   ? bounds.getX() : bounds.getX() + columnWidth/2.0f
                                                                   - pathToTile.getBounds()
                                                                               .getWidth()/2,

                                 pathToTile.getBounds().getHeight() > rowHeight
                                   ? bounds.getY() : bounds.getY() + rowHeight/2.0f
                                                                   - pathToTile.getBounds()
                                                                               .getHeight()/2,

                                 pathToTile.getBounds().getWidth() > columnWidth
                                     ? columnWidth : pathToTile.getBounds().getWidth(),

                                 pathToTile.getBounds().getHeight() > rowHeight
                                     ? rowHeight : pathToTile.getBounds().getHeight(),
                               
                               true);

        for (int row = 0; row * rowHeight < bounds.getHeight(); row++)
        {
            for (int col = 0; col * columnWidth < bounds.getWidth(); col++)
            {
                tiledPath.addPath (pathRescaled, AffineTransform::translation (col * columnWidth,
                                                                               row * rowHeight));
            }   
        }

        g.saveState();
        g.reduceClipRegion (bounds.toNearestIntEdges());

        g.setColour (fillColour);
        g.fillPath (tiledPath);

        g.setColour (strokeColour);
        g.strokePath (tiledPath, PathStrokeType (strokeThickness));

        g.restoreState();
    }
}; // namespace ui

namespace gesture
{
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

	const String getTypeString (const neova_dash::gesture::GestureType type, const bool withSpacingAndCase)
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

	const Colour getHighlightColour (const neova_dash::gesture::GestureType type, const bool active)
	{
		if (!active) return neova_dash::colour::inactiveGesture;

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

    const Colour getHighlightColour (const int typeInt, const bool active)
    {
    	return getHighlightColour (intToGestureType (typeInt), active);
    }

}; // namespace gesture

namespace font
{
	//const Font dashFont = Font();
}; // namespace font

}; // namespace neova_dash