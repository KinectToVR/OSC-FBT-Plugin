#include "pch.h"
#include "DeviceHandler.h"

#include <iostream>
#include <chrono>
#include <ppl.h>

HRESULT OscHandler::getStatusResult()
{
	return S_OK;
}

std::wstring OscHandler::statusResultWString(HRESULT stat)
{
	// Parse your device's status into some nice text here,
	// it has to be formatted like [HEADER]\n[TYPE]\n[MESSAGE]

	switch (stat)
	{
	case S_OK: return L"Success!\nS_OK\nEverything's good!";
	default: return L"Undefined: " + std::to_wstring(stat) +
		L"\nE_UNDEFINED\nSomething weird has happened, though we can't tell what.";
	}
}

void OscHandler::initialize()
{
	// Initialize your device here

	// For the sample's sake, we'll add one dummy joint
	trackedJoints.clear();
	// trackedJoints.push_back(ktvr::K2TrackedJoint(L"Default Joint"));

	// Mark the device as initialized
	initialized = true;
}

void OscHandler::update()
{
	// Update joints' positions here
	// Note: this is fired up every loop

	if (isInitialized())
	{
		// For the plugin's sake, we'll update the joint with
		// the user's head position + 1m in z (front) axis
		// and the user's head orientation

		// trackedJoints[0].update(
		// 	// Position
		// 	getHMDPose().first + Eigen::Vector3d(0, 0, 1),
		// 	// Orientation
		// 	getHMDPose().second,
		// 	// State
		// 	ktvr::State_Tracked
		// );

		// Mark that we see the user
		skeletonTracked = true;
	}
}

void OscHandler::shutdown()
{
	// Turn your device off here
}
