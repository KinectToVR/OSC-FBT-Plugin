#include "pch.h"
#include "OscServer.h"
#include <iostream>

OscServer::OscServer(const std::wstring& ipAddress, uint32_t port)
{
    m_packetBundle = { 0 };
    // Init props

    // Convert IP from wstring to c-style string
    const wchar_t* input = ipAddress.c_str();

    // Count required buffer size (plus one for null-terminator).
    size_t size = (wcslen(input) + 1) * sizeof(wchar_t);
    char* buffer = new char[size];

#ifdef __STDC_LIB_EXT1__
    // wcstombs_s is only guaranteed to be available if __STDC_LIB_EXT1__ is defined
    size_t convertedSize;
    std::wcstombs_s(&convertedSize, buffer, size, input, size);
#else
    std::wcstombs(buffer, input, size);
#endif

    // Use the string stored in "buffer" variable

    int oscError = 0;
    m_oscServer = minioscInit(9003, port, buffer, &oscError);

    std::cout << oscError << std::endl;

    // Free allocated memory:
    delete[] buffer;
}

OscServer::~OscServer()
{
    minioscClose(m_oscServer);
}


void OscServer::Server_Callback(const char* address, const char* type, void** parameters)
{
    // Called when a message is received.  Check "type" to get parameters 
    // This message just blindly assumes it's getting a float.
    // printf("RXCB: %s %s [%p %p] %f\n", address, type, type, parameters[0],
    //     (double)*((float*)parameters[0]));

    std::cout << "RXCB: " << address << " " << type << " " << type << " [" << parameters[0] << " " << (double)*((float*)parameters[0]) << std::endl;
}

int OscServer::Tick() {
    // Poll, waiting for up to 10 ms for a message.
    int r = minioscPoll(m_oscServer, 10, Server_Callback);
    return r;
}

void OscServer::BeginPacket() {
    m_packetBundle = { 0 };
}

void OscServer::SendPacket_Vector3(const std::string& address, float elem1, float elem2, float elem3) {
    minioscBundle(&m_packetBundle, address.c_str(), ",fff", elem1, elem2, elem3);
}

void OscServer::FlushData() {
    minioscSendBundle(m_oscServer, &m_packetBundle);
}