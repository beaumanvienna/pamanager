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

#include "lib.h"
#include "SoundDeviceManager.h"

using namespace std::chrono_literals;

SoundDeviceManager* SoundDeviceManager::m_Instance = nullptr;
pa_context* SoundDeviceManager::m_Context = nullptr;
pa_mainloop*     SoundDeviceManager::m_Mainloop = nullptr;
pa_mainloop_api* SoundDeviceManager::m_MainloopAPI = nullptr;
SoundDeviceManager::ServerInfo SoundDeviceManager::m_ServerInfo = {0,0};

std::vector<std::string> SoundDeviceManager::m_InputDeviceDescriptions;
std::vector<int> SoundDeviceManager::m_InputDeviceIndicies;
std::vector<uint> SoundDeviceManager::m_InputDeviceChannels;
std::vector<std::string> SoundDeviceManager::m_InputDeviceNames;

std::vector<std::string> SoundDeviceManager::m_OutputDeviceDescriptions;
std::vector<int> SoundDeviceManager::m_OutputDeviceIndicies;
std::vector<uint> SoundDeviceManager::m_OutputDeviceChannels;
std::vector<std::string> SoundDeviceManager::m_OutputDeviceNames;

SoundDeviceManager::SoundDeviceManager() {}

// 
// create/provide singleton
// 
SoundDeviceManager* SoundDeviceManager::GetInstance()
{
    if (!m_Instance)
    {
        m_Instance = new SoundDeviceManager();
    }
    return m_Instance;
}

void SoundDeviceManager::Start()
{
    std::thread pulseAudioThread([this](){ PulseAudioThread(); });
    pulseAudioThread.detach();
}

void SoundDeviceManager::PrintInputDeviceList() const
{
    LOG_TRACE("SoundDeviceManager::PrintInputDeviceList:");
    for (auto device: m_InputDeviceDescriptions)
    {
        LOG_INFO(device);
    }
}

void SoundDeviceManager::PrintOutputDeviceList() const
{
    LOG_TRACE("SoundDeviceManager::PrintOutputDeviceList:");
    for (auto device: m_OutputDeviceDescriptions)
    {
        LOG_INFO(device);
    }
}

void SoundDeviceManager::PrintProperties(pa_proplist* props, bool verbose)
{
    if (!verbose) return;

    void* state = nullptr;

    printf("  Properties are: \n");
    while (1)
    {
        const char* key;
        if ((key = pa_proplist_iterate(props, &state)) == nullptr)
        {
            return;
        }
        const char* value = pa_proplist_gets(props, key);
        printf("   key: %s, value: %s\n", key, value);
    }
}

// 
// print information about a sink
// 
void SoundDeviceManager::SinklistCallback(pa_context* context, const pa_sink_info* info, int eol, void* userdata)
{
    LOG_CRITICAL("SinklistCallback");
    // If eol is set to a positive number, you're at the end of the list
    if (eol > 0)
    {
        printf("**No more sinks\n");
        SetDefaultDevices();
        return;
    }
    AddOutputDevice(info->index, info->description, info->name);
    printf("Sink: name %s, description -->%s<--, index: %d\n", info->name, info->description, info->index);
    PrintProperties(info->proplist);
}

// 
// print information about a source
// 
void SoundDeviceManager::SourcelistCallback(pa_context* context, const pa_source_info* info, int eol, void* userdata)
{
    LOG_WARN("SourcelistCallback");
    if (eol > 0)
    {
        printf("**No more sources\n");
        SetDefaultDevices();
        return;
    }
    AddInputDevice(info->index, info->description, info->name);
    printf("Source: name %s, description -->%s<--, index: %d\n", info->name, info->description, info->index);
    PrintProperties(info->proplist);
}

void SoundDeviceManager::SubscribeCallback(pa_context* context, pa_subscription_event_type_t eventType, uint index, void* userdata)
{
    switch (eventType & PA_SUBSCRIPTION_EVENT_FACILITY_MASK)
    {
        case PA_SUBSCRIPTION_EVENT_SINK:
            if ((eventType & PA_SUBSCRIPTION_EVENT_TYPE_MASK) == PA_SUBSCRIPTION_EVENT_REMOVE)
            {
                RemoveOutputDevice(index);
                printf("Removing sink index %d\n", index);
            }
            else
            {
                pa_operation* operation;
                if (!(operation = pa_context_get_sink_info_by_index(context, index, SinklistCallback, nullptr)))
                {
                    ShowError("pa_context_get_sink_info_by_index() failed");
                    return;
                }
                pa_operation_unref(operation);
            }
            break;
        case PA_SUBSCRIPTION_EVENT_SOURCE:
            if ((eventType & PA_SUBSCRIPTION_EVENT_TYPE_MASK) == PA_SUBSCRIPTION_EVENT_REMOVE)
            {
                RemoveInputDevice(index);
                printf("Removing source index %d\n", index);
            }
            else
            {
                pa_operation* operation;
                if (!(operation = pa_context_get_source_info_by_index(context, index, SourcelistCallback, nullptr)))
                {
                    ShowError("pa_context_get_source_info_by_index() failed");
                    return;
                }
                pa_operation_unref(operation);
            }
            break;
    }
}

void SoundDeviceManager::ContextStateCallback(pa_context* context, void* userdata)
{
    LOG_WARN("ContextStateCallback");
    switch (pa_context_get_state(context))
    {
        case PA_CONTEXT_UNCONNECTED:
            LOG_TRACE("ContextStateCallback: PA_CONTEXT_UNCONNECTED");
            break;
        case PA_CONTEXT_CONNECTING:
            LOG_TRACE("ContextStateCallback: PA_CONTEXT_CONNECTING");
            break;
        case PA_CONTEXT_AUTHORIZING:
            LOG_TRACE("ContextStateCallback: PA_CONTEXT_AUTHORIZING");
            break;
        case PA_CONTEXT_SETTING_NAME:
            LOG_TRACE("ContextStateCallback: PA_CONTEXT_SETTING_NAME");
            break;

        case PA_CONTEXT_READY:
        {
            LOG_TRACE("ContextStateCallback: PA_CONTEXT_READY");
            pa_operation* operation;

            // set up a callback to tell us about source devices
            if (!(operation = pa_context_get_source_info_list
                                (
                                    context,
                                    SourcelistCallback,
                                    nullptr
                                )))
            {
                ShowError("pa_context_subscribe() failed");
                return;
            }
            pa_operation_unref(operation);

            // set up a callback to tell us about sink devices
            if (!(operation = pa_context_get_sink_info_list(context,
                                SinklistCallback,
                                nullptr
                                ))) {
                ShowError("pa_context_subscribe() failed");
                return;
            }
            pa_operation_unref(operation);

            pa_context_set_subscribe_callback(context, SubscribeCallback, nullptr);
            pa_subscription_mask_t mask = (pa_subscription_mask_t) (PA_SUBSCRIPTION_MASK_SINK| PA_SUBSCRIPTION_MASK_SOURCE);
            if (!(operation = pa_context_subscribe(context, mask, nullptr, nullptr)))
            {
                ShowError("pa_context_subscribe() failed");
                return;
            }
            pa_operation_unref(operation);

            break;
        }

        case PA_CONTEXT_FAILED:
            LOG_TRACE("ContextStateCallback: PA_CONTEXT_FAILED");
            break;
        case PA_CONTEXT_TERMINATED:
            LOG_TRACE("ContextStateCallback: PA_CONTEXT_TERMINATED");
            break;
        default:
            LOG_TRACE("ContextStateCallback: default");
            break;
    }
}

void SoundDeviceManager::ContextSuccessCallback(pa_context* context, int success, void* userdata)
{
    std::cout << Color::Modifier(Color::FG_GREEN) 
              << "SoundDeviceManager::ContextSuccessCallback: "
              << (success ? "success" : "failed")
              << Color::Modifier(Color::FG_DEFAULT)
              << std::endl;
}

void SoundDeviceManager::ServerInfoCallback(pa_context* context, const pa_server_info* info, void* userdata)
{
    uint iterator = 0;
    for (auto inputDevice : m_InputDeviceNames)
    {
        if (inputDevice == info->default_source_name)
        {
            m_ServerInfo.m_DefaultInputDeviceIndex = iterator;
            break;
        }
        iterator++;
    }
    iterator = 0;
    for (auto outputDevice : m_OutputDeviceNames)
    {
        if (outputDevice == info->default_sink_name)
        {
            m_ServerInfo.m_DefaultOutputDeviceIndex = iterator;
            break;
        }
        iterator++;
    }
    
    std::cout << Color::Modifier(Color::FG_BLUE)
              << "default input:" << m_InputDeviceDescriptions[m_ServerInfo.m_DefaultInputDeviceIndex]
              << Color::Modifier(Color::FG_DEFAULT)
              << std::endl;

    std::cout << Color::Modifier(Color::FG_BLUE)
              << "default output:" << m_OutputDeviceDescriptions[m_ServerInfo.m_DefaultOutputDeviceIndex]
              << Color::Modifier(Color::FG_DEFAULT)
              << std::endl;
}

void SoundDeviceManager::AddInputDevice(uint index, const char* description, const char* name)
{
    for (auto deviceIndex : m_InputDeviceIndicies)
    {
        if (deviceIndex == index)
        {
            // already in list
            return;
        }
    }
    m_InputDeviceDescriptions.push_back(description);
    m_InputDeviceIndicies.push_back(index);
    m_InputDeviceChannels.push_back(2);
    m_InputDeviceNames.push_back(name);
}

void SoundDeviceManager::RemoveInputDevice(uint index)
{
    uint iterator = 0;
    for (auto deviceIndex : m_InputDeviceIndicies)
    {
        if (deviceIndex == index)
        {
            m_InputDeviceDescriptions.erase(m_InputDeviceDescriptions.begin() + iterator);
            m_InputDeviceIndicies.erase(m_InputDeviceIndicies.begin() + iterator);
            m_InputDeviceChannels.erase(m_InputDeviceChannels.begin() + iterator);
            m_InputDeviceNames.erase(m_InputDeviceNames.begin() + iterator);
            return;
        }
        iterator++;
    }
}

void SoundDeviceManager::AddOutputDevice(uint index, const char* description, const char* name)
{
    for (auto deviceIndex : m_OutputDeviceIndicies)
    {
        if (deviceIndex == index)
        {
            // already in list
            return;
        }
    }
    m_OutputDeviceDescriptions.push_back(description);
    m_OutputDeviceIndicies.push_back(index);
    m_OutputDeviceChannels.push_back(2);
    m_OutputDeviceNames.push_back(name);
}

void SoundDeviceManager::RemoveOutputDevice(uint index)
{
    uint iterator = 0;
    for (auto deviceIndex : m_OutputDeviceIndicies)
    {
        if (deviceIndex == index)
        {
            m_OutputDeviceDescriptions.erase(m_OutputDeviceDescriptions.begin() + iterator);
            m_OutputDeviceIndicies.erase(m_OutputDeviceIndicies.begin() + iterator);
            m_OutputDeviceChannels.erase(m_OutputDeviceChannels.begin() + iterator);
            m_OutputDeviceNames.erase(m_OutputDeviceNames.begin() + iterator);
            return;
        }
        iterator++;
    }
}

std::vector<std::string>& SoundDeviceManager::GetInputDeviceList()
{
    return m_InputDeviceDescriptions;
}

std::vector<std::string>& SoundDeviceManager::GetOutputDeviceList()
{
    return m_OutputDeviceDescriptions;
}

void SoundDeviceManager::SetOutputDevice(const std::string& name)
{
    uint iterator = 0;
    for (auto device : m_OutputDeviceDescriptions)
    {
        if (device == name)
        {
            std::string index = std::to_string(m_OutputDeviceIndicies[iterator]);
            std::cout << Color::Modifier(Color::FG_BLUE) 
                      << "SoundDeviceManager::SetOutputDevice: " 
                      << name << ", index: "
                      << index
                      << Color::Modifier(Color::FG_DEFAULT) << std::endl;
            pa_operation* operation;
            operation = pa_context_set_default_sink(m_Context, index.c_str(), ContextSuccessCallback, nullptr);
            pa_operation_unref(operation);
            return;
        }
        iterator++;
    }
    LOG_WARN("SoundDeviceManager::SetOutputDevice: sink not found");
}

void SoundDeviceManager::Mainloop()
{
    int ret;
    if (pa_mainloop_iterate(m_Mainloop, 0, &ret) < 0)
    {
        printf("pa_mainloop_run() failed.");
        exit(1);
    }
    std::this_thread::sleep_for(16ms);
}

void SoundDeviceManager::SetDefaultDevices()
{
    pa_operation* op = pa_context_get_server_info(m_Context, &ServerInfoCallback, nullptr);
    pa_operation_unref(op);
}

std::string& SoundDeviceManager::GetDefaultOutputDevice() const
{
    uint   currentOutputDevice = m_ServerInfo.m_DefaultOutputDeviceIndex;
    return m_OutputDeviceDescriptions[currentOutputDevice];
}

uint SoundDeviceManager::GetVolume() const
{
    uint volume;
    return volume;
}

void SoundDeviceManager::SetVolume(uint volume)
{
    uint sinkNumChannels;
    uint currentOutputDevice = m_ServerInfo.m_DefaultOutputDeviceIndex;
    sinkNumChannels = m_OutputDeviceChannels[currentOutputDevice];

    pa_cvolume cVolume;
    pa_cvolume_init(&cVolume);
    pa_cvolume_set(&cVolume, sinkNumChannels, volume);

    pa_context_set_sink_input_volume
    (
        m_Context,
        currentOutputDevice,
        &cVolume,
        ContextSuccessCallback,
        nullptr
    );
}

void SoundDeviceManager::PulseAudioThread()
{

    LOG_INFO("pulseaudio test: list all sources and sinks");

    // Create a mainloop API and connection to the default server
    m_Mainloop = pa_mainloop_new();
    m_MainloopAPI = pa_mainloop_get_api(m_Mainloop);
    m_Context = pa_context_new(m_MainloopAPI, "Device list");

    // This function connects to the pulse server
    pa_context_connect(m_Context, nullptr, (pa_context_flags_t)0, nullptr);

    // This function defines a callback so the server will tell us its state.
    pa_context_set_state_callback(m_Context, ContextStateCallback, nullptr);

    while(true)
    {
        Mainloop();
    }
}
