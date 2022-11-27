#pragma once

#include <string>
#include <Eigen/Dense>

#include "hekky-osc.hpp"

class OscServer
{
public:

    OscServer(const std::string& ipAddress, uint32_t port);
    ~OscServer() = default;

public:
    void Tick();
    void BeginPacket();
    void FlushData();
    bool IsAlive();

    void SendPacket_Vector3(const std::string& address, float elem1, float elem2, float elem3);
    void SendPacket_Vector3(const std::string& address, Eigen::Vector3d vector);
    void SendPacket_Quat(const std::string& address, const Eigen::Quaterniond& orientation);

    void Cleanup();

private:
    hekky::osc::UdpSender m_udpSender;
};
