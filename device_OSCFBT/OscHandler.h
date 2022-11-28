#pragma once
#include <set>
#include <chrono>
#include <timeapi.h>

#include "Amethyst_API_Devices.h"
#include "Amethyst_API_Paths.h"

#include "OscServer.h"

std::function<void(std::wstring)> globalLogFunc;

/* Not exported */

inline uint64_t timeSinceEpochMillis() {
    return duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

// 20ms -> 50FPS
#define TARGET_FRAME_TIME_MILLIS 20
// 1ms
#define MIN_FRAME_TIME_MILLIS 1

class OscHandler : public ktvr::K2TrackingDeviceBase_JointsBasis
{
public:
    OscHandler()
    {
        deviceName = L"Amethyst OSC";
        trackedJoints.clear();

        Flags_SettingsSupported = true; // Yooooo
        Flags_ForceSelfUpdate = true; // Just in case
    }

    ~OscHandler() override
    {
        killServer(); // Just in case
    }

    std::wstring getDeviceGUID() override
    {
        // This ID is unique to this plugin!
        return L"K2VRTEAM-VEND-API1-DVCE-OSCSPCTR";
    }

    HRESULT getStatusResult() override;
    std::wstring statusResultWString(HRESULT stat) override;

    void initialize() override;
    void update() override;
    void onLoad() override;
    void shutdown() override;

private:
    void killServer();
    void initLogging();

private:
    std::shared_ptr<OscServer> m_server;
    std::unique_ptr<std::thread> m_update_server_thread;

    [[noreturn]] void update_loop()
    {
        while (true)
        {
            uint64_t initialTime = timeSinceEpochMillis();
            update(); // Standard update

#if defined(WIN32)
            //###Externet Change the minimum resolution for periodic timers on Windows to 1ms.
            // On some windows systems the default minimum resolution is ~15ms which can have undesiered effects for tracking.
            // This is needed for Windows 10 2004 and prior versions since any app can change the minimum resolution globaly.
            timeBeginPeriod(1);
#endif
            uint64_t deltaTime = timeSinceEpochMillis() - initialTime;
            uint64_t targetTime = max(TARGET_FRAME_TIME_MILLIS - deltaTime, MIN_FRAME_TIME_MILLIS);
            std::this_thread::sleep_for(std::chrono::milliseconds(targetTime));
#if defined(WIN32)
            timeEndPeriod(1);
#endif
        }
    }

    bool m_hasBeenLoaded = false;
    bool m_yoffset_value_change_pending = false;
    Eigen::Vector3d m_tracker_offset {0.0, 0.0, 0.0};

    // Default IP Address to stream OSC packets to. Currently defaults to localhost.
    std::wstring m_net_target_ip_address = L"127.0.0.1";
    // Default port to stream OSC packets on. Currently defaults to 9000.
    uint32_t m_net_port = 9000;
    uint32_t m_net_port_in = 9001;
    const uint32_t OSC_OFFSET_DEFAULT = 150;

    // UI Elements
    ktvr::Interface::TextBlock *m_ip_label_text_block, *m_port_label_text_block, *m_yoffset_label, *m_portIn_label_text_block;
    ktvr::Interface::TextBox *m_ip_text_box, *m_port_text_box, *m_portIn_text_box;
    ktvr::Interface::NumberBox *m_yoffset_number_box;
    ktvr::Interface::Button *m_connect_button;
};

/* Exported for dynamic linking */
extern "C" __declspec(dllexport) void* TrackingDeviceBaseFactory(
    const char* pVersionName, int* pReturnCode)
{
    // Return the device handler for tracking
    // but only if interfaces are the same / up-to-date
    if (0 == strcmp(ktvr::IAME_API_Devices_Version, pVersionName))
    {
        static auto TrackingHandler = new OscHandler(); // Create a new device handler -> owoTrack

        *pReturnCode = ktvr::K2InitError_None;
        return TrackingHandler;

        // If you wanna know why of all things, the osc plugin is constructed by `new` as an exception workaround
        // then you should read this: (it was causing a crash with its destructor, now the destructor isn't called)
        // https://stackoverflow.com/questions/54253145/c-application-crashes-with-atexit-error#comment95332406_54253145
    }

    // Return code for initialization
    *pReturnCode = ktvr::K2InitError_BadInterface;
}