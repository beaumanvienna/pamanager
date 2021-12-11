/* Engine Copyright (c) 2021 Engine Development Team
   https://github.com/beaumanvienna/gfxRenderEngine

   Permission is hereby granted, free of charge, to any person
   obtaining a copy of this software and associated documentation files
   (the "Software"), to deal in the Software without restriction,
   including without limitation the rights to use, copy, modify, merge,
   publish, distribute, sublicense, and/or sell copies of the Software,
   and to permit persons to whom the Software is furnished to do so,
   subject to the following conditions:

   The above copyright notice and this permission notice shall be
   included in all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
   OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
   IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
   CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
   TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
   SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. */

#include <chrono>
#include <thread>

#include "main.h"
#include "libpamanager.h"
#include "SoundDeviceManager.h"

using namespace std::chrono_literals;
using namespace LibPAmanager;

void OnEnter(SoundDeviceManager* soundDeviceManager);
void InitSound(SoundDeviceManager* soundDeviceManager);

namespace TestSuite
{
    bool g_DeviceManagerReady = false;
}

//
// test application with a main thread
//
int main()
{
    // start test suite
    PrintMessage(Color::FG_GREEN, "*** pulseaudio device manager test ***");

    LibPAmanager::PrintInfo();
    LibPAmanager::PrintVersion();

    PrintMessage(Color::FG_YELLOW, "press enter to cycle through sound output devices");

    // start profiling
    auto startTime = std::chrono::high_resolution_clock::now();

    // create and initialize sound device manager
    auto soundDeviceManager = SoundDeviceManager::GetInstance();
    InitSound(soundDeviceManager);

    // test suite: add keyboard input
    std::thread onEnter(OnEnter, soundDeviceManager);

    // wait until device manager is online
    do
    {
        std::this_thread::sleep_for(1ms);
    } while (!TestSuite::g_DeviceManagerReady);

    // profiling: calculate elapsed time since start
    auto endTime = std::chrono::high_resolution_clock::now();
    auto start = std::chrono::time_point_cast<std::chrono::milliseconds>(startTime).time_since_epoch();
    auto end   = std::chrono::time_point_cast<std::chrono::milliseconds>(endTime).time_since_epoch();
    auto elapsedTime = end - start;
    PrintMessage(Color::FG_BLUE, std::string("elapsed time in milliseconds: ") + std::to_string(elapsedTime.count()));

    uint volume = 0;
    while(true)
    {
        LOG_MESSAGE("main thread\n");

        // get volume
        volume = soundDeviceManager->GetVolume();

        // set volume
        volume += 1;
        if (volume >= 100)
        {
            volume = 0;
        }
        soundDeviceManager->SetVolume(volume);
        std::this_thread::sleep_for(800ms);   
    }
}

//
// start device manager and set up callback function
//
void InitSound(SoundDeviceManager* soundDeviceManager)
{
    soundDeviceManager->Start();
    
    // the callback is called from the sound device manager's thread
    soundDeviceManager->SetCallback([=](const LibPAmanager::Event& event)
    {
        // react upon event
        PrintMessage(Color::FG_RED, event.PrintType());
        auto eventType = event.GetType();
        switch (eventType)
        {
            case LibPAmanager::Event::DEVICE_MANAGER_READY:
            {
                TestSuite::g_DeviceManagerReady = true;
                break;
            }
            case LibPAmanager::Event::OUTPUT_DEVICE_CHANGED:
            {
                auto device = soundDeviceManager->GetDefaultOutputDevice();
                PrintMessage(Color::FG_BLUE, std::string("output device changed to: ") + device);
                break;
            }
            case LibPAmanager::Event::OUTPUT_DEVICE_LIST_CHANGED:
            {
                auto outputDeviceList = soundDeviceManager->GetOutputDeviceList();
                // user code goes here
                for (auto device : outputDeviceList)
                {
                    PrintMessage(Color::FG_BLUE, std::string("list all output devices: ") + device);
                }
                break;
            }
            case LibPAmanager::Event::OUTPUT_DEVICE_VOLUME_CHANGED:
            {
                auto volume = soundDeviceManager->GetVolume();
                // user code goes here
                PrintMessage(Color::FG_BLUE, std::string("output volume changed to: ") + std::to_string(volume));
                break;
            }
            case LibPAmanager::Event::INPUT_DEVICE_LIST_CHANGED:
            {
                auto inputDeviceList  = soundDeviceManager->GetInputDeviceList();
                // user code goes here
                for (auto device : inputDeviceList)
                {
                    PrintMessage(Color::FG_BLUE, std::string("list all input devices: ") + device);
                }
                break;
            }
        }
    });
}

//
// test code to cycle through sound output devices
//
void OnEnter(SoundDeviceManager* soundDeviceManager)
{
    while (true)
    {
        getchar(); // block until enter is pressed

        soundDeviceManager->PrintInputDeviceList();
        soundDeviceManager->PrintOutputDeviceList();

        soundDeviceManager->CycleNextOutputDevice();
    }
}
