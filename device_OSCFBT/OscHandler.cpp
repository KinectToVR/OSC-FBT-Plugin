#include "pch.h"

#include <iostream>
#include <chrono>
#include <format>
#include <ppl.h>

#include "OscHandler.h"

HRESULT OscHandler::getStatusResult()
{
    return S_OK;
}

std::wstring OscHandler::statusResultWString(const HRESULT stat)
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
    // ignored
}

void OscHandler::update()
{
    // Execute a single update loop (tick)
    if (m_server != nullptr && m_server->IsAlive())
    {
        // LOG(INFO) << "OSC Server is running...";

        m_server->Tick();
        m_server->BeginPacket();

        // @TODO: Implement properly
        // m_server.SendPacket_Vector3("/tracking/trackers/{idx}/position", 0, 0, 0);

        const auto joints = getAppJointPoses();
        uint32_t jointIdx = 0;

        for (const auto& joint : joints)
        {
            // LOG(INFO) << "Preparing packet data...";
            
            // std::wstring msg = L"Name::\"" + joint.getJointName() + L"\"\t; Tracking::" + std::to_wstring(joint.getTrackingState());
            // logInfoMessage(msg);

            if (joint.getTrackingState() == ktvr::State_NotTracked)
                continue;

            // Only accept these joints
            if (joint.getJointName().compare(L"AME-WAIST")    || joint.getJointName().compare(L"AME-CHEST") ||
                joint.getJointName().compare(L"AME-LFOOT")    || joint.getJointName().compare(L"AME-RFOOT") ||
                joint.getJointName().compare(L"AME-LELBOW")   || joint.getJointName().compare(L"AME-RELBOW") ||
                joint.getJointName().compare(L"AME-LKNEE")    || joint.getJointName().compare(L"AME-RKNEE") )
            {

                jointIdx++;
                m_server->SendPacket_Vector3(
                    std::format("/tracking/trackers/{}/position", jointIdx), joint.getJointPosition() + m_tracker_offset);
                m_server->SendPacket_Quat(
                    std::format("/tracking/trackers/{}/rotation", jointIdx), joint.getJointOrientation());
            }
        }

        // LOG(INFO) << "Sent OSC packet!";
        m_server->FlushData();
    }
}

void OscHandler::shutdown()
{
    // Turn your device off here
    logInfoMessage(L"Shutting down OSC server...");
    killServer();
}

void OscHandler::onLoad()
{
    globalLogFunc = logInfoMessage;

    // Create elements
    m_ip_label_text_block = CreateTextBlock(
        requestLocalizedString(L"/Plugins/OSC-Plugin/Settings/Labels/TargetIP") + L" ");
    // m_ip_text_box = CreateTextBlock(m_net_target_ip_address);
    m_ip_text_box = CreateTextBox();
    m_ip_text_box->Text(m_net_target_ip_address);
    m_ip_text_box->Width(146);

    m_port_label_text_block = CreateTextBlock(
        requestLocalizedString(L"/Plugins/OSC-Plugin/Settings/Labels/Port") + L" ");
    // m_port_text_box = CreateTextBlock(std::to_wstring(m_net_port));
    m_port_text_box = CreateTextBox();
    m_port_text_box->Text(std::to_wstring(m_net_port));
    m_port_text_box->Width(95);

    m_portIn_label_text_block = CreateTextBlock(
        requestLocalizedString(L"/Plugins/OSC-Plugin/Settings/Labels/PortIn") + L" ");
    // m_port_text_box = CreateTextBlock(std::to_wstring(m_net_port));
    m_portIn_text_box = CreateTextBox();
    m_portIn_text_box->Text(std::to_wstring(m_net_port_in));
    m_portIn_text_box->Width(95);

    m_connect_button = CreateButton(requestLocalizedString(L"/Plugins/OSC-Plugin/Settings/Labels/Connect") + L" ");
    m_connect_button->Width(227);
    m_connect_button->IsEnabled(true);

    m_yoffset_label = CreateTextBlock(
        requestLocalizedString(L"/Plugins/OSC-Plugin/Settings/Labels/YOffset") + L" ");
    // m_port_text_box = CreateTextBlock(std::to_wstring(m_net_port));
    m_yoffset_number_box = CreateNumberBox(OSC_OFFSET_DEFAULT);
    m_yoffset_number_box->Width(250);
    m_yoffset_number_box->OnValueChanged = [&, this](ktvr::Interface::NumberBox* sender, const int& new_value)
    {
        // Just ignore if it was us
        if (m_yoffset_value_change_pending) return;
        m_yoffset_value_change_pending = true; // Lock

        // Backup to writable
        int _value = new_value;

        // Handle resets
        // if (_value < 0) _value = 75;

        const int fixed_new_value =
            std::clamp(_value, -200, 200);

        sender->Value(fixed_new_value); // Overwrite
        m_tracker_offset.y() = static_cast<double>(fixed_new_value) / 100.0;

        // We're done, unlock the handler
        m_yoffset_value_change_pending = false;
        // @TOOD: Save
    };


    // Append the elements to the interface
    layoutRoot->AppendElementPairStack(
        m_ip_label_text_block,
        m_ip_text_box);

    layoutRoot->AppendElementPairStack(
        m_port_label_text_block,
        m_port_text_box);

    layoutRoot->AppendElementPairStack(
        m_portIn_label_text_block,
        m_portIn_label_text_block);
    
    layoutRoot->AppendSingleElement(m_connect_button);

    layoutRoot->AppendElementPairStack(
        m_yoffset_label,
        m_yoffset_number_box);

    // Mark everything as set up
    m_hasBeenLoaded = true;

    // Callback function
    m_connect_button->OnClick = [&, this](ktvr::Interface::Button* sender)
    {
        // @TODO: Actually connect trackers

        logInfoMessage(L"Connecting trackers via OSC...");
        if (m_server != nullptr && m_server->IsAlive())
        {
            killServer();
            logInfoMessage(L"Killing OSC server...");
        }
        else
        {
            logInfoMessage(L"Starting OSC server...");
            m_server = std::make_shared<OscServer>(
                WStringToString(m_ip_text_box->Text()),
                static_cast<uint32_t>(stoi(m_port_text_box->Text())),
                static_cast<uint32_t>(stoi(m_portIn_text_box->Text()))
            );
            m_connect_button->Content(requestLocalizedString(L"/Plugins/OSC-Plugin/Settings/Labels/Disconnect") + L" ");

            // @HACK: Update isn't called from Amethyst automatically, thus we have to dispatch it ourselves
            if (!m_update_server_thread) // Comment this check to reset each co/re/disconnect
                m_update_server_thread.reset(new std::thread(&OscHandler::update_loop, this));
        }
    };
}

void OscHandler::killServer()
{
    if (m_server != nullptr && m_server->IsAlive())
    {
        m_server->Cleanup();
        m_server.reset();

        m_connect_button->Content(requestLocalizedString(L"/Plugins/OSC-Plugin/Settings/Labels/Connect") + L" ");
    }
}