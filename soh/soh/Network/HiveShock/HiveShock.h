#pragma once

#ifdef __cplusplus

#include <cstdint>
#include <string>

// HiveShock: localhost TCP command server for the HiveShock companion app.
// Protocol matches 2s2h TikTok Bridge: one JSON object per line on 0.0.0.0:43000.

namespace HiveShock {
// Enemy counters for this session (only enemies: fairies and bombchus are not counted).
struct Stats {
    int total = 0;        // received so far (waiting + spawned)
    int spawned = 0;      // have entered the game
    int defeated = 0;     // spawned and no longer alive
    int alive = 0;        // alive right now
    int waiting = 0;      // still in the queue
    int eliteBank = 0;    // elite points in use inside the elite bank
    int eliteBankMax = 0; // size of the elite bank (0 = no bank)
    int eliteWaiting = 0; // elite enemies still in the queue
};
Stats GetStats();
// Starts counting from now: what is alive or waiting stays counted, the rest is forgotten.
void ResetStats();
void Enable();
void Disable();
bool IsEnabled();
bool IsListening();
bool IsClientConnected();
uint16_t GetPort(); // TCP port the server listens on (gRemote.HiveShock.Port, default 43000)
std::string GetLocalIP(); // Returns the first non-loopback IPv4 address of the device
// Reacts to an OS memory-pressure signal by shrinking (or clearing) the live bridge-enemy
// budget before the OS reclaims the process. `level` is an Android ComponentCallbacks2
// TRIM_MEMORY_* value; <= 0 means "pressure cleared, restore normal caps". No-op on
// platforms that never call it.
void OnMemoryPressure(int level);
} // namespace HiveShock

#endif // __cplusplus

#ifdef __cplusplus
extern "C" {
#endif

float HiveShock_GetOutgoingDamageMultiplier(void);
// Nonzero for the Kaepora / Mido that a viewer sent (they are not enemies and are handled differently by some hooks).
int HiveShock_IsPestActor(const struct Actor* actor);

#ifdef __cplusplus
}
#endif
