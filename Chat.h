#pragma once
#include <iostream>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <sstream>
#include <vector>
#include <sys/select.h>

#define PORT 7777
#define MESSAGE_LENGTH 4096

class ChatClient {
private:
    int serverSocket = -1;
    struct sockaddr_in serverAddressInfo;
    
    //ответы от сервера
    const std::string ADMIN_LOGIN_SUCCESS = "Admin login successful!";
    const std::string USER_LOGIN_SUCCESS = "Login successful!";
    const std::string USER_REGISTER_SUCCESS = "User added successfully!";
    
public:
    ChatClient() = default;
    ~ChatClient() {
        if (serverSocket != -1) {
            close(serverSocket);
        }
    }
    
    bool connectToServer(const std::string& serverAddressStr = "127.0.0.1");
    void run();
    
private:
    std::string sendCommand(const std::string& command);
    
    // Пользовательские методы
    bool registerUser();
    bool login();
    void userPanel();
    void sendMessage();
    void sendPublicMessage();
    void viewPrivateMessages();
    void viewPublicMessages();
    void listUsers();
    
    // Админские методы
    bool LoginAdmin();
    void AdminPanel();
    void banUser();
    void unbanUser();
    void deleteUser();
    void viewOnlineUsers();
    void viewBannedUsers();
};