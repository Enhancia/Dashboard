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
                  			ApplicationCommandManager& manager, int& presetState, int& dashState)
    : hubConfig (config), commandManager (manager),
      newGesturePanel (newGest), presetModeState (presetState), dashboardState (dashState)
{
	TRACE_IN;

	currentPreset = hubConfig.getSelectedPreset();

	// Creates Bottom Buttons And Lights
	for (int i =0; i < neova_dash::gesture::NUM_GEST; i++)
	{
		buttons.add (new HubButton (i));
		buttons.getLast()->addListener (this);
		addAndMakeVisible (buttons.getLast());

		leds.add (new GestureLED (i, hubConfig, presetModeState));
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
	if (dashboardState == 0 || dashboardState == 2) // connected or waiting for connection
	{
		g.drawImage (hubConfig.getRingIsCharging() ? hubRingImage : hubImage, getLocalBounds().toFloat(),
							   												   RectanglePlacement::yBottom);
	}
	else 
	{
		g.drawImage (hubRingImage, getLocalBounds().toFloat(), RectanglePlacement::yBottom);
	}
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
		leds[i]->setBounds (buttons[i]->getBounds().withHeight (20).translated (0, jmin (-20, -hubSize*6/100))
																   .withSizeKeepingCentre (buttons[i]->getWidth(), hubSize*6/100));
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

void HubComponent::lockHubToPresetMode (const bool shouldLockHub)
{
	if (locked == shouldLockHub) return;

	locked = shouldLockHub;

	if (locked)
	{
		buttons[CTRL_ID]->setInterceptsMouseClicks (false, false);
		buttons[CTRL_ID]->setState (Button::buttonDown);
	}
	else
	{
		buttons[CTRL_ID]->setState (Button::buttonNormal);
		buttons[CTRL_ID]->setInterceptsMouseClicks (true, false);
	}

	repaintLEDs();
}

void HubComponent::setPresetStateToPresetMode (bool notifyChange)
{
    if (presetModeState == slaveState) return;

    if (presetModeState != presetState)
    {
        presetModeState = int (presetState);
        repaintLEDs();

        if (notifyChange) getCommandManager().invokeDirectly (neova_dash::commands::updatePresetModeState, true);
    }
}

void HubComponent::setPresetStateToNormalMode (bool notifyChange)
{
    if (presetModeState == slaveState) return;

	if (presetModeState == presetState)
    {
        presetModeState = int (normalState);
        repaintLEDs();

        if (notifyChange) getCommandManager().invokeDirectly (neova_dash::commands::updatePresetModeState, true);
    }
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
	if (dashboardState == 0)
	{
		if (buttonId == CTRL_ID && presetModeState != slaveState) // Control button
		{
			presetModeState == int (normalState) ? setPresetStateToPresetMode()
						   						 : setPresetStateToNormalMode();
		}
		else if (buttonId != CTRL_ID) // Bottom buttons -> action depending on the mode
		{
			if (presetModeState == presetState)
			{
				hubConfig.setPreset (buttonId);
				currentPreset = buttonId;

				if (!ModifierKeys::currentModifiers.isCommandDown()) setPresetStateToNormalMode();

				commandManager.invokeDirectly (neova_dash::commands::updateDashInterface, true);
			}
			else if (presetModeState == slaveState)
			{
				hubConfig.setPreset (buttonId);
				currentPreset = buttonId;

				commandManager.invokeDirectly (neova_dash::commands::updateDashInterface, true);
			}
			else if (presetModeState == normalState)
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

HubComponent::GestureLED::GestureLED (const int ledNum, HubConfiguration& config, const int& presetState)
	: id (ledNum), hubConfig (config), presetModeState (presetState)
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

	float lightingSizePercent = 0.6f;
	float lightingAngle = MathConstants<float>::pi/4;
	float lightingDeltaX = ledArea.getWidth()/4 * (1.0f - lightingSizePercent) * std::cos (lightingAngle);
	float lightingDeltaY = ledArea.getWidth()/4 * (1.0f - lightingSizePercent) * std::sin (lightingAngle);
	ColourGradient lightingGrad (Colours::white.withAlpha (0.1f), getLocalBounds().getCentre().toFloat().translated (lightingDeltaX, lightingDeltaY),
							     Colours::white.withAlpha (0.0f), getLocalBounds().getCentre().toFloat()
							     															  .translated (std::cos (lightingAngle) * ledArea.getWidth()/2, 
							     															  	           std::cos (lightingAngle) * ledArea.getWidth()/2),
							     true);
	g.setGradientFill (lightingGrad);
	g.fillEllipse (ledArea.withSizeKeepingCentre (ledArea.getWidth()*lightingSizePercent, ledArea.getHeight()*lightingSizePercent)
						  /*.translated (lightingDeltaX, lightingDeltaY)*/);
  
	ColourGradient shadowGrad (Colours::black.withAlpha (0.8f), getLocalBounds().getCentre().toFloat(),
							   Colours::black.withAlpha (0.0f), getLocalBounds().getCentre().translated (0, ledArea.getWidth()/3).toFloat(),
							   true);
	shadowGrad.addColour (0.6, Colours::black.withAlpha (0.15f));
	g.setGradientFill (shadowGrad);
	g.fillEllipse (ledArea);

	if (!forceOff &&
		 ((presetModeState == HubComponent::normalState && hubConfig.getGestureData (id).type != int (neova_dash::gesture::none))
		 || (presetModeState != HubComponent::normalState && hubConfig.getSelectedPreset() == id)))
	{
		Colour ledColour;

		if (presetModeState != HubComponent::normalState) ledColour = Colours::white;
		else
		{ 
		 	ledColour = neova_dash::gesture::getHighlightColour
			                 (neova_dash::gesture::intToGestureType (hubConfig.getGestureData (id).type),
			                  hubConfig.getGestureData (id).on == 0 ? false : true);
		}

		ColourGradient ledGrad (ledColour.withAlpha (0.4f), getLocalBounds().getCentre().toFloat(),
								ledColour.withAlpha (0.0f), getLocalBounds().getCentre().translated (0, ledArea.getWidth()/2).toFloat(),
								true);

		g.setColour (ledColour);
		g.fillEllipse (Rectangle<float>(ledArea.getWidth()/4, ledArea.getWidth()/4).withCentre (ledArea.getCentre()));

		if ((presetModeState == HubComponent::normalState && hubConfig.getGestureData (id).on != 0)
			|| (presetModeState != HubComponent::normalState && hubConfig.getSelectedPreset() == id))
		{
			g.setGradientFill (ledGrad);
			g.fillEllipse (ledArea);
		}
	}
	else
	{
		g.setColour (Colour (0xff505050));
		g.fillEllipse (Rectangle<float>(ledArea.getWidth()/4, ledArea.getWidth()/4).withCentre (ledArea.getCentre()));
	}
}
void HubComponent::GestureLED::resized()
{
}