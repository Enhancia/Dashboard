/*
  ==============================================================================

    DashPath.h
    Created: 16 Sep 2019 3:35:25pm
    Author:  Enhancia Dev

  ==============================================================================
*/

#pragma once

#include "../../JuceLibraryCode/JuceHeader.h"

namespace neova_dash
{
    namespace path
    {
    		enum PathId
    		{
    			options = 0,
    			onOff,
                enhanciaLogo,
                upload,
                tilt,
                vibrato,
                pitchBend,
                roll,
                ring,
                ringFull,
                hub,
                dashIcon
    		};

        extern const Path createPath (PathId pathToCreate);
    }
}