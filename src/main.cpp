
#include <chrono>
#include <thread>

#include <stdio.h>
#include <string.h>
#include <pulse/pulseaudio.h>

#include "colorTTY.h"
#include "SoundDeviceManager.h"

using namespace std::chrono_literals;

int ret;

pa_context *context;

void ShowError(const char *s)
{
    fprintf(stderr, "%s\n", s);
}

void PrintProperties(pa_proplist *props)
{
    return;
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
void sinklist_cb(pa_context *c, const pa_sink_info *i, int eol, void *userdata)
{
    LOG_CRITICAL("sinklist_cb");
    // If eol is set to a positive number, you're at the end of the list
    if (eol > 0)
    {
        printf("**No more sinks\n");
        return;
    }

    printf("Sink: name %s, description %s, index: %d\n", i->name, i->description, i->index);
    PrintProperties(i->proplist);
}

// 
// print information about a source
// 
void sourcelist_cb(pa_context *c, const pa_source_info *i, int eol, void *userdata)
{
    LOG_WARN("sourcelist_cb");
    if (eol > 0)
    {
        printf("**No more sources\n");
        return;
    }

    printf("Source: name %s, description %s, index: %d\n", i->name, i->description, i->index);
    PrintProperties(i->proplist);
}

void subscribe_cb(pa_context *c, pa_subscription_event_type_t t, uint32_t index, void *userdata)
{
    switch (t & PA_SUBSCRIPTION_EVENT_FACILITY_MASK)
    {
        case PA_SUBSCRIPTION_EVENT_SINK:
            if ((t & PA_SUBSCRIPTION_EVENT_TYPE_MASK) == PA_SUBSCRIPTION_EVENT_REMOVE)
            {
                printf("Removing sink index %d\n", index);
            }
            else
            {
                pa_operation *o;
                if (!(o = pa_context_get_sink_info_by_index(c, index, sinklist_cb, nullptr)))
                {
                    ShowError("pa_context_get_sink_info_by_index() failed");
                    return;
                }
                pa_operation_unref(o);
            }
            break;
        case PA_SUBSCRIPTION_EVENT_SOURCE:
            if ((t & PA_SUBSCRIPTION_EVENT_TYPE_MASK) == PA_SUBSCRIPTION_EVENT_REMOVE)
            {
                printf("Removing source index %d\n", index);
            }
            else
            {
                pa_operation *o;
                if (!(o = pa_context_get_source_info_by_index(c, index, sourcelist_cb, nullptr)))
                {
                    ShowError("pa_context_get_source_info_by_index() failed");
                    return;
                }
                pa_operation_unref(o);
            }
            break;
    }
}

void context_state_cb(pa_context *c, void *userdata)
{
    LOG_WARN("context_state_cb");
    switch (pa_context_get_state(c))
    {
        case PA_CONTEXT_UNCONNECTED:
            LOG_TRACE("context_state_cb: PA_CONTEXT_UNCONNECTED");
            break;
        case PA_CONTEXT_CONNECTING:
            LOG_TRACE("context_state_cb: PA_CONTEXT_CONNECTING");
            break;
        case PA_CONTEXT_AUTHORIZING:
            LOG_TRACE("context_state_cb: PA_CONTEXT_AUTHORIZING");
            break;
        case PA_CONTEXT_SETTING_NAME:
            LOG_TRACE("context_state_cb: PA_CONTEXT_SETTING_NAME");
            break;

        case PA_CONTEXT_READY:
        {
            LOG_TRACE("context_state_cb: PA_CONTEXT_READY");
            pa_operation *o;

            // set up a callback to tell us about source devices
            if (!(o = pa_context_get_source_info_list(c,
                                sourcelist_cb,
                                nullptr
                                )))
            {
                ShowError("pa_context_subscribe() failed");
                return;
            }
            pa_operation_unref(o);

            // set up a callback to tell us about sink devices
            if (!(o = pa_context_get_sink_info_list(c,
                                sinklist_cb,
                                nullptr
                                ))) {
                ShowError("pa_context_subscribe() failed");
                return;
            }
            pa_operation_unref(o);

            pa_context_set_subscribe_callback(c, subscribe_cb, nullptr);
            pa_subscription_mask_t mask = (pa_subscription_mask_t) (PA_SUBSCRIPTION_MASK_SINK| PA_SUBSCRIPTION_MASK_SOURCE);
            if (!(o = pa_context_subscribe(c, mask, nullptr, nullptr)))
            {
                ShowError("pa_context_subscribe() failed");
                return;
            }
            pa_operation_unref(o);

            break;
        }

        case PA_CONTEXT_FAILED:
            LOG_TRACE("context_state_cb: PA_CONTEXT_FAILED");
            break;
        case PA_CONTEXT_TERMINATED:
            LOG_TRACE("context_state_cb: PA_CONTEXT_TERMINATED");
            break;
        default:
            LOG_TRACE("context_state_cb: default");
            break;
    }
}

void PulseAudioThread()
{

    LOG_INFO("pulseaudio test: list all sources and sinks");

    // Define our pulse audio loop and connection variables
    pa_mainloop *pa_ml;
    pa_mainloop_api *pa_mlapi;

    // Create a mainloop API and connection to the default server
    pa_ml = pa_mainloop_new();
    pa_mlapi = pa_mainloop_get_api(pa_ml);
    context = pa_context_new(pa_mlapi, "Device list");

    // This function connects to the pulse server
    pa_context_connect(context, nullptr, (pa_context_flags_t)0, nullptr);

    // This function defines a callback so the server will tell us its state.
    pa_context_set_state_callback(context, context_state_cb, nullptr);

    while(true)
    {
        if (pa_mainloop_iterate(pa_ml, 0, &ret) < 0)
        {
            printf("pa_mainloop_run() failed.");
            exit(1);
        }
        std::this_thread::sleep_for(16ms);
    }
}

void OnEnter(SoundDeviceManager* soundDeviceManager)
{
    while (true)
    {
        getchar();
        soundDeviceManager->PrintList();
    }
}

int main()
{
    SoundDeviceManager soundDeviceManager;
    
    std::thread pulseAudioThread(PulseAudioThread);
    std::thread keyBoardCommands(OnEnter, &soundDeviceManager);

    while(true)
    {
        //LOG_INFO("main thread");
        std::this_thread::sleep_for(800ms);
    }
}
