#include "pch.h"
#define GLOG_NO_ABBREVIATED_SEVERITIES
#include <glog/logging.h>

#include <iostream>
#include <chrono>
#include <ppl.h>

#include "OscHandler.h"

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
	m_jointMapping = {
		// ktvr::ITrackedJointType::Joint_SpineShoulder,
		ktvr::ITrackedJointType::Joint_ElbowLeft,
		ktvr::ITrackedJointType::Joint_ElbowRight,
		ktvr::ITrackedJointType::Joint_SpineMiddle,
		ktvr::ITrackedJointType::Joint_SpineWaist,
		// ktvr::ITrackedJointType::Joint_HipLeft,
		// ktvr::ITrackedJointType::Joint_HipRight,
		ktvr::ITrackedJointType::Joint_KneeLeft,
		ktvr::ITrackedJointType::Joint_AnkleLeft,
		ktvr::ITrackedJointType::Joint_KneeRight,
		ktvr::ITrackedJointType::Joint_AnkleRight,
	};

	// Mark the device as initialized
	initialized = true;
}

void OscHandler::update()
{
	// Update joints' positions here
	// Note: this is fired up every loop

	if (isInitialized() && m_server.IsAlive())
	{
		m_server.Tick();
		m_server.BeginPacket();

		// @TODO: Implement properly
		// m_server.SendPacket_Vector3("/tracking/trackers/{idx}/position", 0, 0, 0);

		auto joints = getAppJointPoses();
		uint32_t jointIdx = 0;

		for (int i = 0; i < m_jointMapping.size(); i++) {

			auto joint = joints[m_jointMapping[i]];

			LOG(INFO) << "Sending packet data...";

			jointIdx++;
			m_server.SendPacket_Vector3	(string_format("/tracking/trackers/{%d}/position", jointIdx), joint.getJointPosition());
			m_server.SendPacket_Quat	(string_format("/tracking/trackers/{%d}/rotation", jointIdx), joint.getJointOrientation());
		}

		m_server.FlushData();
	}
}

void OscHandler::shutdown()
{
	// Turn your device off here
	killServer();
}

void OscHandler::onLoad() {

	// @TODO: Settings stuff here!

	// Create elements
	m_ip_label_text_block = CreateTextBlock(
		requestLocalizedString(L"/Plugins/OSC-Plugin/Settings/Labels/TargetIP") + L" ");
	// m_ip_text_box = CreateTextBlock(m_net_target_ip_address);
	m_ip_text_box = CreateTextBox();
	m_ip_text_box->Text(m_net_target_ip_address);
	m_ip_text_box->Width(131);

	m_port_label_text_block = CreateTextBlock(
		requestLocalizedString(L"/Plugins/OSC-Plugin/Settings/Labels/Port") + L" ");
	// m_port_text_box = CreateTextBlock(std::to_wstring(m_net_port));
	m_port_text_box = CreateTextBox();
	m_port_text_box->Text(std::to_wstring(m_net_port));
	m_port_text_box->Width(80);

	m_connect_button = CreateButton(requestLocalizedString(L"/Plugins/OSC-Plugin/Settings/Labels/Connect") + L" ");
	m_connect_button->Width(212);
	m_connect_button->IsEnabled(true);
	
	// Append the elements to the interface
	layoutRoot->AppendElementPairStack(
		m_ip_label_text_block,
		m_ip_text_box);

	layoutRoot->AppendElementPairStack(
		m_port_label_text_block,
		m_port_text_box);

	layoutRoot->AppendSingleElement(m_connect_button);

	// Mark everything as set up
	m_hasBeenLoaded = true;

	// Callback function
	m_connect_button->OnClick = [&, this](ktvr::Interface::Button* sender) {
		// @TODO: Actually connect trackers
		std::thread([&, this] {
			LOG(INFO) << "Connecting trackers via OSC...";
			if (m_server.IsAlive()) {
				killServer();
				LOG(INFO) << "Killing OSC server...";
			} else {
				LOG(INFO) << "Starting OSC server...";
				m_server = OscServer(m_ip_text_box->Text(), stoi(m_port_text_box->Text()));
				m_connect_button->Content(requestLocalizedString(L"/Plugins/OSC-Plugin/Settings/Labels/Disconnect") + L" ");
			}
		});
	};
}

void OscHandler::killServer() {
	if (m_server.IsAlive()) {
		m_server.~OscServer();
		m_connect_button->Content(requestLocalizedString(L"/Plugins/OSC-Plugin/Settings/Labels/Connect") + L" ");
	}
}