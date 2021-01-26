/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/
#include "../../JuceLibraryCode/JuceHeader.h"

#if (JUCE_WINDOWS || defined(__OBJC__))

#include "dashPipe.h"

//==============================================================================
DashPipe::DashPipe(): InterprocessConnection (true, 0x6a6d626e)
{
    setSize (120, 50);
    connected = false;
    
    // Data initialization
    //data = new StringArray (StringArray::fromTokens ("0 0 0 0 0 0 0", " ", String()));
	//dataBuffer = std::make_unique<uint8_t[]>(DATABUFFERSIZE);

    connectToExistingPipe();
}

DashPipe::~DashPipe()
{
	TRACE_IN;
    disconnect();
}

//==============================================================================
void DashPipe::paint (Graphics& g)
{
	TRACE_IN;
	g.fillAll (Colour (0x42000000));
}

void DashPipe::resized()
{
	TRACE_IN;
}

//==============================================================================
bool DashPipe::readData (String s)
{
	/*auto strArr = StringArray::fromTokens(s, " ", String());

    // Checks for full lines
    if (strArr.size() == DATA_SIZE)
    {
        // Splits the string into 7 separate ones
        *data = strArr;
        return true;
    }
    */
	TRACE_IN;
	return false;
}

const String DashPipe::getRawData (int index)
{
	TRACE_IN;
	String test = "test";
	return test;
	//return (*data)[index];
}

void DashPipe::getDataBuffer(uint8_t * buffer, int bytesToRead)
{
	TRACE_IN;
	if (bytesToRead <= DATABUFFERSIZE)
	{
		memcpy(buffer, dataBuffer, bytesToRead);
	}
}

bool DashPipe::getRawDataAsFloatArray(Array<float>& arrayToFill)
{
	TRACE_IN;
	/*
	// Checks that the array has the right amont and type of data
    if (arrayToFill.isEmpty() == false) return false;
    
    //DBG ("Data: " << data->joinIntoString(", "));
    
    // Fills the array with new values taken from the "Data" StringArray
    for (int i =0; i<DATA_SIZE; i++)
    {
        arrayToFill.add ((*data)[i].getFloatValue());
        //DBG ("Value " << i << " = " << arrayToFill[i]);
    }
    */
    return true;
}
//==============================================================================
void  DashPipe::sendString(uint8_t * data, int data_size)
{
	TRACE_IN;
	bool test = sendMessage(MemoryBlock(data, data_size));
    DBG("Send string return :" + String(int(test)));
}

//==============================================================================
bool DashPipe::connectToExistingPipe()
{
	TRACE_IN;
	
    
    #if JUCE_MAC
        //get current userID
        uid_t currentUID;
        SCDynamicStoreCopyConsoleUser(NULL, &currentUID, NULL);

        //create namedpipe  with currentUID to enable multi user session
        return connectToPipe("dashpipe" + String (currentUID), -1);
    #elif JUCE_WINDOWS
        return connectToPipe ("dashpipe", -1);
    #endif
}


bool DashPipe::isConnected()
{
	TRACE_IN;
	return connected;
}

//==============================================================================
void DashPipe::connectionMade()
{
	TRACE_IN;
	connected = true;
    
    #if JUCE_MAC
        String test = "Start";
        sendMessage(MemoryBlock(test.toUTF8(), test.getNumBytesAsUTF8()));
    #endif
    
}

void DashPipe::connectionLost()
{
	TRACE_IN;
	connected = false;
    
    #if JUCE_MAC
        String test = "Stop";
        sendMessage(MemoryBlock(test.toUTF8(), test.getNumBytesAsUTF8()));
    #endif
    
}

void DashPipe::messageReceived (const MemoryBlock &message)
{
	TRACE_IN;
	uint64_t jeannine = *(uint64_t*)message.getData();
	if (jeannine == 0x656E696E6E61656A && message.getSize() < DATABUFFERSIZE)
	{
		memcpy(dataBuffer, message.getData(), message.getSize());
		sendChangeMessage();
	}
	else
	{
		Logger::writeToLog("Hub message : Error");
	}
}

#endif // JUCE WIN || OBJC
