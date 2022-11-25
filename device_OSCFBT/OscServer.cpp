#include "pch.h"
#define GLOG_NO_ABBREVIATED_SEVERITIES
#include <glog/logging.h>

#include <iostream>

#pragma comment(lib, "ws2_32.lib")

#include "OscServer.h"
#include "Amethyst_API_Devices.h"

OscServer::OscServer(const std::wstring& ipAddress, const uint32_t port)
{
    LOG(INFO) << "Initializing OSC Server";
    m_packetBundle = {nullptr};
    // Init props

    int oscError = 0;
    m_oscServer = minioscInit(9003, port, const_cast<char*>(WStringToString(ipAddress).c_str()), &oscError);

    LOG(INFO) << oscError;
}

OscServer::~OscServer()
{
    LOG(INFO) << "Shutting down OSC Server";
    minioscClose(m_oscServer);
    m_oscServer = nullptr;
}

void OscServer::Server_Callback(const char* address, const char* type, void** parameters)
{
    // Called when a message is received.  Check "type" to get parameters 
    // This message just blindly assumes it's getting a float.
    // printf("RXCB: %s %s [%p %p] %f\n", address, type, type, parameters[0],
    //     (double)*((float*)parameters[0]));

    LOG(INFO) << "OSC::RXCB: " << address << " " << type << " " << type << " [" << 
        parameters[0] << " " << static_cast<double>(*static_cast<float*>(parameters[0]));
}

int OscServer::Tick()
{
    // Poll, waiting for up to 10 ms for a message.
    return minioscPoll(m_oscServer, 10, Server_Callback);
}

void OscServer::BeginPacket()
{
    m_packetBundle = {nullptr};
}

int OscServer::SendPacket_Vector3(const std::string& address, const float elem1, const float elem2, const float elem3)
{
    return minioscBundle(&m_packetBundle, address.c_str(), ",fff", elem1, elem2, elem3);
}

int OscServer::SendPacket_Vector3(const std::string& address, Eigen::Vector3d vector)
{
    return minioscBundle(&m_packetBundle, address.c_str(), ",fff", vector.x(), vector.y(), vector.z());
}

int OscServer::SendPacket_Quat(const std::string& address, const Eigen::Quaterniond& orientation)
{
    Eigen::Vector3d eulerAngles = orientation.toRotationMatrix().eulerAngles(0, 1, 2);
    return minioscBundle(&m_packetBundle, address.c_str(), ",fff", eulerAngles.x(), eulerAngles.y(), eulerAngles.z());
}

int OscServer::FlushData()
{
    return minioscSendBundle(m_oscServer, &m_packetBundle);
}

bool OscServer::IsAlive()
{
    return m_oscServer != nullptr;
}
