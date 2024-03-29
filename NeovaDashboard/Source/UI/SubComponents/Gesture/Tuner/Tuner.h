/*
  ==============================================================================

    Tuner.h
    Created: 12 Sep 2019 10:11:42am
    Author:  Enhancia Dev

  ==============================================================================
*/

#pragma once

#include "../../../../../JuceLibraryCode/JuceHeader.h"
#include "../../../../Common/DashCommon.h"
#include "../../../../Common/HubConfiguration.h"
#include "../../../LookAndFeel/DashBoardLookAndFeel.h"

//==============================================================================
/*
*/
class Tuner    : public Component
{
public:
    //==============================================================================
	Tuner (const String unit = String(), Colour colour = Colour (0xff7c80de))
        : valueUnit(unit), tunerColour (colour)
    {}

	~Tuner() {}

    //==============================================================================
	virtual void paint(Graphics&) override {}
	virtual void resized() override {}
    
    //==============================================================================
	virtual void updateDisplay() {}
	virtual void updateComponents() = 0;

    //==============================================================================
    Colour getColour() { return tunerColour; }
    virtual void setColour (const Colour newColour) { tunerColour = newColour; }
    virtual void updateColour() {}

protected:
    Colour tunerColour;
    const String valueUnit;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Tuner)
};