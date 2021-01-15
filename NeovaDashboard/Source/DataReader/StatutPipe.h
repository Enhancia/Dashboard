/*
 ==============================================================================
 
 This file was auto-generated!
 
 ==============================================================================
 */

#ifndef __STATUTPIPE__
#define __STATUTPIPE__

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "../Common/DashCommon.h"

#if JUCE_MAC

//==============================================================================

class StatutPipe  : public Component, public InterprocessConnection, public ChangeBroadcaster
{
public:

    //==============================================================================
    StatutPipe();
    ~StatutPipe();
    
    //==============================================================================
    bool connectNewPipe();
    bool isConnected();
    int getPipeToConnect();
    
    //==============================================================================
    void connectionMade() override;
    void connectionLost() override;
    void messageReceived(const MemoryBlock &message) override;
    
private:
    bool connected;
    int nbPipeToConnect;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (StatutPipe)
};

#endif // JUCE_MAC
#endif // __STATUTPIPE__
