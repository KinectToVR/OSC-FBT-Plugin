#pragma once
#include "Amethyst_API_Devices.h"
#include "miniosc.h"
#include "OscServer.h"
#include "StringUtils.h"

/* Not exported */

class OscHandler : public ktvr::K2TrackingDeviceBase_JointsBasis
{
public:

	OscHandler() {
		deviceName = L"Amethyst OSC";
		Flags_SettingsSupported = true;
		trackedJoints.clear();
	}

	virtual ~OscHandler()
	{
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

private:

	OscServer m_server;

	bool m_hasBeenLoaded = false;
	// bool m_serverIsRunning = false;

	std::vector<ktvr::ITrackedJointType> m_jointMapping;

	// Default IP Address to stream OSC packets to. Currently defaults to localhost.
	std::wstring m_net_target_ip_address = L"127.0.0.1";
	// Default port to stream OSC packets on. Currently defaults to 9000.
	uint32_t m_net_port = 9000;

	// UI Elements
	ktvr::Interface::TextBlock *m_ip_label_text_block, *m_port_label_text_block;
	ktvr::Interface::TextBox *m_ip_text_box, * m_port_text_box;
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
		static OscHandler TrackingHandler; // Create a new device handler -> KinectV2

		*pReturnCode = ktvr::K2InitError_None;
		return &TrackingHandler;
	}

	// Return code for initialization
	*pReturnCode = ktvr::K2InitError_BadInterface;
}
