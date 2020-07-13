// Copyright 2020 Dave Van Ee (heavily based on https://github.com/vvanelslande/vvctre-plugin-shortcuts-free by Valentin Vanelslande)
// Licensed under GPLv2 or any later version
// Refer to the license.txt file included.

#include <cstdint>

#ifdef _WIN32
#define VVCTRE_PLUGIN_EXPORT extern "C" __declspec(dllexport)
#else
#define VVCTRE_PLUGIN_EXPORT extern "C"
#endif

static const char* required_function_names[] = {
    "vvctre_button_device_new",
    "vvctre_button_device_get_state",
    "vvctre_set_paused",
    "vvctre_get_paused",
};

using vvctre_button_device_new_t = void* (*)(void* plugin_manager, const char* params);
using vvctre_button_device_get_state_t = bool (*)(void* device);
using vvctre_set_paused_t = void (*)(void* plugin_manager, bool value);
using vvctre_get_paused_t = bool (*)(void* plugin_manager);

static vvctre_button_device_new_t vvctre_button_device_new;
static vvctre_button_device_get_state_t vvctre_button_device_get_state;
static vvctre_set_paused_t vvctre_set_paused;
static vvctre_get_paused_t vvctre_get_paused;

static void* g_plugin_manager;
static void* g_toggle_pause = nullptr;
static bool g_toggle_pause_pressed = false;

VVCTRE_PLUGIN_EXPORT int GetRequiredFunctionCount() {
    return 4;
}

VVCTRE_PLUGIN_EXPORT const char** GetRequiredFunctionNames() {
    return required_function_names;
}

VVCTRE_PLUGIN_EXPORT void PluginLoaded(void* core, void* plugin_manager,
                                       void* required_functions[]) {
    vvctre_button_device_new = (vvctre_button_device_new_t)required_functions[0];
    vvctre_button_device_get_state = (vvctre_button_device_get_state_t)required_functions[1];
    vvctre_set_paused =
        (vvctre_set_paused_t)required_functions[2];
    vvctre_get_paused =
        (vvctre_get_paused_t)required_functions[3];

    g_plugin_manager = plugin_manager;

    // F8
    g_toggle_pause = vvctre_button_device_new(plugin_manager, "engine:keyboard,code:65");
}

VVCTRE_PLUGIN_EXPORT void BeforeDrawingFPS() {
    if (!g_toggle_pause_pressed &&
        vvctre_button_device_get_state(g_toggle_pause)) {
        g_toggle_pause_pressed = true;
    } else if (g_toggle_pause_pressed &&
               !vvctre_button_device_get_state(g_toggle_pause)) {
        vvctre_set_paused(g_plugin_manager, !vvctre_get_paused(g_plugin_manager));
        g_toggle_pause_pressed = false;
    }
}
