/*
  ==============================================================================

    GesturePanel.cpp
    Created: 3 Sep 2019 11:38:56am
    Author:  Enhancia Dev

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "GesturePanel.h"

//==============================================================================
GesturePanel::GesturePanel (HubConfiguration& data) : hubConfig (data)
{
}

GesturePanel::~GesturePanel()
{
}

void GesturePanel::paint (Graphics& g)
{
    g.setColour (Colours::white);
    g.drawRoundedRectangle (getLocalBounds().toFloat(), 3.0f, 1.0f);
}

void GesturePanel::resized()
{
}
