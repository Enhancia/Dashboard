/*
  ==============================================================================

    DashCommon.h
    Created: 3 Sep 2019 10:02:36am
    Author:  Enhancia Dev

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "HubConfiguration.h"

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

    namespace data
    {
    	/* enum avec les ids/noms des datas ring (vib_val, vib_intensity, ring_battery etc...)

    	enum ringDataIDs
    	{
    		numDatas =0
    	};
    	*/

    	/* Struct qui contient les datas (str firm hub / ring,
    	   int currentSelectedPreset, float[] avec les datas ring, etc)
		
		struct HubData
		{
			
		};

    	*/
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
        const Colour gestureHeader          (0xff636363);
        const Colour gestureBackground      (0xff353535);
        const Colour gestureBackground2     (0xff2c2c2c);
        const Colour mainText               (0xfff0f0f0);
        const Colour subText                (0x80f0f0f0);
    };
};