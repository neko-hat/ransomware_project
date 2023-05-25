#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <process.h>  // for _beginthreadex

#pragma comment(lib, "ws2_32.lib")

#define BUFFER_SIZE 100
#define MAX_CLIENTS 50

typedef unsigned char BYTE;
typedef unsigned long long QWORD;

// 클라이언트 정보를 담는 구조체
typedef struct {
    SOCKET socket;
    BYTE send_buffer[sizeof(QWORD)];
    wchar_t desktop_name[BUFFER_SIZE];
} ClientInfo;

// 클라이언트 스레드 함수
CRITICAL_SECTION clientMutex;

// 클라이언트 스레드 함수
unsigned __stdcall clientThread(void* arg) {
    ClientInfo* client = (ClientInfo*)arg;
    SOCKET client_socket = client->socket;
    char recv_buffer[BUFFER_SIZE] = { 0 };
    int recv_size, send_size;

    // 상호 배제를 위한 뮤텍스 락
    EnterCriticalSection(&clientMutex);

    // 키 생성
    srand((unsigned int)time(NULL));
    for (int i = 0; i < sizeof(QWORD); i++) {
        client->send_buffer[i] = (BYTE)rand();
    }

    // 상호 배제를 위한 뮤텍스 언락
    LeaveCriticalSection(&clientMutex);

    // 클라이언트와의 통신
    recv_size = recv(client_socket, recv_buffer, BUFFER_SIZE, 0);
    if (recv_size == SOCKET_ERROR) {
        printf("Failed to receive data from client\n");
    }
    else {
        printf("Received data from client: %s\n", recv_buffer);

        // 상호 배제를 위한 뮤텍스 락
        EnterCriticalSection(&clientMutex);

        send_size = send(client_socket, (char*)client->send_buffer, sizeof(client->send_buffer), 0);

        // 상호 배제를 위한 뮤텍스 언락
        LeaveCriticalSection(&clientMutex);

        if (send_size == SOCKET_ERROR) {
            printf("Failed to send data to client\n");
        }
        else {
            printf("Sent data to client: ");
            for (int i = 0; i < sizeof(QWORD); i++) {
                printf("%02x ", client->send_buffer[i]);
            }
            printf("\n");
        }
    }

    // 소켓 해제
    closesocket(client_socket);
    free(client);
    _endthreadex(0);
    return 0;
}

int main() {
    WSADATA wsa;
    SOCKET listen_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    int client_addr_size = sizeof(client_addr);
    ClientInfo* client;
    // initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("Failed to initialize Winsock\n");
        return 1;
    }

    // create a listening socket
    if ((listen_socket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("Failed to create socket\n");
        return 1;
    }

    // prepare the sockaddr_in structure
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(8888);

    // bind the listening socket to the specified port
    if (bind(listen_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        printf("Failed to bind socket\n");
        return 1;
    }

    // start listening for incoming connections
    if (listen(listen_socket, 3) == SOCKET_ERROR) {
        printf("Failed to start listening\n");
        return 1;
    }
    printf("Waiting for incoming connections...\n");

    // accept incoming connections and create client threads
    HANDLE clientThreads[MAX_CLIENTS];
    int clientCount = 0;

    // Initialize the critical section
    InitializeCriticalSection(&clientMutex);

    while (clientCount < MAX_CLIENTS) {
        if ((client_socket = accept(listen_socket, (struct sockaddr*)&client_addr, &client_addr_size)) == INVALID_SOCKET) {
            printf("Failed to accept incoming connection\n");
            continue;
        }

        printf("Connection accepted from %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
        client = (ClientInfo*)malloc(sizeof(ClientInfo));
        client->socket = client_socket;

        unsigned threadId;

        // 상호 배제를 위한 뮤텍스 락
        EnterCriticalSection(&clientMutex);

        // Create a new ClientInfo struct for each client
        clientThreads[clientCount] = (HANDLE)_beginthreadex(NULL, 0, &clientThread, client, 0, &threadId);

        // 상호 배제를 위한 뮤텍스 언락
        LeaveCriticalSection(&clientMutex);

        clientCount++;
    }

    // Wait for all client threads to finish
    WaitForMultipleObjects(clientCount, clientThreads, TRUE, INFINITE);

    // Close client sockets and clean up
    for (int i = 0; i < clientCount; i++) {
        closesocket(((ClientInfo*)clientThreads[i])->socket);
        free(clientThreads[i]);
    }

    // Close the listening socket and clean up
    closesocket(listen_socket);
    WSACleanup();

    // Delete the critical section
    DeleteCriticalSection(&clientMutex);

    return 0;
}
