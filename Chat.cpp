#include "Chat.h"

bool ChatClient::connectToServer(const std::string& serverAddressStr) {
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        std::cerr << "Socket creation error!" << std::endl;
        return false;
    }
    
    // Настройка адреса сервера
    memset(&serverAddressInfo, 0, sizeof(serverAddressInfo));
    serverAddressInfo.sin_family = AF_INET;
    serverAddressInfo.sin_port = htons(PORT);
    
    if (inet_pton(AF_INET, serverAddressStr.c_str(), &serverAddressInfo.sin_addr) <= 0) {
        std::cerr << "Invalid address/ Address not supported" << std::endl;
        close(serverSocket);
        serverSocket = -1;
        return false;
    }
    
    // Подключение к серверу
    if (connect(serverSocket, (struct sockaddr *)&serverAddressInfo, sizeof(serverAddressInfo)) < 0) {
        std::cerr << "Connection failed!" << std::endl;
        close(serverSocket);
        serverSocket = -1;
        return false;
    }
    
    std::cout << "Connected to server!" << std::endl;
    return true;
}

std::string ChatClient::sendCommand(const std::string& command) {
    if (serverSocket == -1) {
        std::cerr << "Not connected to server." << std::endl;
        return "";
    }
    
    // Отправляем команду
    std::string fullCommand = command + "\n";
    size_t totalSent = 0;
    while (totalSent < fullCommand.length()) {
        ssize_t sent = send(serverSocket, fullCommand.c_str() + totalSent, 
                           fullCommand.length() - totalSent, 0);
        if (sent < 0) {
            std::cerr << "Send failed: " << strerror(errno) << std::endl;
            return "";
        }
        totalSent += sent;
    }
    
    // Получаем ответ
    char buffer[MESSAGE_LENGTH] = {0};
    std::string response;
    
    // Устанавливаем таймаут для чтения
    fd_set readfds;
    struct timeval tv;
    tv.tv_sec = 3;
    tv.tv_usec = 0;
    
    while (true) {
        FD_ZERO(&readfds);
        FD_SET(serverSocket, &readfds);
        
        int activity = select(serverSocket + 1, &readfds, NULL, NULL, &tv);
        if (activity > 0 && FD_ISSET(serverSocket, &readfds)) {
            ssize_t received = recv(serverSocket, buffer, sizeof(buffer) - 1, 0);
            if (received > 0) {
                buffer[received] = '\0';
                response.append(buffer);
                
                // Если ответ короткий, считаем его полным
                if (received < sizeof(buffer) - 1) {
                    break;
                }
            } else if (received == 0) {
                // Соединение закрыто
                break;
            } else {
                std::cerr << "Receive error: " << strerror(errno) << std::endl;
                break;
            }
        } else {
            // Таймаут или ошибка
            break;
        }
    }
    
    // Убираем лишние символы новой строки в конце
    while (!response.empty() && (response.back() == '\n' || response.back() == '\r')) {
        response.pop_back();
    }
    
    return response;
}

void ChatClient::run() {
    if (serverSocket == -1) {
        std::cerr << "Not connected to server. Call connectToServer() first." << std::endl;
        return;
    }
    
    bool running = true;
    while (running) {
        std::cout << "\n=== MAIN MENU ===\n";
        std::cout << "1 - Register new user\n";
        std::cout << "2 - User login\n";
        std::cout << "3 - View all users\n";
        std::cout << "4 - Exit\n";
        std::cout << "5 - Admin login\n";
        std::cout << "> ";
        
        std::string choice;
        std::getline(std::cin, choice);
        
        if (choice == "1") {
            registerUser();
        } 
        else if (choice == "2") {
            if (login()) {
                userPanel();
            }
        } 
        else if (choice == "3") {
            listUsers();
        } 
        else if (choice == "4") {
            sendCommand("4");
            std::cout << "Goodbye!\n";
            running = false;
        }
        else if (choice == "5") {
            if (LoginAdmin()) {
                AdminPanel();
            }
        } 
        else {
            std::cout << "Invalid choice. Try again.\n";
        }
    }
    
    if (serverSocket != -1) {
        close(serverSocket);
        serverSocket = -1;
    }
}

bool ChatClient::registerUser() {
    std::string login, password, name;
    std::cout << "Enter login: ";
    std::getline(std::cin, login); 
    std::cout << "Enter password: ";
    std::getline(std::cin, password);
    std::cout << "Enter name: ";
    std::getline(std::cin, name);
    
    std::string command = "1 " + login + " " + password + " " + name;
    std::string response = sendCommand(command);
    std::cout << "Server: " << response << std::endl;
    
    return response.find("successfully") != std::string::npos;
}

bool ChatClient::login() {
    std::string login, password;
    std::cout << "Enter login: ";
    std::getline(std::cin, login);
    std::cout << "Enter password: ";
    std::getline(std::cin, password);
    
    std::string command = "2 " + login + " " + password;
    std::string response = sendCommand(command);
    std::cout << "Server: " << response << std::endl;
    
    return response.find("successful") != std::string::npos;
}

bool ChatClient::LoginAdmin() {
    std::string login, password;
    std::cout << "Enter admin login: ";
    std::getline(std::cin, login); 
    std::cout << "Enter admin password: ";
    std::getline(std::cin, password);
    
    std::string command = "5 " + login + " " + password;
    std::string response = sendCommand(command);
    std::cout << "Server: " << response << std::endl;
    
    return response.find("Admin login successful") != std::string::npos || 
           response.find("successful") != std::string::npos;
}

void ChatClient::userPanel() {
    bool running = true;
    while (running) {
        std::cout << "\n=== USER PANEL ===\n";
        std::cout << "1 - Send private message\n";
        std::cout << "2 - Send public message\n";
        std::cout << "3 - View private messages\n";
        std::cout << "4 - View public messages\n";
        std::cout << "5 - View all users\n";
        std::cout << "6 - Logout\n";
        std::cout << "> ";
        
        std::string choice;
        std::getline(std::cin, choice);
        
        if (choice == "1") {
            sendMessage();
        } 
        else if (choice == "2") {
            sendPublicMessage();
        }
        else if (choice == "3") {
            viewPrivateMessages();
        }
        else if (choice == "4") {
            viewPublicMessages();
        }
        else if (choice == "5") {
            listUsers();
        }
        else if (choice == "6") {
            sendCommand("5");  // Выход из пользовательской панели
            std::cout << "Logged out.\n";
            running = false;
        }
        else {
            std::cout << "Invalid choice!\n";
        }
    }
}

void ChatClient::AdminPanel() {
    bool running = true;
    while (running) {
        std::cout << "\n=== ADMIN PANEL ===\n";
        std::cout << "1 - View all users\n";
        std::cout << "2 - View online users\n";
        std::cout << "3 - Ban user\n";
        std::cout << "4 - View public messages\n";
        std::cout << "5 - Unban user\n";
        std::cout << "6 - Delete user\n";
        std::cout << "7 - View banned users\n";
        std::cout << "8 - Exit admin panel\n";
        std::cout << "> ";
        
        std::string choice;
        std::getline(std::cin, choice);
        
        if (choice == "1") {
            listUsers();
        } 
        else if (choice == "2") {
            viewOnlineUsers();
        }
        else if (choice == "3") {
            banUser();
        }
        else if (choice == "4") {
            viewPublicMessages();
        }
        else if (choice == "5") {
            unbanUser();
        }
        else if (choice == "6") {
            deleteUser();
        }
        else if (choice == "7") {
            viewBannedUsers();
        }
        else if (choice == "8") {
            sendCommand("7");  // Выход из админ панели
            std::cout << "Exiting admin panel...\n";
            running = false;
        }
        else {
            std::cout << "Invalid choice!\n";
        }
    }
}

void ChatClient::listUsers() {
    std::string command = "3";
    std::string response = sendCommand(command);
    std::cout << "\n=== ALL USERS ===\n" << response << std::endl;
}

void ChatClient::sendMessage() {
    std::string recipient, message;
    std::cout << "Enter recipient name: ";
    std::getline(std::cin, recipient);
    std::cout << "Enter message: ";
    std::getline(std::cin, message);
    
    std::string command = "1 " + recipient + " " + message;
    std::string response = sendCommand(command);
    std::cout << "Server: " << response << std::endl;
}

void ChatClient::sendPublicMessage() {
    std::string message;
    std::cout << "Enter public message: ";
    std::getline(std::cin, message);
    
    std::string command = "2 " + message;
    std::string response = sendCommand(command);
    std::cout << "Server: " << response << std::endl;
}

void ChatClient::viewPrivateMessages() {
    std::string command = "6";  // Команда для приватных сообщений
    std::string response = sendCommand(command);
    std::cout << "\n=== PRIVATE MESSAGES ===\n" << response << std::endl;
}

void ChatClient::viewPublicMessages() {
    std::string command = "4";
    std::string response = sendCommand(command);
    std::cout << "\n=== PUBLIC MESSAGES ===\n" << response << std::endl;
}

void ChatClient::viewOnlineUsers() {
    std::string command = "admin_2";
    std::string response = sendCommand(command);
    std::cout << "\n=== ONLINE USERS ===\n" << response << std::endl;
}

void ChatClient::viewBannedUsers() {
    std::string command = "admin_6";
    std::string response = sendCommand(command);
    std::cout << "\n=== BANNED USERS ===\n" << response << std::endl;
}

void ChatClient::banUser() {
    std::string login;
    std::cout << "Enter login to ban: ";
    std::getline(std::cin, login);
    
    std::string command = "admin_3 " + login;
    std::string response = sendCommand(command);
    std::cout << "Server: " << response << std::endl;
}

void ChatClient::unbanUser() {
    std::string login;
    std::cout << "Enter login to unban: ";
    std::getline(std::cin, login);
    
    std::string command = "admin_4 " + login;
    std::string response = sendCommand(command);
    std::cout << "Server: " << response << std::endl;
}

void ChatClient::deleteUser() {
    std::string login;
    std::cout << "Enter login to delete: ";
    std::getline(std::cin, login);
    
    std::string command = "admin_5 " + login;
    std::string response = sendCommand(command);
    std::cout << "Server: " << response << std::endl;
}