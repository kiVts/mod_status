#include "sdk_v3.hpp"
#include <map>
#include <string>
#include <string_view>

namespace {
struct Label { swml_ui_handle handle = 0; std::string base, last; };
std::map<swml_ui_handle, Label> g_labels;

// Observe the game's actual bottom-left version text, including static draw
// traversal. Keep its position/font and append to the same native widget.
// The SDK verifies its native text/update bindings and owns restoration/lifetime.
bool native_version(std::string_view text) {
    if (!text.starts_with('v') || !text.ends_with(" 64Bit")) return false;
    text.remove_prefix(1); text.remove_suffix(6);
    int dots = 0; bool digit = false;
    for (char c : text) {
        if (c >= '0' && c <= '9') digit = true;
        else if (c == '.' && digit) { ++dots; digit = false; }
        else return false;
    }
    return digit && dots == 2;
}

void screen(const swml_ui_event* event, void*) {
    if (event->kind == SWML_UI_UNMOUNT) { g_labels.erase(event->root); return; }
    if (event->kind != SWML_UI_MOUNT && event->kind != SWML_UI_UPDATE) return;
    const auto* api = swml3::api();
    swml_runtime_status status{sizeof(status)};
    if (!swml()->runtime_status(&status) || !status.ready) return;
    auto it = g_labels.find(event->root);
    if (it == g_labels.end()) {
        uint32_t needed = 0;
        swml_ui_info info{sizeof(info)};
        if (api->ui_inspect(swml_self(), event->root, &info) != SWML_OK || info.kind != SWML_UI_LABEL ||
            info.rect.anchor != SWML_UI_BOTTOM_LEFT) return;
        char text[128]{};
        if (api->ui_get_text(swml_self(), event->root, text, sizeof(text), &needed) != SWML_OK || !native_version(text)) return;
        it = g_labels.emplace(event->root, Label{event->root, text, {}}).first;
        swml()->log(swml_self(),"version label found; appending mod status to native text");
    }
    auto& label = it->second;
    char current[256]{}; uint32_t needed = 0;
    if (api->ui_get_text(swml_self(), label.handle, current, sizeof(current), &needed) != SWML_OK) {
        g_labels.erase(it); return;
    }
    if (label.base != current && label.last != current) {
        if (!native_version(current)) return; // another owner changed this text; do not overwrite it
        label.base = current;
    }
    const std::string wanted = label.base + " (" + std::to_string(status.loaded_mods) +
        " mods loaded" + (status.fault_found ? ", fault found)" : ")");
    if (wanted == current) return;
    if (api->ui_set_text(swml_self(), label.handle, wanted.c_str()) == SWML_OK) label.last = wanted;
    else swml_report_fault(swml(), swml_self());
}

bool setup() {
    const auto* api = swml3::api();
    if (!SWML_API_HAS_STATUS(swml()) || !SWML_V3_HAS_WATCH_RECT(api) ||
        !(api->capabilities() & SWML_V3_CAP_UI_APPLICATION_CONTEXT) || !(api->capabilities() & SWML_V3_CAP_UI_STATIC_TEXT_OBSERVER)) {
        mod_status("failed: Mod Status requires the matching SWML core");
        return false;
    }
    const swml_ui_rect filter{.03f, .01f, 2, -1, SWML_UI_BOTTOM_LEFT};
    swml_handle watch = 0;
    const auto result = api->ui_watch_rect(swml_self(), "mm_ui_element_text", &filter, screen, nullptr, &watch);
    if (result != SWML_OK) {
        mod_status("failed: native version label watch unavailable (%d)", result);
        return false;
    }
    swml()->log(swml_self(),"watch installed for mm_ui_element_text (native version label)");
    return true;
}
ModSetup registration{setup};
}
