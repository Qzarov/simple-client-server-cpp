#include <iostream>
#include <fstream>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>

#include <string.h>
#include <string>

#include <memory>

using namespace std;

class Client {
public:
    Client() 
    {
        // Create a socket
        sock = CreateSocket();
        
        // Create a hint structure for the server
        port = 54000;
        ip_address = "127.0.0.1";
        hint.sin_family = AF_INET;
        hint.sin_port = htons(port);
        inet_pton(AF_INET, ip_address.c_str(), &hint.sin_addr);
    }

    int ConnectToServer() { return connect(sock, (sockaddr *)&hint, sizeof(hint));}
    int GetSocket() { return sock; }
    int GetPort() { return port; }
    string GetIPAddress() { return ip_address; }
    string SendToServer() 
    {
        ifstream file = OpenFile();
        return ReadSendFile(file);
    }


private:
    int CreateSocket() { return socket(AF_INET, SOCK_STREAM, 0); }
    ifstream OpenFile() 
    {
        string file_name;
        ifstream file;

        while (!file.is_open()) {
            cout << "Enter a filename: ";
            cin >> file_name;
            file = ifstream(file_name);

            if (file.is_open()) {
                cout << "file " << file_name << " opened" << endl;
            }
            else {
                cout << "filename " << file_name << " incorrect" << endl;
            }
        }   

        return file;
    }
    
    string ReadSendFile (ifstream& file) 
    {
        string answer;
        string file_input;
        while (file) {
            getline(file, file_input);

            // Send to server
            int send_res = send(sock, file_input.c_str(), file_input.size() + 1, 0);
            if (send_res == -1) {
                cout << "Could not send to server" << endl;
                break;
            }
        }

        return answer;
    }

    char buf[4096];
    int port;
    int sock;
    string ip_address;
    sockaddr_in hint;
};

int main()
{
    Client client;
    cout << "Hi there, i'm CLIENT\n";

    // Connect to the server on the socket
    if (client.ConnectToServer() == -1) { return 1; }

    // /home/qzarov/dif_tmp_stuff/testfile.txt

    cout << "Commands: SEND_FILE, SEND_COMM, END" << endl;
    string command = "DEF_COM";

    string answer;
    while (true) {
        cout << "Enter command: ";
        cin >> command;
        if (command == "SEND_FILE") {
            answer = client.SendToServer();
        } 
        else if (command == "SEND_COMM") {
            //coming soon
        }
        else if (command == "END") { break; }
        else { cout << "unknown command" << endl; }
    }

    cout << "Bye!" << endl;

    // Close socket
    close(client.GetSocket());

    return 0;
}
