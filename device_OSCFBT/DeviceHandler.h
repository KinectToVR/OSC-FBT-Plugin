#pragma once
#include <thread>

#include "Amethyst_API_Devices.h"

/* Not exported */

class OscHandler : public ktvr::K2TrackingDeviceBase_Spectator
{
public:
	/* Device's internal functions & constructors */

	OscHandler()
	{
		// Create a dummy thread for pulling data from Amethyst
		// Wait 5 seconds after init for all things to be set up

		// Run a lambda to spare additional functions' creation
		std::thread([&]
		{
			// Wait these 5 seconds
			std::this_thread::sleep_for(std::chrono::seconds(5));

			// Repeat until exit
			while (true)
			{
				// Grab joint poses, eventually do something with them
				auto const& joints = getAppJointPoses();

				// Send them via OSC

				// Sleep some time not to waste resources, run @50Hz
				std::this_thread::sleep_for(std::chrono::milliseconds(20));
			}
		}).detach();
	}

	virtual ~OscHandler()
	{
	}

public:
	bool sendOscPoses = false;
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
