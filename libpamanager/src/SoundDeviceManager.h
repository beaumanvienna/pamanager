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
#include <functional>
#include <pulse/pulseaudio.h>

namespace LibPAmanager
{
    class Event;
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
        bool IsReady() const { return m_Ready; }
        std::string& GetDefaultOutputDevice() const;
        std::vector<std::string>& GetInputDeviceList();
        std::vector<std::string>& GetOutputDeviceList();
        void SetOutputDevice(const std::string& description);
        void SetCallback(std::function<void(const Event&)> callback);

    private:
        SoundDeviceManager();
        void PulseAudioThread();

        static void Mainloop();
        static void SetDefaultVolume();
        static void SetDefaultDevices();
        static void RemoveInputDevice(uint index);
        static void RemoveOutputDevice(uint index);
        static void DummyAppEventCallback(const Event&);
        static void SetOutputDevice(const uint outputDevice);
        static void PrintProperties(pa_proplist* props, bool verbose = false);
        static void AddInputDevice(uint index, const char* description, const char* name);
        static void AddOutputDevice(uint index, const char* description, const char* name, const pa_cvolume& volume);

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
        static bool m_Ready;
        static SoundDeviceManager* m_Instance;
        static pa_context* m_Context;

        static pa_mainloop*     m_Mainloop;
        static pa_mainloop_api* m_MainloopAPI;

        // input devices
        static std::vector<std::string> m_InputDeviceDescriptions;
        static std::vector<uint> m_InputDeviceIndicies;
        static std::vector<std::string> m_InputDeviceNames;
        static uint m_InputDevices;
        // output devices
        static std::vector<std::string> m_OutputDeviceDescriptions;
        static std::vector<uint> m_OutputDeviceIndicies;
        static std::vector<std::string> m_OutputDeviceNames;
        static std::vector<uint> m_OutputDeviceVolumes;
        static uint m_OutputDevices;
        static bool m_SetOutputDevice;

        // callback to alert end user application about events
        static std::function<void(const Event&)> m_ApplicationEventCallback;

    private:
        struct DefaultDevices
        {
            uint m_InputDeviceIndex;
            uint m_OutputDeviceIndex;
            uint m_OutputDeviceVolume;
            uint m_OutputDeviceVolumeRequest;
        };
        static DefaultDevices m_DefaultDevices;

    };

    class Event
    {
    public:
        enum EventType
        {
            DEVICE_MANAGER_READY,
            OUTPUT_DEVICE_CHANGED,
            OUTPUT_DEVICE_VOLUME_CHANGED,
            OUTPUT_DEVICE_LIST_CHANGED,
            INPUT_DEVICE_LIST_CHANGED
        };

    public:
        Event(EventType eventType) : m_EventType(eventType) {}
        virtual ~Event() {}

        auto GetType() const { return m_EventType; }
        std::string PrintType() const;

    private:
        EventType m_EventType;

    };
}
