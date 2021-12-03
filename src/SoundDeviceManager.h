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

#pragma once

#include <vector>
#include <pulse/pulseaudio.h>

class SoundDeviceManager
{
public:
    static SoundDeviceManager* GetInstance();
    void Start();
    void PrintList() const;

private:
    SoundDeviceManager();
    void PulseAudioThread();
    
    static void PrintProperties(pa_proplist *props, bool verbose = false);
    static void SinklistCallback(pa_context *c, const pa_sink_info *i, int eol, void *userdata);
    static void SourcelistCallback(pa_context *c, const pa_source_info *i, int eol, void *userdata);
    static void SubscribeCallback(pa_context *c, pa_subscription_event_type_t t, uint32_t index, void *userdata);
    static void ContextStateCallback(pa_context *c, void *userdata);

private:
    static SoundDeviceManager* m_Instance;
    pa_context *m_Context;
    std::vector<std::string> m_DeviceList;
};


