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

namespace font
{
	const Font getDashFont (DashFontId type = regular)
    {
        // Halis Grotesque
        if (type == regular)
            return Font (Typeface::createSystemTypefaceFor (DashData::Ahmet_Altun__HalisGRRegular_otf,
                                                            DashData::Ahmet_Altun__HalisGRRegular_otfSize));
        if (type == bold)
            return Font (Typeface::createSystemTypefaceFor (DashData::Ahmet_Altun__HalisGRBold_otf,
                                                            DashData::Ahmet_Altun__HalisGRBold_otfSize));
        if (type == light)
            return Font (Typeface::createSystemTypefaceFor (DashData::Ahmet_Altun__HalisGRLight_otf,
                                                            DashData::Ahmet_Altun__HalisGRLight_otfSize));
        if (type == ttNorms)
            return Font (Typeface::createSystemTypefaceFor (DashData::TTNormsRegular_otf,
                                                            DashData::TTNormsRegular_otfSize));
            // Enhancia logo (capital only)
        if (type == enhanciaLogo)
            return Font (Typeface::createSystemTypefaceFor (DashData::NOOADemiSerifDEMO_ttf,
                                                            DashData::NOOADemiSerifDEMO_ttfSize));
            // Neova logo (capital only)
        if (type == neovaLogo)
            return Font (Typeface::createSystemTypefaceFor (DashData::Neovafont_otf,
                                                            DashData::Neovafont_otfSize));

        return Font();
    }

    const Font dashFont      = getDashFont();
    const Font dashFontBold  = getDashFont (bold);
    const Font dashFontLight = getDashFont (light);
    const Font dashFontNorms = getDashFont (ttNorms);
    const Font enhanciaFont  = getDashFont (enhanciaLogo);
    const Font neovaFont     = getDashFont (neovaLogo);

}; // namespace font

namespace gesture
{
	const Colour getHighlightColour (const GestureType type, const bool active)
	{
		//if (!active) return neova_dash::colour::inactiveGesture;
		Colour highlightColour;

		switch (type)
		{
			case vibrato:
				highlightColour = colour::vibrato;
        break;
			
			case pitchBend:
				highlightColour = colour::pitchBend;
        break;
			
			case tilt:
				highlightColour = colour::tilt;
        break;
			
			case roll:
				highlightColour = colour::roll;
        break;
			
			case wave:
				highlightColour = colour::wave;
        break;

			default:
				highlightColour = colour::inactiveGesture;
		}

		if (!active) highlightColour = highlightColour.withAlpha (0.4f);

		return highlightColour;
	}

    const Colour getHighlightColour (const int typeInt, const bool active)
    {
    	return getHighlightColour (intToGestureType (typeInt), active);
    }
} // namespace gesture
}; // namespace neova_dash