#define WIN32_LEAN_AND_MEAN
#include <iostream>
#include <windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma comment(lib, "ws2_32.lib")
using namespace std;

int main() {
    
    WSADATA wsaData;
    ADDRINFO hints;
    ADDRINFO* addrResult = NULL;
    SOCKET ClientSocket = INVALID_SOCKET;
    SOCKET ListenSocket = INVALID_SOCKET;

    const char* sendBuffer = "Hello from server";
    char recvBuffer[512];
    int result;

    result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        cout << "WSAStartup failed, result = " << result << endl;
        return 1;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    result = getaddrinfo(NULL, "666", &hints, &addrResult);
    if (result != 0) {
        cout << "getaddrinfo failed, result = " << result << endl;
        WSACleanup();
        return 1;
    }

    ListenSocket = socket(addrResult->ai_family, addrResult->ai_socktype, addrResult->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {
        cout << "Socket creation failed" << endl;
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }

    result = bind(ListenSocket, addrResult->ai_addr, addrResult->ai_addrlen);
    if (result == SOCKET_ERROR) {
        cout << "Bind Connection failed" << endl;
        closesocket(ListenSocket);
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }
    result = listen(ListenSocket, SOMAXCONN);

    if (result == SOCKET_ERROR) {
        cout << "Listening socket failed" << endl;
        closesocket(ListenSocket);
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }

    ClientSocket = accept(ListenSocket, NULL, NULL);
    if (ClientSocket == INVALID_SOCKET) {
        cout << "Accepting socket failed" << endl;
        closesocket(ListenSocket);
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }

    closesocket(ListenSocket);  

    


    do {
        ZeroMemory(recvBuffer, 512);
        result = recv(ClientSocket, recvBuffer, 512, 0); 

        if (result > 0) {
            cout << "Recv: " << result << " bytes" << endl;
            cout << "Recv data: " << recvBuffer << endl;
            result = send(ClientSocket, sendBuffer, (int)strlen(sendBuffer), 0);
            if (result == SOCKET_ERROR) {
                cout << "FAILED";
                closesocket(ClientSocket);
                freeaddrinfo(addrResult);
                WSACleanup();
                return 1;

            }
        }
        else if (result == 0) {
            cout << "Connection closing" << endl;
        }
        else {
            cout << "recv failed: " << WSAGetLastError() << endl;
        }

    } while (result > 0);

    result = shutdown(ClientSocket,SD_SEND);
    if (result == SOCKET_ERROR) {
        cout << "shutdown cli";
    }
    closesocket(ClientSocket);
    freeaddrinfo(addrResult);
    WSACleanup();
    return 0;
}