/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#include "DataReader.h"

//==============================================================================
DataReader::DataReader(): InterprocessConnection (true, 0x6a6d626e)
{
    setSize (120, 50);
    connected = false;
    
    // Data initialization
    data = new StringArray (StringArray::fromTokens ("0 0 0 0 0 0", " ", String()));
    
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

DataReader::~DataReader()
{
    data = nullptr;
    connectedLabel = nullptr;
  #if JUCE_MAC
    statutPipe = nullptr;
  #endif
}

//==============================================================================
void DataReader::paint (Graphics& g)
{
    g.fillAll (Colour (0x42000000));
}

void DataReader::resized()
{
}

//==============================================================================
bool DataReader::readData (String s)
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

const String DataReader::getRawData (int index)
{
    return (*data)[index];
}


bool DataReader::getRawDataAsFloatArray(Array<float>& arrayToFill)
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
bool DataReader::connectToExistingPipe()
{
	return connectToPipe ("mynamedpipe", -1);
}

bool DataReader::connectToExistingPipe(int nbPipe)
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

bool DataReader::isConnected()
{
    return connected;
}

//==============================================================================
void DataReader::connectionMade()
{
    connected = true;
    
    #if JUCE_MAC
        String test = "Start";
        sendMessage(MemoryBlock(test.toUTF8(), test.getNumBytesAsUTF8()));
    #endif
    
    connectedLabel->setColour (Label::textColourId, Colour (0xaa00ff00));
    connectedLabel->setText (TRANS ("<Connected>" /* : pipe " + String(pipeNumber)*/), dontSendNotification);
}

void DataReader::connectionLost()
{
    connected = false;
    
    #if JUCE_MAC
        String test = "Stop";
        sendMessage(MemoryBlock(test.toUTF8(), test.getNumBytesAsUTF8()));
    #endif
    
    connectedLabel->setColour (Label::textColourId, Colour (0xaaff0000));
    connectedLabel->setText (TRANS ("Disconnected"), dontSendNotification);
}

void DataReader::messageReceived (const MemoryBlock &message)
{
    if (connected && message.getSize() != 0)
    {
        if (readData (message.toString()))
        {
            sendChangeMessage();
        }
    }
}


void DataReader::changeListenerCallback (ChangeBroadcaster * source)
{
    //only happens on MacOS
  #if JUCE_MAC
    int nbPipeToConnect = statutPipe->getPipeToConnect();
    connectToExistingPipe(nbPipeToConnect);
    statutPipe->disconnect();
    statutPipe.reset();
  #endif
}