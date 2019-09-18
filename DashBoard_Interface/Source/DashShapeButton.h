/*
  ==============================================================================

    DashShapeButton.h
    Created: 16 Sep 2019 3:47:54pm
    Author:  Enhancia Dev

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

class DashShapeButton	: public ShapeButton
{
public:
    //==============================================================================
	DashShapeButton (const String &name, Colour backgroundColour,
					  Colour normalOffColour, Colour overOffColour, Colour downOffColour,
					  Colour normalOnColour, Colour overOnColour, Colour downOnColour);

	DashShapeButton (const String &name, Colour backgroundColour,
					  Colour normalColour, Colour overColour, Colour downColour);

	DashShapeButton (const String &name, Colour backgroundColour, Colour pathColour);

	DashShapeButton (const String &name, Colour backgroundColour,
		              Colour pathColourOff, Colour pathColourOn);

	~DashShapeButton();

    //==============================================================================
	void resized() override;
	void paintButton (Graphics&, bool, bool) override;
	void setBorderSize (BorderSize<int>);
	void resetBorderSize();

    //==============================================================================
	void setBackgroundColour (const Colour newColour);

    //==============================================================================
	void setStrokeColours (const Colour newStrokeNormalColour,
						   const Colour newStrokeOverColour,
						   const Colour newStrokeDownColour);

	void setStrokeOnColours (const Colour newStrokeOnNormalColour,
						     const Colour newStrokeOnOverColour,
						     const Colour newStrokeOnDownColour);

	void setStrokeOffAndOnColours (const Colour newStrokeOffNormalColour,
						   		   const Colour newStrokeOffOverColour,
						   		   const Colour newStrokeOffDownColour,
						   		   const Colour newStrokeOnNormalColour,
						   		   const Colour newStrokeOnOverColour,
						   		   const Colour newStrokeOnDownColour);

	void setStrokeOffAndOnColours (const Colour newStrokeOffColour,
						           const Colour newStrokeOnColour);

    //==============================================================================
    enum PaintMode
    {
    	stroke =0,
    	fill,
    	fillAndStroke
    };

    void setPaintMode (const PaintMode newPaintMode);
    void setStrokeThickness (const float newThickness);

private:
	Colour backgroundFill;
	float strokeThickness = 1.0f;
	
	PaintMode paintMode = stroke;
	Colour pathStrokeOffNormal, pathStrokeOffOver, pathStrokeOffDown;
	Colour pathStrokeOnNormal, pathStrokeOnOver, pathStrokeOnDown;
	bool useDashBorders = true;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DashShapeButton)
};