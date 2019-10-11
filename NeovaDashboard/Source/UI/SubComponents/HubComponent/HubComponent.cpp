/*
  ==============================================================================

    HubComponent.cpp
    Created: 3 Sep 2019 11:37:45am
    Author:  Enhancia Dev

  ==============================================================================
*/

#include "HubComponent.h"

//==============================================================================
// HubComponent

HubComponent::HubComponent (HubConfiguration& config, NewGesturePanel& newGest,
							ApplicationCommandManager& manager, int & dashState)
    : hubConfig (config), commandManager (manager), newGesturePanel (newGest), dashboardState (dashState)
{
	TRACE_IN;

	currentPreset = hubConfig.getSelectedPreset();

	// Creates Bottom Buttons And Lights
	for (int i =0; i < neova_dash::gesture::NUM_GEST; i++)
	{
		buttons.add (new HubButton (i));
		buttons.getLast()->addListener (this);
		addAndMakeVisible (buttons.getLast());

		leds.add (new GestureLED (i, hubConfig, mode));
		addAndMakeVisible (leds.getLast());
	}

	// Control button
	buttons.add (new HubButton (CTRL_ID));
	buttons.getLast()->addListener (this);
	addAndMakeVisible (buttons.getLast());
}

HubComponent::~HubComponent()
{
	TRACE_IN;

	for (int i =0; i < buttons.size(); i++)
	{
		buttons[i]->removeListener (this);
	}

	buttons.clear();
	leds.clear();
}

void HubComponent::paint (Graphics& g)
{
	g.drawImage (hubImage, getLocalBounds().toFloat(),
						   RectanglePlacement::yBottom);
}

void HubComponent::resized()
{
	using namespace neova_dash::ui;

	int hubSize = jmin (getHeight(), getWidth());
	auto buttonArea = Rectangle<int> (hubSize*15/100, hubSize*15/100);

	buttons[0]->setBounds (buttonArea.withCentre ({hubSize*212/1000, hubSize*75/100}));
	buttons[1]->setBounds (buttonArea.withCentre ({hubSize*398/1000, hubSize*817/1000}));
	buttons[2]->setBounds (buttonArea.withCentre ({hubSize*597/1000, hubSize*817/1000}));
	buttons[3]->setBounds (buttonArea.withCentre ({hubSize*785/1000, hubSize*75/100}));

	for (int i=0; i<neova_dash::gesture::NUM_GEST; i++)
	{
		leds[i]->setBounds (buttons[i]->getBounds().withHeight (20).translated (0, jmin (-20, -hubSize*7/100)));
	}

	buttons[CTRL_ID]->setBounds (Rectangle<int> (hubSize*86/1000,
												 hubSize*86/1000)
									.withCentre ({hubSize*788/1000, hubSize*227/1000}));
}

void HubComponent::update()
{
	repaintLEDs();
}

void HubComponent::repaintLEDs()
{
	for (int i =0; i < leds.size(); i++)
	{
		leds[i]->forceOff = (dashboardState != 0);
		leds[i]->repaint();
	}
}

void HubComponent::switchHubMode()
{
	if (mode == presetSelection)
	{
		mode = gestureMute;
		if (ctrlButtonDown) ctrlButtonDown = false;
	}

	else if (mode == gestureMute) mode = presetSelection;

	repaintLEDs();
}

HubComponent::HubMode HubComponent::getCurrentMode()
{
	return mode;
}


bool HubComponent::getControlButtonDown()
{
	return ctrlButtonDown;
}

void HubComponent::setControlButtonDown (const bool shouldBeDown)
{
	ctrlButtonDown = shouldBeDown;
}

void HubComponent::buttonClicked (Button* bttn)
{
	if (auto hubButton = dynamic_cast<HubButton*> (bttn))
	{
		handleHubButtonClick (hubButton->id);
	}
}

void HubComponent::handleHubButtonClick (const int buttonId)
{
	if (buttonId == CTRL_ID) // Control button
	{
		ctrlButtonDown = !ctrlButtonDown;
		switchHubMode();
	}

	else // Bottom buttons -> action depending on the mode
	{
		if (mode == presetSelection)
		{
			hubConfig.setPreset (buttonId);

			// TO DELETE IF UPDATE LOGIC
			currentPreset = buttonId;
			if (!ModifierKeys::currentModifiers.isCommandDown()) switchHubMode();

			commandManager.invokeDirectly (neova_dash::commands::updateDashInterface, true);
		}
		else if (mode == gestureMute)
		{
			if (hubConfig.getGestureData (buttonId).type != neova_dash::gesture::none)
			{
				hubConfig.setUint8Value (buttonId,
										 HubConfiguration::on,
										 (hubConfig.getGestureData (buttonId).on == 0) ? 1 : 0);
				
				hubConfig.setSelectedGesture (buttonId);
				commandManager.invokeDirectly (neova_dash::commands::updateDashInterface, true);
			}
			else
			{
				if (!newGesturePanel.isVisible())
				{
					newGesturePanel.showPanelForGestureID (buttonId);
				}
				else 
				{
					if (newGesturePanel.getLastSelectedSlot() == buttonId)
					{
						newGesturePanel.hidePanel();
					}
					else
					{ 
						newGesturePanel.showPanelForGestureID (buttonId);
					}
				}
			}
		}
	}
}

HubComponent::HubButton::HubButton (const int toggleNum)
	: Button (String ("Preset Button ") + String (toggleNum)),
	  id (toggleNum)
{
}
HubComponent::HubButton::~HubButton()
{
}
void HubComponent::HubButton::resized()
{
}
void HubComponent::HubButton::paintButton (Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
{
	auto ellipseArea = getLocalBounds().withSize (jmin (getWidth(), getHeight()),
											  jmin (getWidth(), getHeight()))
								   .withCentre ({ getLocalBounds().getCentreX(),
								   				  jmin (getWidth(), getHeight())/2 })
								   .reduced (1);

	Colour gradColour = shouldDrawButtonAsDown
					    	? neova_dash::colour::subText.withAlpha (0.07f)
					    	: shouldDrawButtonAsHighlighted ? neova_dash::colour::subText.withAlpha (0.2f)
					    									: Colour (0);

	ColourGradient higlightFill (ColourGradient (gradColour,
												 ellipseArea.getCentre().toFloat(),
												 Colour (0),
												 {float (ellipseArea.getCentreX()), 0.0f},
												 true));
	higlightFill.addColour (0.4f, gradColour);
	higlightFill.addColour (0.8f, Colour (0));

	if (shouldDrawButtonAsHighlighted || shouldDrawButtonAsDown)
	{
		g.setGradientFill (higlightFill);
		g.fillEllipse (ellipseArea.toFloat());
	}
}

//==============================================================================
// GestureLED

HubComponent::GestureLED::GestureLED (const int ledNum, HubConfiguration& config, HubComponent::HubMode& hubMode)
	: id (ledNum), hubConfig (config), mode (hubMode)
{
}
HubComponent::GestureLED::~GestureLED()
{
}
void HubComponent::GestureLED::paint (Graphics& g)
{
	auto ledArea = Rectangle<float> (float (jmin (getHeight(), getWidth())),
									 float (jmin (getHeight(), getWidth()))).withCentre (getLocalBounds().getCentre()
																			                             .toFloat());

	if (!forceOff &&
		((mode == HubComponent::gestureMute && hubConfig.getGestureData (id).type != int (neova_dash::gesture::none))
		|| (mode == HubComponent::presetSelection && hubConfig.getSelectedPreset() == id)))
	{
		Colour ledColour;

		if (mode == HubComponent::presetSelection) ledColour = Colours::white;
		else
		{ 
		 	ledColour = neova_dash::gesture::getHighlightColour
			                 (neova_dash::gesture::intToGestureType (hubConfig.getGestureData (id).type),
			                  hubConfig.getGestureData (id).on == 0 ? false : true);
		}

		ColourGradient ledGrad (ledColour.withAlpha (0.6f), getLocalBounds().getCentre().toFloat(),
								Colour (0), getLocalBounds().getCentre().translated (0, 5).toFloat(),
								true);

		g.setColour (ledColour);
		g.fillEllipse (Rectangle<float>(3.0f, 3.0f).withCentre (ledArea.getCentre()));

		if (hubConfig.getGestureData (id).on != 0)
		{
			g.setGradientFill (ledGrad);
			g.fillEllipse (ledArea);
		}
	}
	else
	{
		g.setColour (Colour (0x80000000));
		g.fillEllipse (Rectangle<float>(3.0f, 3.0f).withCentre (ledArea.getCentre()));
	}
}
void HubComponent::GestureLED::resized()
{
}