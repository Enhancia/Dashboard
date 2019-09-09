/*
  ==============================================================================

    DashCommon.h
    Created: 3 Sep 2019 10:02:36am
    Author:  Enhancia Dev

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

namespace neova_dash
{
    namespace commands
    {
    	// Lists all possible commands with their associated id
    	enum commandIDs
    	{
    		flashHub            = 0x00000001, // Writes Temp Hub Config To Its Memory
    		upgradeHub          = 0x00000002, // Upgrades Hub Firm
    		upgradeRing         = 0x00000003, // Upgrades Ring Firm
    		
    		uploadConfigToHub   = 0x00000004, // Uploads config to HUB
    		updateDashInterface = 0x00000005  // Updates Dash interface to match the HUB data 
    	};
    };

    namespace ui
    {
        const int MARGIN           = 8;
        const int MARGIN_SMALL     = 3;
        const int DASHBOARD_WIDTH  = 1266*3/4;
        const int DASHBOARD_HEIGHT = 1100*3/4;
        const int HEADER_HEIGHT    = 55;
    };

    namespace colour
    {
        const Colour dashboardBackground    (0xff2f2f2f);
        const Colour dashboardBackground2   (0xff2b2b2b);
        const Colour headerBackground       (0xff464646);
        const Colour uploadButtonBackground (0xff565656);
        const Colour emptySlotOutline       (0xff808080);
        const Colour emptySlotBackground    (0xff808080);
        const Colour gestureHeader          (0xff636363);
        const Colour gestureBackground      (0xff353535);
        const Colour gestureBackground2     (0xff2c2c2c);
        const Colour mainText               (0xfff0f0f0);
        const Colour subText                (0x80f0f0f0);

        const Colour inactiveGesture        (0xff808080);
        const Colour vibrato                (0xffec6565);
        const Colour pitchBend              (0xff1fcaa8);
        const Colour tilt                   (0xfff3a536);
        const Colour roll                   (0xffa255f3);
        const Colour wave                   (0xff7c80de);
    };

    namespace gesture
    {
        enum GestureType
        {
            none = 0,
            vibrato,
            pitchBend,
            tilt,
            roll,
            wave
        };

        static const String getTypeString (neova_dash::gesture::GestureType type, bool withSpacingAndCase);
        static const Colour getHighlightColour (neova_dash::gesture::GestureType type);
    };

    namespace font
    {
        const Font dashFont = Font();
    };
};