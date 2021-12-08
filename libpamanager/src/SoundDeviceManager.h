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

namespace LibPAmanager
{
    class SoundDeviceManager
    {
    public:
        static SoundDeviceManager* GetInstance();
        void Start();
        uint GetVolume() const;
        void SetVolume(uint volume);
        void CycleNextOutputDevice();
        void PrintInputDeviceList() const;
        void PrintOutputDeviceList() const;
        std::string& GetDefaultOutputDevice() const;
        void SetOutputDevice(const std::string& description);
        std::vector<std::string>& GetInputDeviceList();
        std::vector<std::string>& GetOutputDeviceList();
    
    private:
        SoundDeviceManager();
        void PulseAudioThread();
    
        static void Mainloop();
        static void SetDefaultDevices();
        static void RemoveInputDevice(uint index);
        static void RemoveOutputDevice(uint index);
        static void SetOutputDevice(const uint outputDevice);
        static void PrintProperties(pa_proplist* props, bool verbose = false);
        static void AddInputDevice(uint index, const char* description, const char* name);
        static void AddOutputDevice(uint index, const char* description, const char* name);
    
        // callback functions
        static void ServerInfoCallback(pa_context* context, const pa_server_info* info, void* userdata);
        static void SinklistCallback(pa_context* context, const pa_sink_info* info, int eol, void* userdata);
        static void SourcelistCallback(pa_context* context, const pa_source_info* info, int eol, void* userdata);
        static void SubscribeCallback(pa_context* context, pa_subscription_event_type_t eventType, uint index, void* userdata);
        static void GetSinkVolumeCallback(pa_context *context, const pa_sink_info *info, int eol, void *userdata);
        static void SetSinkVolumeCallback(pa_context *context, const pa_sink_info *info, int eol, void *userdata);
        static void ContextSuccessCallback(pa_context* context, int success, void* userdata);
        static void ContextStateCallback(pa_context* context, void* userdata);
    
    private:
        static SoundDeviceManager* m_Instance;
        static pa_context* m_Context;
    
        static pa_mainloop*     m_Mainloop;
        static pa_mainloop_api* m_MainloopAPI;
    
        // input devices
        static std::vector<std::string> m_InputDeviceDescriptions;
        static std::vector<uint> m_InputDeviceIndicies;
        static std::vector<std::string> m_InputDeviceNames;
        // output devices
        static std::vector<std::string> m_OutputDeviceDescriptions;
        static std::vector<uint> m_OutputDeviceIndicies;
        static std::vector<std::string> m_OutputDeviceNames;
        
        static uint m_DefaultOutputDeviceVolume;
    
    private:
        struct ServerInfo
        {
            uint m_DefaultInputDeviceIndex;
            uint m_DefaultOutputDeviceIndex;
        };
        static ServerInfo m_ServerInfo;
    
    };
}
