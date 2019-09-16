/*
  ==============================================================================

    DashShapeButton.cpp
    Created: 16 Sep 2019 3:47:54pm
    Author:  Enhancia Dev

  ==============================================================================
*/

#include "DashShapeButton.h"

DashShapeButton::DashShapeButton (const String &name, Colour backgroundColour,
					  				Colour normalOffColour, Colour overOffColour, Colour downOffColour,
					  				Colour normalOnColour, Colour overOnColour, Colour downOnColour)
	: ShapeButton (name, Colour (0x00000000),
						 Colour (0x00000000),
						 Colour (0x00000000)),
	  backgroundFill (backgroundColour),
	  pathStrokeOffNormal (normalOffColour), pathStrokeOffOver (overOffColour), pathStrokeOffDown (downOffColour),
	  pathStrokeOnNormal (normalOnColour),   pathStrokeOnOver (overOnColour),   pathStrokeOnDown (downOnColour)
{
	setOnColours (Colour (0), Colour (0), Colour (0));
	shouldUseOnColours (true);
}

DashShapeButton::DashShapeButton (const String &name, Colour backgroundColour,
					  				Colour normalColour, Colour overColour, Colour downColour)
	: DashShapeButton (name, backgroundColour,
						normalColour, overColour, downColour,
						normalColour, overColour, downColour)
{
}

DashShapeButton::DashShapeButton (const String &name, Colour backgroundColour, Colour pathColour)
	: DashShapeButton (name, backgroundColour, pathColour,
							  				    pathColour.withAlpha (0.5f),
							  					pathColour.withAlpha (0.3f))
{
}

DashShapeButton::DashShapeButton (const String &name, Colour backgroundColour,
									Colour pathColourOff, Colour pathColourOn)
	: DashShapeButton (name, backgroundColour, pathColourOff,
							  				    pathColourOff.withAlpha (0.5f),
							  					pathColourOff.withAlpha (0.3f),
							  					pathColourOn,
							  				    pathColourOn.withAlpha (0.5f),
							  					pathColourOn.withAlpha (0.3f))
{
}

DashShapeButton::~DashShapeButton()
{
}

void DashShapeButton::resized()
{
	ShapeButton::resized();
	if (useDashBorders) setBorderSize (BorderSize<int> (jmin (getHeight(), getWidth())/4));
}

void DashShapeButton::paintButton (Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
{
	// Draws background
	Path buttonShape;
	buttonShape.addEllipse (getLocalBounds().withSizeKeepingCentre (jmin (getHeight(), getWidth()),
																	jmin (getHeight(), getWidth()))
											.toFloat());
	g.setColour (backgroundFill);
	g.fillPath (buttonShape);

	// Sets Outline Colour for ShapeButton
	if (paintMode != fill)
	{
		if (shouldDrawButtonAsHighlighted) setOutline (getToggleState() ? pathStrokeOnOver
																		: pathStrokeOffOver,
													   1.0f);

		else if (shouldDrawButtonAsDown)   setOutline (getToggleState() ? pathStrokeOnDown
																		: pathStrokeOffDown,
													   1.0f);

		else                               setOutline (getToggleState() ? pathStrokeOnNormal
																		: pathStrokeOffNormal,
													   1.0f);
	}

	// Draws Regular Shape Button on top
	g.reduceClipRegion (buttonShape);
	ShapeButton::paintButton (g, shouldDrawButtonAsHighlighted, shouldDrawButtonAsDown);
}

void DashShapeButton::setBorderSize (BorderSize<int> newBorders)
{
	useDashBorders = false;

	ShapeButton::setBorderSize (newBorders);
	resized();
}

void DashShapeButton::resetBorderSize()
{
	useDashBorders = true;
	resized();
}

void DashShapeButton::setBackgroundColour (const Colour newColour)
{
	backgroundFill = newColour;
	repaint();
}

void DashShapeButton::setStrokeColours (const Colour newStrokeNormalColour,
					   					 const Colour newStrokeOverColour,
					   					 const Colour newStrokeDownColour)
{
	setStrokeOffAndOnColours (newStrokeNormalColour, newStrokeOverColour, newStrokeDownColour,
					          newStrokeNormalColour, newStrokeOverColour, newStrokeDownColour);
}

void DashShapeButton::setStrokeOnColours (const Colour newStrokeOnNormalColour,
					     				   const Colour newStrokeOnOverColour,
					     				   const Colour newStrokeOnDownColour)
{
	pathStrokeOnNormal = newStrokeOnNormalColour;
	pathStrokeOnOver   = newStrokeOnOverColour;
	pathStrokeOnDown   = newStrokeOnDownColour;

	repaint();
}

void DashShapeButton::setStrokeOffAndOnColours (const Colour newStrokeOffNormalColour,
					   					 		 const Colour newStrokeOffOverColour,
					   					 		 const Colour newStrokeOffDownColour,
					   					 		 const Colour newStrokeOnNormalColour,
					   					 		 const Colour newStrokeOnOverColour,
					   					 		 const Colour newStrokeOnDownColour)
{
	pathStrokeOffNormal = newStrokeOffNormalColour;
	pathStrokeOffOver   = newStrokeOffOverColour;
	pathStrokeOffDown   = newStrokeOffDownColour;

	pathStrokeOnNormal = newStrokeOnNormalColour;
	pathStrokeOnOver   = newStrokeOnOverColour;
	pathStrokeOnDown   = newStrokeOnDownColour;

	repaint();
}

void DashShapeButton::setStrokeOffAndOnColours (const Colour newStrokeOffColour,
					   					 		 const Colour newStrokeOnColour)
{
	setStrokeOffAndOnColours (newStrokeOffColour, newStrokeOffColour.withAlpha (0.5f), 
												  newStrokeOffColour.withAlpha (0.3f),
					          newStrokeOnColour,  newStrokeOnColour.withAlpha  (0.5f),  
					          					  newStrokeOnColour.withAlpha  (0.3f));
}


void DashShapeButton::setPaintMode (const PaintMode newPaintMode)
{
	if (paintMode == newPaintMode) return;

	switch (newPaintMode)
	{
		case stroke: 
			paintMode = stroke;

			setStrokeOffAndOnColours (pathStrokeOffNormal, pathStrokeOffOver, pathStrokeOffDown,
					   				  pathStrokeOnNormal,  pathStrokeOnOver, pathStrokeOnDown);

			setColours (Colour (0), Colour (0), Colour (0));
			setOnColours (Colour (0), Colour (0), Colour (0));

			break;

		case fill: 
			paintMode = fill;

			setOutline (Colour (0), 0.0f);
			setColours (pathStrokeOffNormal, pathStrokeOffOver, pathStrokeOffDown);
			setOnColours (pathStrokeOnNormal, pathStrokeOnOver, pathStrokeOnDown);
			break;

		case fillAndStroke:
			paintMode = fillAndStroke;

			setStrokeOffAndOnColours (pathStrokeOffNormal, pathStrokeOffOver, pathStrokeOffDown,
					   				  pathStrokeOnNormal, pathStrokeOnOver, pathStrokeOnDown);

			setColours (pathStrokeOffNormal, pathStrokeOffOver, pathStrokeOffDown);
			setOnColours (pathStrokeOnNormal, pathStrokeOnOver, pathStrokeOnDown);
			break;

		default:
			break;
	}
}