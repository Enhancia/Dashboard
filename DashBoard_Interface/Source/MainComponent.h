/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

ApplicationCommandManager& getCommandManager();

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent   : public Component,
                        public ApplicationCommandTarget
{
public:
    //==============================================================================
    MainComponent (Colour& colRef);
    ~MainComponent();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

    //==============================================================================
    void mouseDown (const MouseEvent&);

    //==============================================================================
    ApplicationCommandTarget* getNextCommandTarget() override;
    void getAllCommands (Array<CommandID> &commands) override;
    void getCommandInfo (CommandID commandID, ApplicationCommandInfo& result) override;
    bool perform (const InvocationInfo& info) override;

private:
    //==============================================================================
    const Colour& colourRef;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
