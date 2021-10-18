/*
  ==============================================================================

    DashCommon.h
    Created: 3 Sep 2019 10:02:36am
    Author:  Enhancia Dev

  ==============================================================================
*/

#pragma once
#pragma warning(push)
#pragma warning( disable: 4505 )    // unreferenced function has been removed

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
            flashHub                    = 0x00000001, // Writes Temp Hub Config To Its Memory
            upgradeHub                  = 0x00000002, // Upgrades Hub Firm
            upgradeRing                 = 0x00000003, // Upgrades Ring Firm
            uploadConfigToHub           = 0x00000004, // Uploads config to HUB
            updatePresetModeState       = 0x00000005, // Updates the backend preset mode to fit the interface
            checkDashboardUpdate        = 0x00000006, // checks database for new dashboard updates

            // Frontend commands
            updateDashInterface             = 0x01000001, // Updates Dash interface to match the HUB data
            setStateAndUpdateDashInterface  = 0x01000002, // Updates Dash interface and state to match HUB data
            updateInterfaceLEDs             = 0x01000003, // Updates the Hub lEDs in the Dash interface
            updateBatteryDisplay            = 0x01000004, // Updates the battery display in the header
            allowUserToFlashHub             = 0x01000005, // Updates upload button after config was changed
            openFirmUpgradePanel            = 0x01000006, // Launches the firm upgrade panel
            openDashboardUpdatePanel        = 0x01000007, // Launches the soft update panel
            checkAndUpdateNotifications     = 0x01000008, // Updates and looks for data that should trigger notifications on the interface
            openBugReportPanel              = 0x01000009  // Launches the bug report panel
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
    
        const float PITCHBEND_DISPLAY_MIN = -80.0f;
        const float PITCHBEND_DISPLAY_MAX =  80.0f;
    
        const float TILT_DISPLAY_MIN = -80.0f;
        const float TILT_DISPLAY_MAX =  80.0f;
    
        const float ROLL_DISPLAY_MIN = -80.0f;
        const float ROLL_DISPLAY_MAX =  80.0f;
    
        const float WAVE_DISPLAY_MIN = -80.0f;
        const float WAVE_DISPLAY_MAX =  80.0f;

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
        const Colour notificationBubble     (0xff8090f0);

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
        const Colour vibrato                (0xffff5ba1);
        const Colour pitchBend              (0xffb859f2);
        const Colour tilt                   (0xffffa21d);
        const Colour roll                   (0xff1ec0b7);
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

            const float cutThresh = 3.46f;
            Array<float> batteryTiers ( {3.52f, 3.58f, 3.61f, 3.64f,
                                         3.69f, 3.76f, 3.84f, 3.92f, 4.01f, 4.12f});

            // Standard behaviour
            float battery = 0.0f;

            for (float batteryTier : batteryTiers)
            {
                if (isCharging && batteryTier == batteryTiers[0]) continue; // Skips 1st check if ring is charging, 1st level is 2nd battery tier

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
        static bool isTestVersion()
        {
            if (JUCEApplication::getInstance() != nullptr)
            {
                return JUCEApplication::getInstance()->getApplicationVersion()
                                                     .upToFirstOccurrenceOf (".", false, true)
                                                     .getIntValue() >= 100;
            }

            return false;
        }

        const int COMPATIBLE_FIRM = 1;
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

        const uint8_t defaultCCs[] = { 1, 2, 3, 4 /*3, 9, 14, 15, 20, 21, 22, 23, 24, 26, 27, 28, 29, 30, 31,
                                         85, 86, 87, 89, 90, 102, 103, 104, 105, 106, 107, 108, 109,
                                         110, 111, 112, 113, 114, 115, 116, 117, 118, 119 */};
    }

    namespace log
    {
        enum LogLevel
        {
            trace =0,
            debug,
            info,
            warning,
            error,
            fatal
        };

        enum LogCategory
        {
            general =0,
            gesture,
            ui,
            options,
            update,
            hubCommunication,
            config
        };
        
        const String levelStrings[] = {
            "TRACE:  ",
            "DEBUG:  ",
            "INFO:   ",
            "WARNING:",
            "ERROR:  ",
            "FATAL:  "
        };

        const String categoryStrings[] = {
            "general         : ",
            "gesture         : ",
            "ui              : ",
            "options         : ",
            "update          : ",
            "hubCommunication: ",
            "configuration   : "
        };

        static void writeToLog (const String& message, const LogCategory category=general, const LogLevel level=info)
        {
          #if !JUCE_DEBUG
            if (int (level) >= int (info)) // cuts TRACE and DEBUG entries on production build
          #endif
            {
                String logString;
                const Time logTime (Time::getCurrentTime());

                logString += "[" + logTime.toISO8601(true) + "] ";

                if (auto* currentThread = Thread::getCurrentThread())
                {
                    logString += "[" + currentThread->getThreadName() + "] ";
                }
                
                logString += levelStrings[int(level)] + categoryStrings[int(category)]
                          +  message;

                Logger::writeToLog (logString);
            }
        }
    }

    namespace auth
    {
        const String MACHINE_TOKEN ("50327c582d22471d2427faed42c9928dcd8b0e98 "); //std::getenv ("MACHINE_ENHANCIA_OAUTH");
    }

    namespace keyboard_shortcut
    {
        const KeyPress selectGestureLeft     (KeyPress::leftKey);
        const KeyPress selectGestureRight    (KeyPress::rightKey);
        const KeyPress selectGestureUp       (KeyPress::upKey);
        const KeyPress selectGestureDown     (KeyPress::downKey);
        const KeyPress uploadToHub           ('s', ModifierKeys (ModifierKeys::commandModifier), 's');

        #if JUCE_WINDOWS
        const KeyPress deleteGesture         (KeyPress::deleteKey);
        #elif JUCE_MAC
        const KeyPress deleteGesture         (KeyPress::backspaceKey);
        #endif

        const KeyPress duplicateGesture      ('d', ModifierKeys (ModifierKeys::commandModifier), 'd');
        const KeyPress displayOptions        ('o', ModifierKeys (ModifierKeys::commandModifier), 'o');
        const KeyPress muteGesture           (KeyPress::spaceKey);
        const KeyPress selectNextBank        (KeyPress::rightKey, ModifierKeys (ModifierKeys::commandModifier), 0);
        const KeyPress selectPreviousBank    (KeyPress::leftKey, ModifierKeys (ModifierKeys::commandModifier), 0);
        const KeyPress selectBank1           ('1', ModifierKeys (ModifierKeys::commandModifier), '1');
        const KeyPress selectBank2           ('2', ModifierKeys (ModifierKeys::commandModifier), '2');
        const KeyPress selectBank3           ('3', ModifierKeys (ModifierKeys::commandModifier), '3');
        const KeyPress selectBank4           ('4', ModifierKeys (ModifierKeys::commandModifier), '4');
        const KeyPress muteGesture1          (KeyPress::numberPad1);
        const KeyPress muteGesture2          (KeyPress::numberPad2);
        const KeyPress muteGesture3          (KeyPress::numberPad3);
        const KeyPress muteGesture4          (KeyPress::numberPad4);
        const KeyPress easterEgg             ('n', ModifierKeys (ModifierKeys::commandModifier + ModifierKeys::shiftModifier), 'N');
    }
};