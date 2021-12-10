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

//
// test application with a main thread
//
int main()
{
    auto soundDeviceManager = SoundDeviceManager::GetInstance();
    soundDeviceManager->Start();

    std::thread onEnter(OnEnter, soundDeviceManager);

    PrintMessage(Color::FG_GREEN, "*** pulseaudio device manager test ***");

    LibPAmanager::PrintInfo();
    LibPAmanager::PrintVersion();

    PrintMessage(Color::FG_YELLOW, "press enter to cycle through sound output devices");

    uint volume = 0;
    std::this_thread::sleep_for(300ms);

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
