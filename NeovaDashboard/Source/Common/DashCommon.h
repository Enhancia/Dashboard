/*
  ==============================================================================

    DashCommon.h
    Created: 3 Sep 2019 10:02:36am
    Author:  Enhancia Dev

  ==============================================================================
*/

#pragma once

#include "../../JuceLibraryCode/JuceHeader.h"
#include "DashPath.h"
#include "DashGestures.h"

// Preprocessor expressions
#define TRACE_IN  Logger::writeToLog ("[+] Entering: " + String(__FUNCTION__) + "\n")
#define TRACE_OUT Logger::writeToLog ("[-]  Leaving: " + String(__FUNCTION__) + "\n")

namespace neova_dash
{
    namespace commands
    {
    	// Lists all possible commands with their associated id
    	enum commandIDs
    	{
            // Backend commands
            flashHub              = 0x00000001, // Writes Temp Hub Config To Its Memory
            upgradeHub            = 0x00000002, // Upgrades Hub Firm
            upgradeRing           = 0x00000003, // Upgrades Ring Firm
            uploadConfigToHub     = 0x00000004, // Uploads config to HUB
            updatePresetModeState = 0x00000005, // Updates the backend preset mode to fit the interface

            // Frontend commands
            updateDashInterface  = 0x01000001, // Updates Dash interface to match the HUB data
            updateInterfaceLEDs  = 0x01000002, // Updates the Hub lEDs in the Dash interface
            updateBatteryDisplay = 0x01000003, // Updates the battery display in the header
            allowUserToFlashHub  = 0x01000004, // Updates upload button after config was changed
            openFirmUpgradePanel = 0x01000005  // Launches the firm upgrade panel
    	};
    };

    namespace ui
    {
        // UI display constant sizes, in px
        const int MARGIN           = 8;
        const int MARGIN_SMALL     = 3;
        const int HEADER_HEIGHT    = 55;

        // Gesture tuner display ranges
        const float VIBRATO_DISPLAY_MAX  = 100.0f;
        const float VIBRATO_THRESH_DISPLAY_MAX = 100.0f;
    
        const float PITCHBEND_DISPLAY_MIN = -90.0f;
        const float PITCHBEND_DISPLAY_MAX =  90.0f;
    
        const float TILT_DISPLAY_MIN = -90.0f;
        const float TILT_DISPLAY_MAX =  90.0f;
    
        const float ROLL_DISPLAY_MIN = -90.0f;
        const float ROLL_DISPLAY_MAX =  90.0f;
    
        const float WAVE_DISPLAY_MIN = -90.0f;
        const float WAVE_DISPLAY_MAX =  90.0f;

        const int FRAMERATE = 60; // Display frequency in Hz

        extern void paintTiledPath (Graphics&, Path&, juce::Rectangle<float>,
                                               const float, const float,
                                               const Colour, const Colour,
                                               const float);
    };

    namespace colour
    {
        const Colour dashboardBackground    (0xff2f2f2f);
        const Colour dashboardBackground2   (0xff2b2b2b);
        const Colour topPanelBackground     (0xff2b2b2b);
        const Colour topPanelTransparentArea(0xc0000005);
        const Colour headerBackground       (0xff464646);
        const Colour uploadButtonBackground (0xff565656);
        const Colour emptySlotOutline       (0xff808080);
        const Colour emptySlotBackground    (0x10808080);
        const Colour gestureHeader          (0xff636363);
        const Colour gestureBackground      (0xff353535);
        const Colour gestureBackground2     (0xff2c2c2c);
        const Colour mainText               (0xfff0f0f0);
        const Colour subText                (0x80f0f0f0);

        const Colour tunerSliderThumb       (0xffffffff);
        const Colour tunerSliderBackground  (0xff4e4e4e);
        const Colour midiSliderBackground   (0xff434343);

        const Colour inactiveGesture        (0xff808080);
        const Colour vibrato                (0xffec6565);
        const Colour pitchBend              (0xff1fcaa8);
        const Colour tilt                   (0xfff3a536);
        const Colour roll                   (0xffa255f3);
        const Colour wave                   (0xff7c80de);
    };

    namespace data
    {
        enum HubData
        {
            variance =0,
            acceleration,
            tilt,
            roll,
            battery,

            numDatas
        };

        static float convertRawBatteryToPercentage (float rawBatteryValue, bool isCharging)
        {
            if (rawBatteryValue < 3.46f && !isCharging) return 0.0f;
            else if (rawBatteryValue > 4.12 && isCharging) return 1.0f;

            Array<float> batteryTiers ( {3.52f, 3.58f, 3.61f, 3.64f, 3.69f,
                                          3.76f, 3.84f, 3.92f, 4.01f, 4.12f});

            float battery = isCharging ? 0.0f : 0.1f;

            for (float batteryTier : batteryTiers)
            {
                if (rawBatteryValue > batteryTier)
                {
                    battery += 0.1f;
                }

                else return battery;
            }

            return 1.0f;
        }
    }

    namespace compatibility
    {
        const int COMPATIBLE_FIRM = 202;
    }

    namespace font
    {
        enum DashFontId
        {
            regular =0,
            bold,
            light,
            ttNorms,
            enhanciaLogo,
            neovaLogo
        };
        
        extern const Font dashFont;
        extern const Font dashFontBold;
        extern const Font dashFontLight;
        extern const Font dashFontNorms;
        extern const Font enhanciaFont;
        extern const Font neovaFont;
    };

    namespace gesture
    {
        extern const Colour getHighlightColour (const GestureType type, const bool active = true);
        extern const Colour getHighlightColour (const int typeInt, const bool active = true);
    }

    namespace midi
    {
        enum CCMessage
        {
            modWheel = 1,
            volume = 7,
            balance = 8,
            pan = 9,
            expression = 11
        };

        //extern String getCCString();

        const uint8_t undefinedCCs[] = { /*3, 9, 14, 15, 20, 21, 22, 23, 24, 26, 27, 28, 29, 30, 31,
                                         85, 86, 87, 89, 90,*/102, 103, 104, 105, 106, 107, 108, 109,
                                         110, 111, 112, 113, 114, 115, 116, 117, 118, 119 };
    }

    namespace auth
    {
        const String MACHINE_TOKEN ("50327c582d22471d2427faed42c9928dcd8b0e98 "); //std::getenv ("MACHINE_ENHANCIA_OAUTH");
    }
};