/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/
#include "../../JuceLibraryCode/JuceHeader.h"

#if (JUCE_WINDOWS || defined(__OBJC__))

#include "DataReader.h"

//==============================================================================
DataReader::DataReader (ApplicationCommandManager& manager, HubConfiguration& config)
    : InterprocessConnection (true, 0x6a6d626e), commandManager (manager), hubConfig (config)
{
    setSize (120, 50);
    connected = false;
    
    // Data initialization
    data = new StringArray (StringArray::fromTokens ("0 0 0 0 0 0", " ", String()));

    for (int i=0; i<neova_dash::data::numDatas; i++)
    {
        floatData.add (0.0f);
    }

    jassert (floatData.size() == 5);
    
    #if JUCE_MAC
        statutPipe = std::make_unique<StatutPipe> ();
        statutPipe->addChangeListener(this);
    #else
        // Pipe creation
        connectToExistingPipe();
    #endif
}

DataReader::~DataReader()
{
	data = nullptr;
    floatData.clear();

  #if JUCE_MAC
    statutPipe = nullptr;
  #endif
    
    disconnect();
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
void DataReader::timerCallback()
{
    if (hubConfig.getRingIsConnected())
    {
        hubConfig.setRingIsConnected (false);
        commandManager.invokeDirectly (neova_dash::commands::updateDashInterface, true);
    }

    stopTimer();
}

//==============================================================================
bool DataReader::readData (String s)
{
	auto strArr = StringArray::fromTokens(s, " ", String());

    // Checks for full lines
    if (strArr.size() == DATA_SIZE)
    {
        startTimer (3000);

        // Ring was disconnected
        if (!hubConfig.getRingIsConnected())
        {
            hubConfig.setRingIsConnected (true);
            commandManager.invokeDirectly (neova_dash::commands::setStateAndUpdateDashInterface, true);
        }
        // Ring was in chargeMode
        else if (hubConfig.getRingIsCharging())
        {
            hubConfig.setRingIsCharging (false);
            commandManager.invokeDirectly (neova_dash::commands::updateBatteryDisplay, true);
        }

        // Splits the string into 7 separate ones
        *data = strArr;

        for (int i=0; i<neova_dash::data::numDatas; i++)
        {
            floatData.set (i, (*data)[i].getFloatValue());
        }

        floatData.set (neova_dash::data::tilt, -floatData[neova_dash::data::tilt]);
        //DBG ("Data : " << data->joinIntoString (" "));

        return true;
    }

    // If ring charges, battery data only
    if (strArr.size() == 1)
    {
        startTimer (3000);

        // Ring was disconnected
        if (!hubConfig.getRingIsConnected())
        {
            hubConfig.setRingIsConnected (true);
            hubConfig.setRingIsCharging (true);
            commandManager.invokeDirectly (neova_dash::commands::setStateAndUpdateDashInterface, true);
            commandManager.invokeDirectly (neova_dash::commands::updateBatteryDisplay, true);
        }

        // Notifies Ring wasn't charging
        else if (!hubConfig.getRingIsCharging())
        {
            hubConfig.setRingIsCharging (true);
            commandManager.invokeDirectly (neova_dash::commands::updateBatteryDisplay, true);
        }

        (*data).set (neova_dash::data::battery, strArr[0]);
        floatData.set (neova_dash::data::battery, strArr[0].getFloatValue());

		//DBG ("Battery : " << (*data) [neova_dash::data::battery]);

        return true;
    }
    
	return false;
}

const String DataReader::getRawData (int index)
{
    return (*data)[index];
}

const float& DataReader::getFloatValueReference (const neova_dash::data::HubData dataId)
{
    if (dataId == neova_dash::data::numDatas) return floatData.getReference (int (neova_dash::data::battery));

    return floatData.getReference (int (dataId));
}

bool DataReader::getRawDataAsFloatArray(Array<float>& arrayToFill)
{
    // Checks that the array has the right amont and type of data
    if (arrayToFill.isEmpty() == false) return false;
    
    //DBG ("Data: " << data->joinIntoString(", "));
    
    // Fills the array with new values taken from the "Data" StringArray
    for (int i =0; i<DATA_SIZE; i++)
    {
        //arrayToFill.add ((*data)[i].getFloatValue());
        arrayToFill.addArray (floatData, floatData.size());
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
        sendMessage (MemoryBlock (test.toUTF8(), test.getNumBytesAsUTF8()));
    #endif
}

void DataReader::connectionLost()
{
    connected = false;
    
    #if JUCE_MAC
        String test = "Stop";
        sendMessage (MemoryBlock (test.toUTF8(), test.getNumBytesAsUTF8()));
    #endif

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

#endif //JUCE WIN || OBJC
