#include "SohMenu.h"
#include <soh/Notification/Notification.h>
#include <soh/Network/Network.h>
#include "SohGui.hpp"
#include "soh/OTRGlobals.h"
#include <soh/Network/Sail/Sail.h>
#include <soh/Network/CrowdControl/CrowdControl.h>
#include <soh/Network/HiveShock/HiveShock.h>

namespace SohGui {

extern std::shared_ptr<SohMenu> mSohMenu;
using namespace UIWidgets;

void SohMenu::AddMenuNetwork() {
    // Add Network Menu
    AddMenuEntry("Network", CVAR_SETTING("Menu.NetworkSidebarSection"));
    WidgetPath path;

#ifndef ENABLE_REMOTE_CONTROL
    path = { "Network", "Info", SECTION_COLUMN_1 };
    AddSidebarEntry("Network", path.sidebarName, 2);

    AddWidget(path,
              ICON_FA_EXCLAMATION_TRIANGLE " The Network features are unavailable because SoH was compiled without "
                                           "network support (\"ENABLE_REMOTE_CONTROL\" build flag).",
              WIDGET_TEXT)
        .Options(TextOptions().Color(Colors::Orange));
    return;
#endif

#ifndef __ANDROID__
    // Sail
    path = { "Network", "Sail", SECTION_COLUMN_1 };
    AddSidebarEntry("Network", path.sidebarName, 3);

    AddWidget(path,
              "Sail is a networking protocol designed to facilitate remote "
              "control of the Ship of Harkinian client. It is intended to "
              "be utilized alongside a Sail server, for which we provide a "
              "few straightforward implementations on our GitHub. The current "
              "implementations available allow integration with Twitch chat "
              "and SAMMI Bot, feel free to contribute your own!\n"
              "\n"
              "Click this button to copy the link to the Sail Github "
              "page to your clipboard.",
              WIDGET_TEXT);
    AddWidget(path, ICON_FA_CLIPBOARD "##Sail", WIDGET_BUTTON)
        .Callback([](WidgetInfo& info) {
            ImGui::SetClipboardText("https://github.com/HarbourMasters/sail");
            Notification::Emit({
                .message = "Copied to clipboard",
            });
        })
        .Options(ButtonOptions().Tooltip("https://github.com/HarbourMasters/sail"));
    AddWidget(path, "Host & Port", WIDGET_CUSTOM).CustomFunction([](WidgetInfo& info) {
        ImGui::BeginDisabled(Sail::Instance->isEnabled || CVarGetInteger(CVAR_SETTING("DisableChanges"), 0));
        ImGui::Text("%s", info.name.c_str());
        CVarInputString("##HostSail", CVAR_REMOTE_SAIL("Host"),
                        InputOptions()
                            .Color(THEME_COLOR)
                            .PlaceholderText("127.0.0.1")
                            .DefaultValue("127.0.0.1")
                            .Size(ImVec2(ImGui::GetFontSize() * 15, 0))
                            .LabelPosition(LabelPositions::None));
        ImGui::SameLine();
        ImGui::Text(":");
        ImGui::SameLine();
        CVarInputInt("##PortSail", CVAR_REMOTE_SAIL("Port"),
                     InputOptions()
                         .Color(THEME_COLOR)
                         .PlaceholderText("43384")
                         .DefaultValue("43384")
                         .Size(ImVec2(ImGui::GetFontSize() * 5, 0))
                         .LabelPosition(LabelPositions::None));
        ImGui::EndDisabled();
    });
    AddWidget(path, "Enable##Sail", WIDGET_BUTTON)
        .PreFunc([](WidgetInfo& info) {
            std::string host = CVarGetString(CVAR_REMOTE_SAIL("Host"), "127.0.0.1");
            uint16_t port = CVarGetInteger(CVAR_REMOTE_SAIL("Port"), 43384);
            info.options->disabled = !(!SohUtils::IsStringEmpty(host) && port > 1024 && port < 65535);
            if (Sail::Instance->isEnabled) {
                info.name = "Disable##Sail";
            } else {
                info.name = "Enable##Sail";
            }
        })
        .Callback([](WidgetInfo& info) {
            if (Sail::Instance->isEnabled) {
                CVarClear(CVAR_REMOTE_SAIL("Enabled"));
                Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
                Sail::Instance->Disable();
            } else {
                CVarSetInteger(CVAR_REMOTE_SAIL("Enabled"), 1);
                Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
                Sail::Instance->Enable();
            }
        });
    AddWidget(path, "Connecting...##Sail", WIDGET_TEXT).PreFunc([](WidgetInfo& info) {
        info.isHidden = !Sail::Instance->isEnabled;
        if (Sail::Instance->isConnected) {
            info.name = "Connected##Sail";
        } else {
            info.name = "Connecting...##Sail";
        }
    });

    path.sidebarName = "Crowd Control";
    AddSidebarEntry("Network", path.sidebarName, 3);
    path.column = SECTION_COLUMN_1;

    AddWidget(path, "About Crowd Control", WIDGET_SEPARATOR_TEXT);
    AddWidget(path,
              "Crowd Control is a platform that allows viewers to interact "
              "with a streamer's game in real time.\n"
              "\n"
              "Please head over to www.crowdcontrol.live for more information!",
              WIDGET_TEXT);

    AddWidget(path, "Connect to Crowd Control", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Host & Port", WIDGET_CUSTOM).CustomFunction([](WidgetInfo& info) {
        ImGui::BeginDisabled(CrowdControl::Instance->isEnabled || CVarGetInteger(CVAR_SETTING("DisableChanges"), 0));
        ImGui::Text("%s", info.name.c_str());
        CVarInputString("##HostCrowdControl", CVAR_REMOTE_CROWD_CONTROL("Host"),
                        InputOptions()
                            .Color(THEME_COLOR)
                            .PlaceholderText("127.0.0.1")
                            .DefaultValue("127.0.0.1")
                            .Size(ImVec2(ImGui::GetFontSize() * 15, 0))
                            .LabelPosition(LabelPositions::None));
        ImGui::SameLine();
        ImGui::Text(":");
        ImGui::SameLine();
        CVarInputInt("##PortCrowdControl", CVAR_REMOTE_CROWD_CONTROL("Port"),
                     InputOptions()
                         .Color(THEME_COLOR)
                         .PlaceholderText("43384")
                         .DefaultValue("43384")
                         .Size(ImVec2(ImGui::GetFontSize() * 5, 0))
                         .LabelPosition(LabelPositions::None));
        ImGui::EndDisabled();
    });
    AddWidget(path, "Enable##CrowdControl", WIDGET_BUTTON)
        .PreFunc([](WidgetInfo& info) {
            std::string host = CVarGetString(CVAR_REMOTE_CROWD_CONTROL("Host"), "127.0.0.1");
            uint16_t port = CVarGetInteger(CVAR_REMOTE_CROWD_CONTROL("Port"), 43384);
            info.options->disabled = !(!SohUtils::IsStringEmpty(host) && port > 1024 && port < 65535);
            if (CrowdControl::Instance->isEnabled) {
                info.name = "Disable##CrowdControl";
            } else {
                info.name = "Enable##CrowdControl";
            }
        })
        .Callback([](WidgetInfo& info) {
            if (CrowdControl::Instance->isEnabled) {
                CVarClear(CVAR_REMOTE_CROWD_CONTROL("Enabled"));
                Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
                CrowdControl::Instance->Disable();
            } else {
                CVarSetInteger(CVAR_REMOTE_CROWD_CONTROL("Enabled"), 1);
                Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
                CrowdControl::Instance->Enable();
            }
        });
    AddWidget(path, "Connecting...", WIDGET_TEXT).PreFunc([](WidgetInfo& info) {
        info.isHidden = !CrowdControl::Instance->isEnabled;
        if (CrowdControl::Instance->isConnected) {
            info.name = "Connected";
        } else {
            info.name = "Connecting...";
        }
    });
    AddWidget(path, "Additional Settings", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Enemy Name Tags", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_REMOTE_CROWD_CONTROL("EnemyNameTags"))
        .RaceDisable(true)
        .Options(CheckboxOptions().Tooltip(
            "When viewers spawn enemies, the enemy will have a name tag above them with the viewer's name."));
    AddWidget(path, "Spawned Enemies Ignored Ingame", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_REMOTE_CROWD_CONTROL("SpawnedEnemiesIgnoredIngame"))
        .RaceDisable(true)
        .Options(CheckboxOptions().Tooltip("Enemies spawned by CrowdControl won't be considered for \"clear enemy "
                                           "rooms\", so they don't need to be killed to complete these rooms."));
#endif // !__ANDROID__

    // HiveShock — available on all platforms including Android
    path = { "Network", "HiveShock", SECTION_COLUMN_1 };
    AddSidebarEntry("Network", path.sidebarName, 3);

    AddWidget(path, "About HiveShock", WIDGET_SEPARATOR_TEXT);
    AddWidget(path,
              "HiveShock is a companion app that lets viewers send real-time "
              "commands to the game over your local network.\n"
              "\n"
              "Enable the server below, then connect from the HiveShock app "
              "using the IP and port shown.",
              WIDGET_TEXT);

    AddWidget(path, "Server", WIDGET_SEPARATOR_TEXT);
    // Enable / Disable toggle
    AddWidget(path, "Enable##HiveShock", WIDGET_BUTTON)
        .PreFunc([](WidgetInfo& info) {
            if (HiveShock::IsEnabled()) {
                info.name = "Disable##HiveShock";
            } else {
                info.name = "Enable##HiveShock";
            }
        })
        .Callback([](WidgetInfo& info) {
            if (HiveShock::IsEnabled()) {
                CVarClear(CVAR_REMOTE_HIVESHOCK("Enabled"));
                Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
                HiveShock::Disable();
            } else {
                CVarSetInteger(CVAR_REMOTE_HIVESHOCK("Enabled"), 1);
                Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
                HiveShock::Enable();
            }
        });

    // Device IP + Port info (refreshed every frame)
    AddWidget(path, "##HiveShockAddress", WIDGET_CUSTOM).CustomFunction([](WidgetInfo& info) {
        if (!HiveShock::IsEnabled()) {
            return;
        }
        std::string ip   = HiveShock::GetLocalIP();
        std::string addr = ip + ":" + std::to_string(HiveShock::GetPort());
        ImGui::Text("Connect to:  ");
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(0.4f, 1.0f, 0.4f, 1.0f), "%s", addr.c_str());
        ImGui::SameLine();
        if (ImGui::SmallButton(ICON_FA_CLIPBOARD "##HiveShockCopy")) {
            ImGui::SetClipboardText(addr.c_str());
            Notification::Emit({ .message = "Address copied to clipboard" });
        }
        ImGui::SetItemTooltip("Copy to clipboard");
    });

    // Connection status
    AddWidget(path, "Waiting for client...##HiveShock", WIDGET_TEXT).PreFunc([](WidgetInfo& info) {
        if (!HiveShock::IsEnabled()) {
            info.isHidden = true;
            return;
        }
        info.isHidden = false;
        if (HiveShock::IsClientConnected()) {
            info.name = ICON_FA_CHECK " Client connected##HiveShock";
        } else if (HiveShock::IsListening()) {
            info.name = "Waiting for client...##HiveShock";
        } else {
            info.name = "Starting server...##HiveShock";
        }
    });

    // Port, only editable while the server is stopped.
    AddWidget(path, "Port", WIDGET_CUSTOM).CustomFunction([](WidgetInfo& info) {
        ImGui::BeginDisabled(HiveShock::IsEnabled() || CVarGetInteger(CVAR_SETTING("DisableChanges"), 0));
        ImGui::Text("%s", info.name.c_str());
        CVarInputInt("##PortHiveShock", CVAR_REMOTE_HIVESHOCK("Port"),
                     InputOptions()
                         .Color(THEME_COLOR)
                         .PlaceholderText("43000")
                         .DefaultValue("43000")
                         .Size(ImVec2(ImGui::GetFontSize() * 5, 0))
                         .LabelPosition(LabelPositions::None));
        ImGui::EndDisabled();
    });
    AddWidget(path, "Enemy Load Limit: %d", WIDGET_CVAR_SLIDER_INT)
        .CVar(CVAR_REMOTE_HIVESHOCK("MaxLoad"))
        .Options(IntSliderOptions()
                     .Min(4)
                     .Max(100)
#ifdef __ANDROID__
                     .DefaultValue(8)
#else
                     .DefaultValue(14)
#endif
                     .Format("%d")
                     .Tooltip("How many enemies spawned through HiveShock can be alive at once. Each enemy uses "
                              "1 to 4 points (bats and cuccos 1, Dark Link and Iron Knuckles 4). Extra spawns are "
                              "never discarded: they wait in a queue and enter as enemies are defeated."));
    AddWidget(path, "Elite Head Start: %d", WIDGET_CVAR_SLIDER_INT)
        .CVar(CVAR_REMOTE_HIVESHOCK("EliteReserve"))
        .Options(IntSliderOptions()
                     .Min(0)
                     .Max(40)
#ifdef __ANDROID__
                     .DefaultValue(6)
#else
                     .DefaultValue(10)
#endif
                     .Format("%d")
                     .Tooltip("Points on top of the Enemy Load Limit that only elite enemies (Iron Knuckle, Dark "
                              "Link, Stalfos...) can use, so the first ones enter at once without waiting for "
                              "ordinary enemies to be defeated. It is not a ceiling: elites can also use the ordinary "
                              "limit, and while an elite waits, the room that frees up as ordinary enemies are "
                              "defeated is reserved for it (no new ordinary enemy enters) until it fits. 0 turns the "
                              "head start off."));
    AddWidget(path, "Elite Enemies First", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_REMOTE_HIVESHOCK("ElitePriority"))
        .RaceDisable(false)
        .Options(CheckboxOptions().DefaultValue(true).Tooltip(
            "Elite enemies (Iron Knuckle, Garo Master, Eyegore, Dark Link, Dinolfos, Lizalfos, Stalfos, Garo, "
            "Floormaster and White Wolfos) go ahead of the ordinary ones waiting in the queue. They still respect "
            "the limits below: an elite waits for room like any other, but nothing ordinary is let in before it."));
    AddWidget(path, "Hard Limits (advanced)", WIDGET_SEPARATOR_TEXT);
    AddWidget(path,
              "These two caps protect the game from running out of actor and collision slots. The defaults are safe "
              "(lower on Android, which also halves them automatically when the system is low on memory). Raising "
              "them lets many more enemies be alive at once but can lower the frame rate or make the game unstable, "
              "so raise them gradually and only if you need to. The Enemy Load Limit above is still applied first.",
              WIDGET_TEXT);
    AddWidget(path, "Max HiveShock Enemies Alive: %d", WIDGET_CVAR_SLIDER_INT)
        .CVar(CVAR_REMOTE_HIVESHOCK("MaxAlive"))
        .Options(IntSliderOptions()
                     .Min(8)
                     .Max(80)
#ifdef __ANDROID__
                     .DefaultValue(16)
#else
                     .DefaultValue(32)
#endif
                     .Format("%d")
                     .Tooltip("Most enemies spawned through HiveShock that can be alive at once, whatever their "
                              "weight. Higher values can hurt performance or stability."));
    AddWidget(path, "Max Enemies In The Scene: %d", WIDGET_CVAR_SLIDER_INT)
        .CVar(CVAR_REMOTE_HIVESHOCK("MaxSceneEnemies"))
        .Options(IntSliderOptions()
                     .Min(16)
                     .Max(100)
#ifdef __ANDROID__
                     .DefaultValue(20)
#else
                     .DefaultValue(40)
#endif
                     .Format("%d")
                     .Tooltip("Most enemies allowed in the scene in total, counting the game's own. Spawns wait "
                              "while the scene is at this number. Higher values can hurt performance or "
                              "stability."));
    AddWidget(path, "Defeat Alive Enemies When Link Dies", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_REMOTE_HIVESHOCK("DefeatOnDeath"))
        .RaceDisable(false)
        .Options(CheckboxOptions().Tooltip(
            "When Link dies, the HiveShock enemies that are still alive are removed and counted as defeated, so you "
            "never get stuck with enemies you cannot kill. The spawns still waiting in the queue carry on as usual. "
            "When off, the living enemies follow Link back into the scene after he dies."));
    AddWidget(path, "Enemies Disappear On Zone Change", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_REMOTE_HIVESHOCK("DespawnOnZoneChange"))
        .RaceDisable(false)
        .Options(CheckboxOptions().Tooltip(
            "When Link changes zone (a room inside a dungeon or a loading screen) the HiveShock enemies that are "
            "still alive are left behind and counted as defeated. The spawns still waiting in the queue carry on "
            "and enter in the new zone. When off (default), the living enemies follow Link to the new zone."));
    AddWidget(path, "Enemy Counter", WIDGET_TEXT).PreFunc([](WidgetInfo& info) {
        info.isHidden = !HiveShock::IsEnabled();
        const HiveShock::Stats stats = HiveShock::GetStats();
        info.name = "Spawned " + std::to_string(stats.spawned) + " / " + std::to_string(stats.total) +
                    "  |  Alive " + std::to_string(stats.alive) + "  |  Waiting " + std::to_string(stats.waiting) +
                    "  |  Defeated " + std::to_string(stats.defeated);
        if (stats.eliteBankMax > 0) {
            info.name += "  |  Elite head start " + std::to_string(stats.eliteBank) + "/" +
                         std::to_string(stats.eliteBankMax) + " (" + std::to_string(stats.eliteWaiting) +
                         " waiting)";
        }
    });
    AddWidget(path, "Reset Counter", WIDGET_BUTTON)
        .Options(ButtonOptions().Tooltip("Starts counting from now. Enemies that are alive or waiting stay counted."))
        .PreFunc([](WidgetInfo& info) { info.isHidden = !HiveShock::IsEnabled(); })
        .Callback([](WidgetInfo& info) { HiveShock::ResetStats(); });
    AddWidget(path, "Toggle Enemy Counter Window", WIDGET_WINDOW_BUTTON)
        .CVar(CVAR_WINDOW("HiveShockCounter"))
        .RaceDisable(false)
        .WindowName("HiveShock Counter")
        .Options(WindowButtonOptions().Tooltip(
            "Shows a small on-screen counter of the enemies sent through HiveShock: spawned out of the total "
            "received, alive, waiting in the queue and defeated."));

    path.sidebarName = "Anchor";
    AddSidebarEntry("Network", path.sidebarName, 2);
}

} // namespace SohGui
