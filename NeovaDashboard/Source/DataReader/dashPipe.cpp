/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#include "dashPipe.h"

//==============================================================================
DashPipe::DashPipe(): InterprocessConnection (true, 0x6a6d626e)
{
    setSize (120, 50);
    connected = false;
    
    // Data initialization
    data = new StringArray (StringArray::fromTokens ("0 0 0 0 0 0 0", " ", String()));
    
    #if JUCE_MAC
        statutPipe = std::make_unique<StatutPipe> ();
        statutPipe->addChangeListener(this);
    #else
        // Pipe creation
        connectToExistingPipe();
    #endif
    
    // Label creation
    addAndMakeVisible (connectedLabel = new Label ("connectedLabel", TRANS ("Disconnected")));
    connectedLabel->setColour (Label::textColourId, Colour (0xaaff0000));
    connectedLabel->setColour (Label::backgroundColourId, Colour (0x00000000));
    connectedLabel->setBounds (10, 5, 100, 40);
}

DashPipe::~DashPipe()
{
    data = nullptr;
    connectedLabel = nullptr;
  #if JUCE_MAC
    statutPipe = nullptr;
  #endif
}

//==============================================================================
void DashPipe::paint (Graphics& g)
{
    g.fillAll (Colour (0x42000000));
}

void DashPipe::resized()
{
}

//==============================================================================
bool DashPipe::readData (String s)
{
	auto strArr = StringArray::fromTokens(s, " ", String());

    // Checks for full lines
    if (strArr.size() == DATA_SIZE)
    {
        // Splits the string into 7 separate ones
        *data = strArr;
        return true;
    }
    
	return false;
}

const String DashPipe::getRawData (int index)
{
    return (*data)[index];
}

bool DashPipe::getRawDataAsFloatArray(Array<float>& arrayToFill)
{
    // Checks that the array has the right amont and type of data
    if (arrayToFill.isEmpty() == false) return false;
    
    //DBG ("Data: " << data->joinIntoString(", "));
    
    // Fills the array with new values taken from the "Data" StringArray
    for (int i =0; i<DATA_SIZE; i++)
    {
        arrayToFill.add ((*data)[i].getFloatValue());
        //DBG ("Value " << i << " = " << arrayToFill[i]);
    }
    
    return true;
}
//==============================================================================
void  DashPipe::sendString(uint8_t * data, int data_size)
{
	sendMessage(MemoryBlock(data, data_size));
}

//==============================================================================
bool DashPipe::connectToExistingPipe()
{
	return connectToPipe ("dashpipe", -1);
}

bool DashPipe::connectToExistingPipe(int nbPipe)
{
    //only happens on MacOS
  #if JUCE_MAC
    //get current userID
    uid_t currentUID;
    SCDynamicStoreCopyConsoleUser(NULL, &currentUID, NULL);

    //create namedpipe  with currentUID to enable multi user session
    return connectToPipe("mynamedpipe" + String (currentUID) + String(nbPipe), -1);
  #elif JUCE_WINDOWS
	return false;
  #endif
}

bool DashPipe::isConnected()
{
    return connected;
}

//==============================================================================
void DashPipe::connectionMade()
{
    connected = true;
    
    #if JUCE_MAC
        String test = "Start";
        sendMessage(MemoryBlock(test.toUTF8(), test.getNumBytesAsUTF8()));
    #endif
    
    connectedLabel->setColour (Label::textColourId, Colour (0xaa00ff00));
    connectedLabel->setText (TRANS ("<Connected>" /* : pipe " + String(pipeNumber)*/), dontSendNotification);
}

void DashPipe::connectionLost()
{
    connected = false;
    
    #if JUCE_MAC
        String test = "Stop";
        sendMessage(MemoryBlock(test.toUTF8(), test.getNumBytesAsUTF8()));
    #endif
    
    connectedLabel->setColour (Label::textColourId, Colour (0xaaff0000));
    connectedLabel->setText (TRANS ("Disconnected"), dontSendNotification);
}

void DashPipe::messageReceived (const MemoryBlock &message)
{
	sendChangeMessage();	
	/* For testing */
	/*
	uint8_t data[1024];
	memcpy(data, "jeannine", sizeof("jeannine"));
	uint32_t ctrl = 0x03;
	memcpy(data + 8, &ctrl, sizeof(uint32_t));

	sendString(data, 12);
	*/
}

void DashPipe::changeListenerCallback (ChangeBroadcaster * source)
{
    //only happens on MacOS
  #if JUCE_MAC
    int nbPipeToConnect = statutPipe->getPipeToConnect();
    connectToExistingPipe(nbPipeToConnect);
    statutPipe->disconnect();
    statutPipe.reset();
  #endif
}