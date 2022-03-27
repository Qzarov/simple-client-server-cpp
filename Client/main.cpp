#include <iostream>
#include <fstream>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include <string>

int main() {
    std::cout << "Hi there, i'm CLIENT\n";

    // Create a socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        return 1;
    } 

    // Create a hint structure for the server 
    int port = 54000;
    std::string ip_address = "127.0.0.1";
    
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(port);
    inet_pton(AF_INET, ip_address.c_str(), &hint.sin_addr);

    // Connect to the server on the socket
    int connect_res = connect(sock, (sockaddr*)&hint, sizeof(hint));
    if (connect_res == -1) {
        return 1;
    }

    // While loop:
    char buf[4096];
    std::string file_input;

    // /home/qzarov/dif_tmp_stuff/testfile.txt

    while(file_input != "END") {
        // Enter lines of text
        std::cout << "Your file:" << std::endl;
        //getline(file, user_input);

        std::string file_name;
        std::ifstream file;
        bool is_file_open = false;

        // File opening
        while (!is_file_open) {
            std::cout << "Enter a filename: ";
            std::cin >> file_name;
            file = std::ifstream(file_name);       
        
            if (file.is_open()) {
                std::cout << "file " << file_name << " opened" << std::endl;
                is_file_open = true;
            } else {
                std::cout << "filename " << file_name << " incorrect" << std::endl;
                is_file_open = false;
            }
        }

        // Sending file
        while (file) {
            getline(file, file_input); 
            std::cout << "68, inp: " << file_input << std::endl;
            // Send to server
            int send_res = send(sock, file_input.c_str(), file_input.size() + 1, 0);
            if (send_res == -1) {
                std::cout << "Could not send to server" << std::endl;
                break;
            }
            
            // Wait for response
            memset(buf, 0, 4096);
            int bytes_received = recv(sock, buf, 4096, 0);

            // Display response
            std::cout << "SERVER>" << std::string(buf, bytes_received) << std::endl;
        }
    } 

    std::cout << "Bye!" << std::endl;
    
    // Close socket
    close(sock);

    return 0;
}
