#include <iostream>
#include <fstream>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <string>

using namespace std;

class Server {
public:
    Server()
    {
        CreateSocket();
        ListeningSocket();
        AcceptCall();
    }

    int GetSocket() { return listening_sock; }
    int GetClientSocket() { return client_socket; }
    sockaddr_in* GetHint() { return &hint; }
    sockaddr_in GetClient() { return client; }
    char* GetHost() {return host; } 
    char* GetSvc() {return svc; } 

    void CloseListeningSocket() 
    {
        close(listening_sock);
        memset(host, 0, NI_MAXHOST);
        memset(svc, 0, NI_MAXSERV); 

        int res = getnameinfo((sockaddr*)&client, sizeof(client), 
                        host, NI_MAXHOST, svc, NI_MAXSERV, 0);
        
        if (res) {
            cout << host << " connected on " << svc << endl;
        } else {
            sockaddr_in client = client;  
            inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
            cout << host << " connected on " << ntohs(client.sin_port) << endl;
        }
    }

private:
    void CreateSocket()
    {
        listening_sock = socket(AF_INET, SOCK_STREAM, 0);
        if(listening_sock == -1) {
            std::cerr << "Can't create socket";
        }
    }
    void ListeningSocket() 
    {
        hint.sin_family = AF_INET;
        hint.sin_port = htons(54000);
        inet_pton(AF_INET, "0.0.0.0", &hint.sin_addr); // 127.0.0.1

        if (bind(listening_sock, (sockaddr*)&hint, sizeof(hint)) == -1) {
            std::cerr << "Can't bind to IP/port" << std::endl;
        }

        if (listen(listening_sock, SOMAXCONN) == -1) {
            std::cerr << "Can't listen!" << std::endl;
        }
    }
    void AcceptCall()
    {
        socklen_t client_size = sizeof(client);
        client_socket = accept(listening_sock, (sockaddr*)&hint, &client_size);

        if (client_socket == -1) {
            cerr << "Problem with clients connecting" << endl;
        }
    }

    int listening_sock;
    sockaddr_in hint;
    char host[NI_MAXHOST];
    char svc[NI_MAXSERV];
    sockaddr_in client;
    int client_socket;
};

int main() 
{
    Server server;
    cout << "Hi there, i'm SERVER\n";

    
    //int result = server.CloseListeningSocket();


    
    // Wait for a message
    char buf[4096];
    string inp;
    while (inp != "SIGTERM" and inp != "SIGHUP") {
        // Clear the buffer
        memset(buf, 0, 4096);
        int bytes_recv = recv(server.GetClientSocket(), buf, 4096, 0);
        
        
        if (bytes_recv == -1) {
            cerr << "There was a connection issue" << endl;
            break;
        }

        // Display message
        inp = string(buf, 0, bytes_recv);
        cout << "Received: " << inp << endl;
        
        ofstream out_file;
        out_file.open("server_output.txt", ios::app);
        if (out_file.is_open()){
            out_file << inp << endl;
            out_file << "server_output.txt" << endl;
        }
        out_file.close();
        
        // Answer to the client
        string answer = "OK";
        //send(client_socket, (char*)&answer, answer.size() + 1, 0);  
        send(server.GetClientSocket(), buf, bytes_recv + 1, 0);       
    }
    // /home/qzarov/dif_tmp_stuff/testfile.txt

    // Close socket
    close(server.GetClientSocket());

    return 0;
}
