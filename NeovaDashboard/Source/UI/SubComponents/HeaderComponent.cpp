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
    TRACE_IN;
    batteryComponent = std::make_unique<BatteryComponent> (reader.getFloatValueReference (neova_dash::data::battery),
                                                           config);
    addAndMakeVisible (*batteryComponent);

    createButton();
}

HeaderComponent::~HeaderComponent()
{
    TRACE_IN;

	batteryComponent = nullptr;
}

void HeaderComponent::paint (Graphics& g)
{
    g.setColour (neova_dash::colour::headerBackground);
    g.fillRoundedRectangle (getLocalBounds().toFloat(), 3.0f);

	auto area = getLocalBounds().reduced (0, neova_dash::ui::MARGIN_SMALL);
    g.setColour (neova_dash::colour::mainText);

    g.setFont (neova_dash::font::dashFont.withHeight (19.0f).boldened().withExtraKerningFactor (0.1f));
    g.drawText ("Neova", area, Justification::centredTop, false);

	g.setFont(neova_dash::font::dashFont.withHeight (12.0f));
	g.drawText("DASHBOARD", area, Justification::centredBottom, false);
}

void HeaderComponent::resized()
{
	auto area = getLocalBounds().reduced (neova_dash::ui::MARGIN, 0);

	batteryComponent->setBounds (area.removeFromRight (jmax (area.getWidth()/9, 88)));

    optionsButton->setBounds (area.removeFromLeft (40));
}

void HeaderComponent::buttonClicked (Button* bttn)
{
    if (bttn == optionsButton.get())
    {
        optionsPanel.setVisible (true);
    }
}

void HeaderComponent::createButton()
{
    // Close button
    optionsButton = std::make_unique <DashShapeButton> ("Open Options Button",
                                                       Colour(0),
                                                       neova_dash::colour::mainText);
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
            batteryComponent->startTimer (30000);
        }
        else
        {
            batteryComponent->stopTimer();
        }
    }
}

HeaderComponent::BatteryComponent::BatteryComponent (const float& batteryValRef, HubConfiguration& config)
        : batteryValueRef (batteryValRef), hubConfig (config)
{
    startTimer (30000);
    launchDelayedRepaint (3000);
}

HeaderComponent::BatteryComponent::~BatteryComponent()
{
    stopTimer();
}

void HeaderComponent::BatteryComponent::paint (Graphics& g)
{
	g.setColour (neova_dash::colour::mainText);

	auto area = getLocalBounds().reduced(neova_dash::ui::MARGIN);

    g.setFont (neova_dash::font::dashFont.withHeight (13.0f));
    g.drawText ("Ring :", area.removeFromLeft (area.getWidth()/2), Justification::centred, true);

    auto batteryArea = area.removeFromLeft (area.getWidth()/2)
                           .withSizeKeepingCentre (12, area.getHeight()*3/4);
    drawBatteryPath (g, batteryArea.toFloat());
    
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

    drawConnectedPath (g, area.reduced (area.getWidth()/4, area.getHeight()/4)
                                  .toFloat());
}

void HeaderComponent::BatteryComponent::timerCallback()
{
    if (!waitForRepaint) repaintIfNeeded();
    
    DBG ("Header Timer tick, battery : " << lastBattery);
}

void HeaderComponent::BatteryComponent::repaintIfNeeded()
{
    const float battery = jmax (jmin (neova_dash::data::convertRawBatteryToPercentage (batteryValueRef,
																					   hubConfig.getRingIsCharging()),
                                      1.0f),
                                0.0f);

    if (battery != lastBattery || hubConfig.getRingIsCharging() != lastChargeState)
    {
        lastBattery = battery;
        lastChargeState = hubConfig.getRingIsCharging();
        repaint();
    }
}

void HeaderComponent::BatteryComponent::update()
{
    if (hubConfig.getRingIsConnected() != lastConnectionState)
    {
        lastConnectionState = hubConfig.getRingIsConnected();
        repaint();
    }

    if (hubConfig.getRingIsCharging() != lastChargeState)
    {
        lastChargeState = hubConfig.getRingIsCharging();
        repaint();

        launchDelayedRepaint (3000);
    }
}

void HeaderComponent::BatteryComponent::launchDelayedRepaint (const int delayMs)
{
    auto repaintBatteryLambda = [this] () {
                                              repaintIfNeeded();
                                              waitForRepaint = false;
                                          };

    waitForRepaint = true;
    Timer::callAfterDelay (delayMs, repaintBatteryLambda);
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
    Path batteryOut, batteryTop, batteryFill;

    //batteryOut.addRectangle (area.reduced (1).withTrimmedTop (1)/*, 1.0f*/);
    batteryTop.addRectangle (area.getX() + area.getWidth()*3/8, area.getY(),
                                    area.getWidth()/4, 2.0f);

    auto fillArea = area.withHeight (int ((area.toFloat().getHeight() - 7.0f)*(lastBattery)))
                        .withBottomY (area.getBottom()-3)
                        .reduced (3, 0);
    bool roundedTop = (lastBattery * area.getHeight() < 2.0f);

    g.setColour (neova_dash::colour::mainText);
    //g.strokePath (batteryOut, PathStrokeType (1.0f));
    g.drawRect (area.toNearestIntEdges().reduced (1).withTrimmedTop (1), 1.0f);
    g.fillPath (batteryTop);

    if (lastConnectionState)
    {
        batteryFill.addRectangle (fillArea);

        if (lastChargeState)
        {
            drawLightningPath (batteryFill, area.withTrimmedTop (2).reduced (3, 2));
        }
        batteryFill.setUsingNonZeroWinding (false);

        g.setColour ((lastBattery <= 0.1f) ? Colours::red
                                           : (lastBattery <= 0.2f)
                                                ? Colours::orange
                                                : (lastBattery <= 0.3f)
                                                    ? Colours::yellow
                                                    : (lastBattery == 1.0f)
                                                        ? Colour (0xff8090f0)
                                                        : neova_dash::colour::mainText);
        g.fillPath (batteryFill);
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