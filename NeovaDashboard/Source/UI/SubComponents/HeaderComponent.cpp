/*
  ==============================================================================

    HeaderComponent.cpp
    Created: 3 Sep 2019 11:36:30am
    Author:  Enhancia Dev

  ==============================================================================
*/

#include "HeaderComponent.h"

//==============================================================================
HeaderComponent::HeaderComponent (OptionsPanel& options, HubConfiguration& config, DataReader& reader)
    : optionsPanel (options)
{
    batteryComponent = std::make_unique<BatteryComponent> (reader.getFloatValueReference (neova_dash::data::battery),
                                                           config);
    addAndMakeVisible (*batteryComponent);

    createButton();
}

HeaderComponent::~HeaderComponent()
{
	batteryComponent = nullptr;
}

void HeaderComponent::paint (Graphics& g)
{
    g.setColour (neova_dash::colour::headerBackground);
    g.fillRoundedRectangle (getLocalBounds().toFloat(), 3.0f);
    const float spacing = 7.5f;

    auto area = getLocalBounds().reduced (0, 0);
    g.setColour (neova_dash::colour::mainText);

    g.setFont (neova_dash::font::neovaFont.withHeight (13.5f).withExtraKerningFactor (0.27f));
    g.drawText ("NEOVA", area.removeFromTop (area.getHeight () / 2).withTrimmedBottom (static_cast<int>(spacing / 4.0f)), Justification::centredBottom, false);

  	g.setFont(neova_dash::font::dashFontLight.withHeight (9.5f).withExtraKerningFactor (0.38f));
    g.drawText ("DASH TESTS", area.withTrimmedTop (static_cast<int>(spacing * 3.0f / 4.0f)), Justification::centredTop, false);
}

void HeaderComponent::resized()
{
	auto area = getLocalBounds();

	batteryComponent->setBounds (area.removeFromRight (jmax (area.getWidth()/10, 112)));

    optionsButton->setBounds (area.removeFromLeft (40));
}

void HeaderComponent::buttonClicked (Button* bttn)
{
    if (bttn == optionsButton.get())
    {
        optionsPanel.setVisible (true);
        optionsPanel.grabKeyboardFocus();
    }
}

void HeaderComponent::createButton()
{
    // Close button
    optionsButton = std::make_unique <DashShapeButton> ("Open Options Button",
                                                       Colour(0),
                                                       neova_dash::colour::mainText);
    optionsButton->setComponentID ("optionsButton");
    addAndMakeVisible (*optionsButton);

    optionsButton->setShape (neova_dash::path::createPath (neova_dash::path::options),
                            false, true, false);
    optionsButton->setPaintMode (DashShapeButton::fillAndStroke);
    optionsButton->setStrokeThickness (1.8f);
    optionsButton->addListener (this);
}

void HeaderComponent::update()
{
    batteryComponent->update();
}

void HeaderComponent::setBatteryVisible (bool shouldBeVisible)
{
    if (batteryComponent->isVisible() != shouldBeVisible)
    {
        batteryComponent->setVisible (shouldBeVisible);

        if (shouldBeVisible)
        {
            batteryComponent->startTimer (int (BatteryComponent::batteryCheckTimer), 5000);
        }
        else
        {
            batteryComponent->stopTimer (int (BatteryComponent::batteryCheckTimer));
        }
    }
}

HeaderComponent::BatteryComponent::BatteryComponent (const float& batteryValRef, HubConfiguration& config)
        : batteryValueRef (batteryValRef), hubConfig (config)
{
    lastBattery = jmax (jmin (neova_dash::data::convertRawBatteryToPercentage (batteryValueRef,
                                                                               hubConfig.getRingIsCharging()),
                                      1.0f),
                                0.0f);
    lastRawBattery = batteryValueRef;
    lastConnectionState = hubConfig.getRingIsConnected();

    startTimer (int (batteryCheckTimer), 30000);
    if (lastConnectionState) startTimer (int (blinkTimer), 1000);

    launchDelayedRepaint (2000, true);
}

HeaderComponent::BatteryComponent::~BatteryComponent()
{
    stopTimer (int (batteryCheckTimer));
    //stopTimer (int (blinkTimer));
}

void HeaderComponent::BatteryComponent::paint (Graphics& g)
{
	g.setColour (neova_dash::colour::mainText);

	auto area = getLocalBounds().reduced(neova_dash::ui::MARGIN);

    drawRingPath (g, area.removeFromLeft (area.getWidth()/4).reduced (3).toFloat());

    g.setColour (neova_dash::colour::subText);
    //g.drawText (":", area, Justification::centredLeft);

    indicatorsArea = area.withTrimmedLeft (neova_dash::ui::MARGIN);

	if (lastConnectionState)
	{
		drawBatteryPath(g, indicatorsArea.toFloat());
	}
    else
    {
        drawConnectedPath (g, indicatorsArea.withSizeKeepingCentre (15, area.getHeight()*3/4)
                                            .toFloat());
    }
    
    /* TO TEST : battery percentage display
    if (lastConnectionState)
    {
        g.setFont (neova_dash::font::dashFontLight.withHeight (11.0f));
        
        g.drawText (String (int (lastBattery*100)) + String ("%"),
                    batteryArea.withSizeKeepingCentre (area.getWidth(), batteryArea.getHeight())
                               .withBottom (getHeight())
                               .withTop (batteryArea.getBottom()),
                    Justification::centred);
    }*/

}

void HeaderComponent::BatteryComponent::timerCallback (int timerID)
{
    if (timerID == int (batteryCheckTimer) && !waitForRepaint)
    {
        repaintIfNeeded();
    }    
    else if (timerID == int (blinkTimer))
    {         
        blinkState = !blinkState;
        repaintBlinkingIndicators();
    }    
}

void HeaderComponent::BatteryComponent::repaintIfNeeded (bool forceRepaint)
{
    const float battery = jmax (jmin (neova_dash::data::convertRawBatteryToPercentage (batteryValueRef,
																					   hubConfig.getRingIsCharging()),
                                      1.0f),
                                0.0f);
    const float newRawBattery = batteryValueRef;

    DBG ("----------------\nComputing battery level\nRaw      " << batteryValueRef
                << "\nRounded  " << battery
                << "\nCharging " << (lastChargeState ? "Yes" : "No"));

    if (batteryValueRef < 3.4f && lastConnectionState)
    {
        if (hubConfig.getRingIsCharging() != lastChargeState)
        {
            lastChargeState = hubConfig.getRingIsCharging();
        }

        numBlinkingIndicators = (!lastChargeState && lastBattery == 0.0f) ? 4
                                                                          : (lastChargeState && lastBattery != 1.0f) ? 1
                                                                          : 0;

        // Specific case : ring just got into charging and sends a 0-level battery before computing its actual battery level.
        launchDelayedRepaint (500);
    }

    else if (forceRepaint
             || lastConnectionState && ((battery != lastBattery && ((!lastChargeState && ((newRawBattery - lastRawBattery) < 0.0f) || lastBattery == 0.0f) ||
                                                                    (lastChargeState && (newRawBattery - lastRawBattery) > 0.01f)))
                                        || hubConfig.getRingIsCharging() != lastChargeState))
    {
        lastBattery = battery;
        lastRawBattery = newRawBattery;
        lastChargeState = hubConfig.getRingIsCharging();
        
        numIndicators = battery < 0.0f ? 0
                            : (battery == 0.0f && !lastChargeState) ? 4
                            : battery < 0.2f ? 1
                            : battery < 0.4 ? 2
                            : battery < 0.7 ? 3
                            : 4;

        numBlinkingIndicators = (!lastChargeState && lastBattery == 0.0f) ? 4
                                                                          : (lastChargeState && lastBattery != 1.0f) ? 1
                                                                          : 0;
        repaint();
    }

    else if (hubConfig.getRingIsConnected() != lastConnectionState)
    {
        lastConnectionState = hubConfig.getRingIsConnected();

        if (lastConnectionState) startTimer (int (blinkTimer), 1000);
        else                     stopTimer (int (blinkTimer));
        
        if (lastConnectionState)
        {
            numIndicators = battery < 0.0f ? 0
                                : (battery == 0.0f && !lastChargeState) ? 4
                                : battery < 0.2f ? 1
                                : battery < 0.4 ? 2
                                : battery < 0.7 ? 3
                                : 4;

            numBlinkingIndicators = (!lastChargeState && lastBattery == 0.0f) ? 4
                                                                              : (lastChargeState && lastBattery != 1.0f) ? 1
                                                                              : 0;
        }

        repaint();
    }
}
void HeaderComponent::BatteryComponent::repaintBlinkingIndicators()
{
    if (numBlinkingIndicators != 0)
    {
        if (numBlinkingIndicators == 4) repaint();
        
        else if (numBlinkingIndicators == 1)
        {
            const int indicatorAreaW = indicatorsArea.getWidth()/4;

            repaint (indicatorsArea.withWidth (indicatorAreaW)
                                   .withX (indicatorsArea.getX() + indicatorAreaW * (numIndicators - 1))
                                   .withSizeKeepingCentre (6, 6));
        }
    }
}

void HeaderComponent::BatteryComponent::update()
{
    if (hubConfig.getRingIsConnected() != lastConnectionState)
    {
        repaintIfNeeded(true);
    }

    if (hubConfig.getRingIsCharging() != lastChargeState)
    {
        repaintIfNeeded();
        repaint (getLocalBounds().withRight (15));
        launchDelayedRepaint (1000);
    }
}

void HeaderComponent::BatteryComponent::launchDelayedRepaint (const int delayMs, bool forceRepaint)
{
    auto repaintBatteryLambda = [this] (){
                                              repaintIfNeeded();
                                              waitForRepaint = false;
                                         };
    
    auto repaintBatteryLambdaForce = [this] (){
                                                    repaintIfNeeded(true);
                                                    waitForRepaint = false;
                                              };

    waitForRepaint = true;
    
    if (forceRepaint)   Timer::callAfterDelay (delayMs, repaintBatteryLambdaForce);
    else                Timer::callAfterDelay (delayMs, repaintBatteryLambda);
}

void HeaderComponent::BatteryComponent::drawLightningPath (Path& path, juce::Rectangle<float> area)
{
    Path lightning;

    lightning.startNewSubPath (7.0f, 0.0f);
    lightning.lineTo          (4.0f, 3.0f);
    lightning.lineTo          (8.0f, 3.0f);
    lightning.lineTo          (3.0f, 8.0f);
    lightning.lineTo          (4.0f, 5.0f);
    lightning.lineTo          (0.0f, 5.0f);
    lightning.lineTo          (3.0f, 0.0f);
    lightning.closeSubPath();

    lightning.scaleToFit (area.getX(), area.getY(), area.getWidth(), area.getHeight(), true);

    path.addPath (lightning);
}

void HeaderComponent::BatteryComponent::drawBatteryPath (Graphics& g, juce::Rectangle<float> area)
{
    const Colour fillColour = lastBattery < 0.2f ? Colours::red
                                                 : lastBattery < 0.4f ? Colours::yellow
                                                 : Colours::lime;

    const int indicatorAreaWidth = static_cast<int>(area.getWidth () / 4);

    for (int indicator = 0; indicator < 4; indicator++)
    {
        auto indicatorArea = area.removeFromLeft (static_cast<float>(indicatorAreaWidth));

        if (indicator < numIndicators || numBlinkingIndicators == 4)
        {
            // Blinking fill
            if (numBlinkingIndicators == 4 || (numBlinkingIndicators == 1 && indicator == numIndicators-1))
            {
                //g.setColour (fillColour.withAlpha (0.5f)); // TO DELETE (1st implementation for testing)
                g.setColour (blinkState ? fillColour : neova_dash::colour::dashboardBackground);
            }

            // Standard fill
            else
            {
                g.setColour (fillColour);
            }
        }
        else
        {
            g.setColour (neova_dash::colour::dashboardBackground);
        }
        
        g.fillEllipse (indicatorArea.withSizeKeepingCentre (4, 4));
    }
}

void HeaderComponent::BatteryComponent::drawConnectedPath (Graphics& g, juce::Rectangle<float> area)
{
    g.setColour (lastConnectionState ? neova_dash::colour::mainText
                                     : neova_dash::colour::mainText.withAlpha (0.2f));
    
    g.drawArrow (Line<float> (area.getX() + area.getWidth()/4,
                              area.getY() + area.getHeight()/6,
                              area.getX() + area.getWidth()/4,
                              area.getY() + area.getHeight()),
                 1.5f, 7.0f, 5.0f);

    g.drawArrow (Line<float> (area.getX() + area.getWidth()*3/4,
                              area.getY() + area.getHeight(),
                              area.getX() + area.getWidth()*3/4,
                              area.getY() + area.getHeight()/6),
                 1.5f, 7.0f, 5.0f);
    

    if (!lastConnectionState)
    {
        auto crossArea = area.withSizeKeepingCentre (jmin (area.getWidth(), area.getHeight(),
                                              area.getWidth(), area.getHeight())/2,
                                        jmin (area.getWidth(), area.getHeight(),
                                              area.getWidth(), area.getHeight())/2)
                             .withX (area.getX())
                             .withBottomY (area.getY() + area.getHeight());

        g.setColour (Colours::red);
        
        g.drawLine (Line<float> (crossArea.getX(),
                                 crossArea.getY(),
                                 crossArea.getX() + crossArea.getWidth(),
                                 crossArea.getY() + crossArea.getHeight()), 1.5f);
        g.drawLine (Line<float> (crossArea.getX() + crossArea.getWidth(),
                                 crossArea.getY(),
                                 crossArea.getX(),
                                 crossArea.getY() + crossArea.getHeight()), 1.5f);
    }
}

void HeaderComponent::BatteryComponent::drawRingPath (Graphics& g, juce::Rectangle<float> area)
{
    Path ringPath = neova_dash::path::createPath (neova_dash::path::ringFull);

    ringPath.scaleToFit (area.getX(),
                         area.getY(),
                         area.getWidth(),
                         area.getHeight(),
                         true);

    g.setColour (neova_dash::colour::mainText);

    if (lastChargeState)
    {
        auto zipzoopArea = area.withSizeKeepingCentre (7.0f, 20.0f)
                                         .withX (area.getRight() + 1.0f);
		//g.drawRect(zipzoopArea, 1.0f);
        drawLightningPath (ringPath, zipzoopArea);
    }

    g.fillPath (ringPath);
}