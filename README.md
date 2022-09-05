# Neova Dashboard
Holds the JUCE project folder for Enhancia's Dashboard plugin.
This project is no longer maintained by Enhancia and is now public for developpers to consult and/or fork.

**This project is strictly NOT meant for commercial use**. If you have questions or remarks concerning the project or the repository, please contact me at a.levacher@hotmail.fr .

## How to compile the Dashboard
To compile the Dashboard, you must install the [JUCE library](https://juce.com/get-juce) and create a standalone app *.jucer* project called "Neova Dashboard", then link the files from this repo to this project which can easily be done with the Projucer. You can then use the Projucer to export and compile the project.

As newer versions of JUCE are published, new bugs might come up and changes might be needed to get the project to compile. If you want to start from a working project, you can get the last projucer version we used for the project ([JUCE 6.1.6](https://github.com/juce-framework/JUCE/releases/tag/6.1.6)) and go from there.

The code was designed for MacOS and Windows. If you are planning to work on Linux, please note that the project might need severe changes to work properly.

## Using the Dashboard with Neova
To use the Dashboard with your Neova hardware, you need to have installed on your computer:
- The Enhancia Driver Service

The easiest way to go about that is to use [our last Dashboard installer](https://sites.google.com/view/enhancia-website/download?authuser=0) to have those programs properly installed to your machine.
