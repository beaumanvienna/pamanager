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

SoundDeviceManager* SoundDeviceManager::m_Instance = nullptr;
std::vector<std::string> SoundDeviceManager::m_InputDeviceList;
std::vector<int> SoundDeviceManager::m_InputDeviceIndicies;
std::vector<std::string> SoundDeviceManager::m_OutputDeviceList;
std::vector<int> SoundDeviceManager::m_OutputDeviceIndicies;

SoundDeviceManager::SoundDeviceManager()
{}

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
    for (auto device: m_InputDeviceList)
    {
        LOG_INFO(device);
    }
}

void SoundDeviceManager::PrintOutputDeviceList() const
{
    LOG_TRACE("SoundDeviceManager::PrintOutputDeviceList:");
    for (auto device: m_OutputDeviceList)
    {
        LOG_INFO(device);
    }
}

void SoundDeviceManager::PrintProperties(pa_proplist *props, bool verbose)
{
    if (!verbose) return;

    void *state = nullptr;

    printf("  Properties are: \n");
    while (1)
    {
        const char *key;
        if ((key = pa_proplist_iterate(props, &state)) == nullptr)
        {
            return;
        }
        const char *value = pa_proplist_gets(props, key);
        printf("   key: %s, value: %s\n", key, value);
    }
}

// 
// print information about a sink
// 
void SoundDeviceManager::SinklistCallback(pa_context *c, const pa_sink_info *i, int eol, void *userdata)
{
    LOG_CRITICAL("SinklistCallback");
    // If eol is set to a positive number, you're at the end of the list
    if (eol > 0)
    {
        printf("**No more sinks\n");
        return;
    }
    AddOutputDevice(i->index, i->description);
    printf("Sink: name %s, description -->%s<--, index: %d\n", i->name, i->description, i->index);
    PrintProperties(i->proplist);
}

// 
// print information about a source
// 
void SoundDeviceManager::SourcelistCallback(pa_context *c, const pa_source_info *i, int eol, void *userdata)
{
    LOG_WARN("SourcelistCallback");
    if (eol > 0)
    {
        printf("**No more sources\n");
        return;
    }
    AddInputDevice(i->index, i->description);
    printf("Source: name %s, description -->%s<--, index: %d\n", i->name, i->description, i->index);
    PrintProperties(i->proplist);
}

void SoundDeviceManager::SubscribeCallback(pa_context *c, pa_subscription_event_type_t t, uint index, void *userdata)
{
    switch (t & PA_SUBSCRIPTION_EVENT_FACILITY_MASK)
    {
        case PA_SUBSCRIPTION_EVENT_SINK:
            if ((t & PA_SUBSCRIPTION_EVENT_TYPE_MASK) == PA_SUBSCRIPTION_EVENT_REMOVE)
            {
                RemoveOutputDevice(index);
                printf("Removing sink index %d\n", index);
            }
            else
            {
                pa_operation *operation;
                if (!(operation = pa_context_get_sink_info_by_index(c, index, SinklistCallback, nullptr)))
                {
                    ShowError("pa_context_get_sink_info_by_index() failed");
                    return;
                }
                pa_operation_unref(operation);
            }
            break;
        case PA_SUBSCRIPTION_EVENT_SOURCE:
            if ((t & PA_SUBSCRIPTION_EVENT_TYPE_MASK) == PA_SUBSCRIPTION_EVENT_REMOVE)
            {
                RemoveInputDevice(index);
                printf("Removing source index %d\n", index);
            }
            else
            {
                pa_operation *operation;
                if (!(operation = pa_context_get_source_info_by_index(c, index, SourcelistCallback, nullptr)))
                {
                    ShowError("pa_context_get_source_info_by_index() failed");
                    return;
                }
                pa_operation_unref(operation);
            }
            break;
    }
}

void SoundDeviceManager::ContextStateCallback(pa_context *c, void *userdata)
{
    LOG_WARN("ContextStateCallback");
    switch (pa_context_get_state(c))
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
            pa_operation *operation;

            // set up a callback to tell us about source devices
            if (!(operation = pa_context_get_source_info_list(c,
                                SourcelistCallback,
                                nullptr
                                )))
            {
                ShowError("pa_context_subscribe() failed");
                return;
            }
            pa_operation_unref(operation);

            // set up a callback to tell us about sink devices
            if (!(operation = pa_context_get_sink_info_list(c,
                                SinklistCallback,
                                nullptr
                                ))) {
                ShowError("pa_context_subscribe() failed");
                return;
            }
            pa_operation_unref(operation);

            pa_context_set_subscribe_callback(c, SubscribeCallback, nullptr);
            pa_subscription_mask_t mask = (pa_subscription_mask_t) (PA_SUBSCRIPTION_MASK_SINK| PA_SUBSCRIPTION_MASK_SOURCE);
            if (!(operation = pa_context_subscribe(c, mask, nullptr, nullptr)))
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

void SoundDeviceManager::AddInputDevice(uint index, const char* description)
{
    for (auto deviceIndex : m_InputDeviceIndicies)
    {
        if (deviceIndex == index)
        {
            // already in list
            return;
        }
    }
    m_InputDeviceList.push_back(description);
    m_InputDeviceIndicies.push_back(index);
}

void SoundDeviceManager::RemoveInputDevice(uint index)
{
    uint iterator = 0;
    for (auto deviceIndex : m_InputDeviceIndicies)
    {
        if (deviceIndex == index)
        {
            m_InputDeviceList.erase(m_InputDeviceList.begin() + iterator);
            m_InputDeviceIndicies.erase(m_InputDeviceIndicies.begin() + iterator);
            return;
        }
        iterator++;
    }
}

void SoundDeviceManager::AddOutputDevice(uint index, const char* description)
{
    for (auto deviceIndex : m_OutputDeviceIndicies)
    {
        if (deviceIndex == index)
        {
            // already in list
            return;
        }
    }
    m_OutputDeviceList.push_back(description);
    m_OutputDeviceIndicies.push_back(index);
}

void SoundDeviceManager::RemoveOutputDevice(uint index)
{
    uint iterator = 0;
    for (auto deviceIndex : m_OutputDeviceIndicies)
    {
        if (deviceIndex == index)
        {
            m_OutputDeviceList.erase(m_OutputDeviceList.begin() + iterator);
            m_OutputDeviceIndicies.erase(m_OutputDeviceIndicies.begin() + iterator);
            return;
        }
        iterator++;
    }
}

std::vector<std::string>& SoundDeviceManager::GetInputDeviceList()
{
    return m_InputDeviceList;
}

std::vector<std::string>& SoundDeviceManager::GetOutputDeviceList()
{
    return m_OutputDeviceList;
}

void SoundDeviceManager::SetOutputDevice(const std::string& name)
{
    uint iterator = 0;
    for (auto device : m_OutputDeviceList)
    {
        if (device == name)
        {
            std::string index = std::to_string(m_OutputDeviceIndicies[iterator]);
            std::cout << Color::Modifier(Color::FG_BLUE) 
                      << "SoundDeviceManager::SetOutputDevice: " 
                      << name << ", index: "
                      << index
                      << Color::Modifier(Color::FG_DEFAULT) << std::endl;
            pa_operation *operation;
            operation = pa_context_set_default_sink(m_Context, index.c_str(), ContextSuccessCallback, nullptr);
            pa_operation_unref(operation);
            return;
        }
        iterator++;
    }
    LOG_WARN("SoundDeviceManager::SetOutputDevice: sink not found");
}

void SoundDeviceManager::ContextSuccessCallback(pa_context *c, int success, void *userdata)
{
    std::cout << Color::Modifier(Color::FG_GREEN) 
              << "SoundDeviceManager::ContextSuccessCallback: "
              << (success ? "success" : "failed")
              << Color::Modifier(Color::FG_DEFAULT)
              << std::endl;
}

void SoundDeviceManager::PulseAudioThread()
{

    LOG_INFO("pulseaudio test: list all sources and sinks");

    // Define our pulse audio loop and connection variables
    pa_mainloop *pa_ml;
    pa_mainloop_api *pa_mlapi;

    // Create a mainloop API and connection to the default server
    pa_ml = pa_mainloop_new();
    pa_mlapi = pa_mainloop_get_api(pa_ml);
    m_Context = pa_context_new(pa_mlapi, "Device list");

    // This function connects to the pulse server
    pa_context_connect(m_Context, nullptr, (pa_context_flags_t)0, nullptr);

    // This function defines a callback so the server will tell us its state.
    pa_context_set_state_callback(m_Context, ContextStateCallback, nullptr);

    while(true)
    {
        int ret;
        if (pa_mainloop_iterate(pa_ml, 0, &ret) < 0)
        {
            printf("pa_mainloop_run() failed.");
            exit(1);
        }
        std::this_thread::sleep_for(16ms);
    }
}
