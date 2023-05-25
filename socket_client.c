#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

#define BUFFER_SIZE 100

typedef unsigned char BYTE;
typedef unsigned long long QWORD;

int main() {
    WSADATA wsa;
    SOCKET client_socket;
    struct sockaddr_in server_addr;
    BYTE recv_buffer[sizeof(QWORD)];
    char send_buffer[MAX_COMPUTERNAME_LENGTH + 1];
    int recv_size;
    DWORD send_size = sizeof(send_buffer);

    // initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("Failed to initialize Winsock\n");
        return 1;
    }

    // create a socket
    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("Failed to create socket\n");
        return 1;
    }

    // prepare the sockaddr_in structure
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("192.168.0.71");
    server_addr.sin_port = htons(8888);

    // connect to the server
    if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        printf("Failed to connect to server\n");
        return 1;
    }

    printf("Connected to server\n");

    // send data to the server
    if (GetComputerNameA(send_buffer, &send_size)) {
        printf("Computer Name: %s\n", send_buffer);
    }
    else {
        perror("Failed to get computer name");
        exit(EXIT_FAILURE);
    }

    send_size = send(client_socket, send_buffer, strlen(send_buffer), 0);
    if (send_size == SOCKET_ERROR) {
        printf("Failed to send data to server\n");
        return 1;
    }

    printf("Sent data to server: %s\n", send_buffer);

    // receive data from the server
    recv_size = recv(client_socket, recv_buffer, sizeof(recv_buffer), 0);
    if (recv_size == SOCKET_ERROR) {
        printf("Failed to receive data from server\n");
        return 1;
    }

    for (int i = 0; i < sizeof(QWORD); i++) {
        printf("%02x ", recv_buffer[i]);
    }
    printf("\n");

    // close socket
    closesocket(client_socket);
    WSACleanup();

    return 0;
}
