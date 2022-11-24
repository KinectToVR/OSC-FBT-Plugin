#pragma once

#include <string>

#include "miniosc.h"

class OscServer
{
public:
	OscServer() = default;
	OscServer(std::wstring ipAddress, uint32_t port);
	~OscServer();

public:
	int Tick();
	void BeginPacket();
	void SendPacket_Vector3(std::string address, float elem1, float elem2, float elem3);
	void FlushData();

private:
	static void Server_Callback(const char* address, const char* type, void** parameters);

private:
	mobundle packetBundle;
	miniosc* m_oscServer;
};

