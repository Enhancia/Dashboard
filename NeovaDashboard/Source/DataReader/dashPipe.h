/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#ifndef __DASH_PIPE__
#define __DASH_PIPE__

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "../Common/DashCommon.h"

#if JUCE_MAC
#include <SystemConfiguration/SystemConfiguration.h>
#endif //JUCE_MAC
//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/

/**
 *  \class DataReader DataReader.h
 *
 *  \brief Class that gets the raw data from the ring.
 *
 *  This is a class that is instanciated by the PlumeProcessor object. It inherits from InterprocessConnection
 *  so that it can create a pipe, to which a RawDataReader app can connect to send data.
 */
class DashPipe : public Component,
                     private InterprocessConnection,
                     public ChangeBroadcaster
{
public:
    static constexpr int DATA_SIZE = 7;
	static constexpr int DATABUFFERSIZE = 1024;
    
    //==============================================================================
	DashPipe();
    ~DashPipe();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

    //==============================================================================
    bool readData(String s);
    const String getRawData(int index);
	void getDataBuffer(uint8_t * buffer, int bytesToRead);
    bool getRawDataAsFloatArray(Array<float>& arrayToFill);

	//==============================================================================
	void sendString(uint8_t * data, int data_size);
    
    //==============================================================================
    bool connectToExistingPipe();
    bool isConnected();
    
    //==============================================================================
    void connectionMade() override;
    void connectionLost() override;
    void messageReceived(const MemoryBlock &message) override;
    
private:
    //==============================================================================
    bool connected;
    int pipeNumber = -1;
	//std::unique_ptr<uint8_t[]> dataBuffer;
	
	uint8_t dataBuffer[1024];
    //ScopedPointer<StringArray> data;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DashPipe)
};

#endif // __RAW_DATA_READER__
