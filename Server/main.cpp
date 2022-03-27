#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <string>

int main() 
{
    std::cout << "Hi there, i'm SERVER\n";

    // Create a socket
    int listening = socket(AF_INET, SOCK_STREAM, 0);
    if(listening == -1) {
        std::cerr << "Can't create socket";
        return -1;
    }

    // Bind the socket to a IP and port
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(54000);
    inet_pton(AF_INET, "0.0.0.0", &hint.sin_addr); // 127.0.0.1

    if (bind(listening, (sockaddr*)&hint, sizeof(hint)) == -1) {
        std::cerr << "Can't bind to IP/port" << std::endl;
        return -2;
    }

    // Mark the socket for listening in
    if (listen(listening, SOMAXCONN) == -1) {
        std::cerr << "Can't listen!" << std::endl;
        return -3;
    }

    // Accept a call
    sockaddr_in client;
    socklen_t client_size = sizeof(client);
    char host[NI_MAXHOST];
    char svc[NI_MAXSERV];

    int client_socket = accept(listening, (sockaddr*)&hint, &client_size);

    if (client_socket == -1) {
        std::cerr << "Problem with clients connecting" << std::endl;
        return -4;
    }

    // Close the listening socket
    close(listening);
    memset(host, 0, NI_MAXHOST);
    memset(svc, 0, NI_MAXSERV);

    int result = getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, svc, NI_MAXSERV, 0);

    if (result) {
        std::cout << host << " connected on " << svc << std::endl;
    } else {
        inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
        std::cout << host << " connected on " << ntohs(client.sin_port) << std::endl;
    }
    
    // Wait for a message
    char buf[4096];
    std::string inp;
    while (inp != "SIGTERM" and inp != "SIGHUP") {
        // Clear the buffer
        memset(buf, 0, 4096);
        int bytes_recv = recv(client_socket, buf, 4096, 0);
        
        if (bytes_recv == 0) {
            std::cerr << "The client disconnected" << std::endl;
            break;
        }
        
        if (bytes_recv == -1) {
            std::cerr << "There was a connection issue" << std::endl;
            break;
        }

        // Display message
        inp = std::string(buf, 0, bytes_recv);
        std::cout << "Received: " << inp << std::endl;

        // Redend message
        send(client_socket, buf, bytes_recv + 1, 0);        
    }

    // Close socket
    close(client_socket);

    return 0;
}
