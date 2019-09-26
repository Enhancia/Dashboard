/*
  ==============================================================================

    DashCommon.h
    Created: 3 Sep 2019 10:02:36am
    Author:  Enhancia Dev

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "DashPath.h"

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
    		flashHub            = 0x00000001, // Writes Temp Hub Config To Its Memory
    		upgradeHub          = 0x00000002, // Upgrades Hub Firm
    		upgradeRing         = 0x00000003, // Upgrades Ring Firm
    		uploadConfigToHub   = 0x00000004, // Uploads config to HUB

            // Frontend commands
    		updateDashInterface = 0x01000001, // Updates Dash interface to match the HUB data
            updateInterfaceLEDs = 0x01000002  // Updates the Hub lEDs in the Dash interface
    	};
    };

    namespace ui
    {
        // UI display constant sizes, in px
        const int MARGIN           = 8;
        const int MARGIN_SMALL     = 3;
        const int HEADER_HEIGHT    = 55;

        // Gesture tuner display ranges
        const float VIBRATO_DISPLAY_MAX  = 500.0f;
        const float VIBRATO_THRESH_DISPLAY_MAX = 500.0f;
    
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

    namespace gesture
    {
        const int NUM_GEST = 4;

        enum GestureType
        {
            vibrato =0,
            pitchBend,
            tilt,
            roll,
            wave,

            numTypes,
            none
        };

        enum MidiType
        {
            pitchMidi =1,
            ccMidi,
            afterTouchMidi
        };

        // gesture max range values
        const float VIBRATO_RANGE_MAX  = 500.0f;
        const float VIBRATO_THRESH_MAX = 300.0f;
        
        const float PITCHBEND_MIN = -180.0f;
        const float PITCHBEND_MAX =  180.0f;
        
        const float TILT_MIN = -180.0f;
        const float TILT_MAX =  180.0f;
        
        const float ROLL_MIN = -180.0f;
        const float ROLL_MAX =  180.0f;
        
        const float WAVE_MIN = -180.0f;
        const float WAVE_MAX =  180.0f;

        // Helper methods for gestures
        extern GestureType intToGestureType (const int typeInt);
        extern const String getTypeString (const GestureType type, const bool withSpacingAndCase);
        extern const String getTypeString (const int typeInt, const bool withSpacingAndCase);
        extern const String getDescriptionString (const GestureType type);
        extern const Colour getHighlightColour (const GestureType type, const bool active = true);
        extern const Colour getHighlightColour (const int typeInt, const bool active = true);
    };

    namespace font
    {
        enum DashFontId
        {
            regular =0,
            bold,
            light,
            enhanciaLogo
        };
        
        extern const Font dashFont;
        extern const Font dashFontBold;
        extern const Font dashFontLight;
        extern const Font enhanciaFont;
    };
};