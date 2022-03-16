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
    : presetModeState (presetState), hubConfig (config),
      commandManager (manager), newGesturePanel (newGest), dashboardState (dashState)
{
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

    const int hubSize = jmin (getHeight(), getWidth());
    const auto buttonArea = juce::Rectangle<int> (hubSize*15/100, hubSize*15/100);

	buttons[0]->setBounds (buttonArea.withCentre ({hubSize*212/1000, hubSize*75/100}));
	buttons[1]->setBounds (buttonArea.withCentre ({hubSize*398/1000, hubSize*817/1000}));
	buttons[2]->setBounds (buttonArea.withCentre ({hubSize*597/1000, hubSize*817/1000}));
	buttons[3]->setBounds (buttonArea.withCentre ({hubSize*785/1000, hubSize*75/100}));

	for (int i=0; i<neova_dash::gesture::NUM_GEST; i++)
	{
		leds[i]->setBounds (buttons[i]->getBounds().withHeight (20).translated (0, jmin (-20, -hubSize*6/100))
																   .withSizeKeepingCentre (buttons[i]->getWidth(), hubSize*6/100));
	}

	buttons[CTRL_ID]->setBounds (juce::Rectangle<int> (hubSize*86/1000,
												 hubSize*86/1000)
									.withCentre ({hubSize*788/1000, hubSize*227/1000}));
}

void HubComponent::update() const
{
	repaintLEDs();
}

void HubComponent::repaintLEDs() const
{
	for (int i =0; i < leds.size(); i++)
	{
		leds[i]->forceOff = dashboardState != 0;
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

void HubComponent::setPresetStateToPresetMode (bool notifyChange) const
{
    if (presetModeState == slaveState) return;

    if (presetModeState != presetState)
    {
        presetModeState = static_cast<int>(presetState);
        repaintLEDs();

        if (notifyChange) getCommandManager().invokeDirectly (neova_dash::commands::updatePresetModeState, true);
    }
}

void HubComponent::setPresetStateToNormalMode (bool notifyChange) const
{
    if (presetModeState == slaveState) return;

	if (presetModeState == presetState)
    {
        presetModeState = static_cast<int>(normalState);
        repaintLEDs();

        if (notifyChange) getCommandManager().invokeDirectly (neova_dash::commands::updatePresetModeState, true);
    }
}

bool HubComponent::getControlButtonDown() const
{
	return ctrlButtonDown;
}

void HubComponent::setControlButtonDown (const bool shouldBeDown)
{
	ctrlButtonDown = shouldBeDown;
}

void HubComponent::buttonClicked (Button* bttn)
{
	if (const auto hubButton = dynamic_cast<HubButton*> (bttn))
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
			presetModeState == static_cast<int>(normalState) ? setPresetStateToPresetMode()
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
											 hubConfig.getGestureData (buttonId).on == 0 ? 1 : 0);
					
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
    const auto ellipseArea = getLocalBounds().withSize (jmin (getWidth(), getHeight()),
                                                        jmin (getWidth(), getHeight()))
                                             .withCentre ({ getLocalBounds().getCentreX(),
                                                 jmin (getWidth(), getHeight())/2 })
                                             .reduced (1);

    const Colour gradColour = shouldDrawButtonAsDown
                                  ? neova_dash::colour::subText.withAlpha (0.07f)
                                  : shouldDrawButtonAsHighlighted ? neova_dash::colour::subText.withAlpha (0.2f)
                                  : Colour (0);

    auto higlightFill (ColourGradient (gradColour,
                                       ellipseArea.getCentre().toFloat(),
                                       Colour (0),
                                       {static_cast<float>(ellipseArea.getCentreX()), 0.0f},
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
    const auto ledArea = juce::Rectangle<float> (static_cast<float>(jmin(getHeight(), getWidth())),
                                           static_cast<float>(jmin(getHeight(), getWidth()))).withCentre (getLocalBounds().getCentre()
                                                                                                                          .toFloat());

    const float lightingSizePercent = 0.6f;
    const float lightingAngle = MathConstants<float>::pi/4;
    const float lightingDeltaX = ledArea.getWidth()/4 * (1.0f - lightingSizePercent) * std::cos (lightingAngle);
    const float lightingDeltaY = ledArea.getWidth()/4 * (1.0f - lightingSizePercent) * std::sin (lightingAngle);
    const ColourGradient lightingGrad (Colours::white.withAlpha (0.1f), getLocalBounds().getCentre().toFloat().translated (lightingDeltaX, lightingDeltaY),
                                       Colours::white.withAlpha (0.0f), getLocalBounds().getCentre().toFloat()
                                                                                        .translated (std::cos (lightingAngle) * ledArea.getWidth()/2, 
                                                                                                     std::cos (lightingAngle) * ledArea.getWidth()/2),
                                       true);
	g.setGradientFill (lightingGrad);
	g.fillEllipse (ledArea.withSizeKeepingCentre (ledArea.getWidth()*lightingSizePercent, ledArea.getHeight()*lightingSizePercent)
						  /*.translated (lightingDeltaX, lightingDeltaY)*/);

	ColourGradient shadowGrad(Colours::black.withAlpha(0.8f), getLocalBounds().getCentre().toFloat(),
							  Colours::black.withAlpha(0.0f), getLocalBounds().getCentre().translated(0, static_cast<int>(ledArea.getWidth() / 3)).toFloat(),
							  true);
	shadowGrad.addColour (0.6, Colours::black.withAlpha (0.15f));
	g.setGradientFill (shadowGrad);
	g.fillEllipse (ledArea);

	if (!forceOff &&
		 (presetModeState == normalState && hubConfig.getGestureData (id).type != static_cast<int>(neova_dash::gesture::none)
		 || presetModeState != normalState && hubConfig.getSelectedPreset() == id))
	{
		Colour ledColour;

		if (presetModeState != normalState) ledColour = Colours::white;
		else
		{ 
		 	ledColour = getHighlightColour
			                 (neova_dash::gesture::intToGestureType (hubConfig.getGestureData (id).type),
             hubConfig.getGestureData (id).on == 0 ? false : true);
		}

        const ColourGradient ledGrad(ledColour.withAlpha(0.4f), getLocalBounds().getCentre().toFloat(),
                                     ledColour.withAlpha(0.0f), getLocalBounds().getCentre().translated(0, static_cast<int>(ledArea.getWidth() / 2)).toFloat(),
                                     true);

		g.setColour (ledColour);
		g.fillEllipse (juce::Rectangle<float>(ledArea.getWidth()/4, ledArea.getWidth()/4).withCentre (ledArea.getCentre()));

		if (presetModeState == normalState && hubConfig.getGestureData (id).on != 0
			|| presetModeState != normalState && hubConfig.getSelectedPreset() == id)
		{
			g.setGradientFill (ledGrad);
			g.fillEllipse (ledArea);
		}
	}
	else
	{
		g.setColour (Colour (0xff505050));
		g.fillEllipse (juce::Rectangle<float>(ledArea.getWidth()/4, ledArea.getWidth()/4).withCentre (ledArea.getCentre()));
	}
}
void HubComponent::GestureLED::resized()
{
}