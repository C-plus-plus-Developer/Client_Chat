#include "Chat.h"

int main() {
    
    ChatClient client;
    if (client.connectToServer("127.0.0.1")) { 
        client.run();
    }

    return 0;
}
