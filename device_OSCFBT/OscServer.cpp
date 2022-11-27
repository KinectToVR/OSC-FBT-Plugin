#include "pch.h"
#define GLOG_NO_ABBREVIATED_SEVERITIES
#include <glog/logging.h>

#include <iostream>

#include "OscServer.h"
#include "Amethyst_API_Devices.h"

OscServer::OscServer(const std::string& ipAddress, uint32_t port)
    : m_udpSender(ipAddress, port)
{
    LOG(INFO) << "Initializing OSC Server";
}

void OscServer::Cleanup()
{
    LOG(INFO) << "Shutting down OSC Server";
    m_udpSender.Close();
}

void OscServer::Tick()
{
}

void OscServer::BeginPacket()
{

}

void OscServer::SendPacket_Vector3(const std::string& address, const float elem1, const float elem2, const float elem3)
{
    auto message = hekky::osc::OscMessage(address);
    message.Push((float) elem1)->Push((float) elem2)->Push((float) elem3);
    m_udpSender.Send(message);
}

void OscServer::SendPacket_Vector3(const std::string& address, Eigen::Vector3d vector)
{
    auto message = hekky::osc::OscMessage(address);
    message.Push( (float) (vector.x()))->Push( (float) (vector.y()))->Push( (float) (vector.z()));
    m_udpSender.Send(message);
}

void OscServer::SendPacket_Quat(const std::string& address, const Eigen::Quaterniond& orientation)
{
    Eigen::Vector3d eulerAngles = orientation.toRotationMatrix().eulerAngles(0, 1, 2);

    auto message = hekky::osc::OscMessage(address);
    message.Push(
        (float) (eulerAngles.x() * math::constants::RAD2DEG))->Push(
        (float) (eulerAngles.y() * math::constants::RAD2DEG))->Push(
        (float) (eulerAngles.z() * math::constants::RAD2DEG));
    m_udpSender.Send(message);
}

void OscServer::FlushData()
{

}

bool OscServer::IsAlive()
{
    return m_udpSender.IsAlive();
}
