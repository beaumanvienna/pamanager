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
#include "SoundDeviceManager.h"

using namespace std::chrono_literals;
void OnEnter(SoundDeviceManager* soundDeviceManager);

int main()
{
    auto soundDeviceManager = SoundDeviceManager::GetInstance();
    soundDeviceManager->Start();

    std::thread onEnter(OnEnter, soundDeviceManager);

    while(true)
    {
        LOG_CRITICAL("main thread");
        std::this_thread::sleep_for(800ms);
    }
}

//
// test code to cycle through sound output devices
//
void OnEnter(SoundDeviceManager* soundDeviceManager)
{
    uint listEntry = 0;
    while (true)
    {
        getchar();
        soundDeviceManager->PrintInputDeviceList();
        soundDeviceManager->PrintOutputDeviceList();

        auto list = soundDeviceManager->GetOutputDeviceList();
        soundDeviceManager->SetOutputDevice(list[listEntry]);
        listEntry++;
        if (listEntry == list.size())
        {
            listEntry = 0;
        }
    }
}