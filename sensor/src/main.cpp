#include <udp_server.h>
#include <iostream>

int main(int argc, char **argv)
{
    (void)argc; // unused
    (void)argv;
    char buffer[512];
    common::UDPServer server(9000);
    server.getSocket().setSendTimeout(2000);
    std::cout << "UDP server started\n";
    int retval = server.receive(buffer, 511);
    if(retval < 0)
    {
        std::cout << "error, no data received\n";
        return -1;
    }
    std::cout << "Received: " << buffer << "\n";
    if(server.send(buffer, retval) > 0)
        std::cout << "Sent answer\n";
    else
        std::cout << "Failed to send answer\n";
    return 0;
}

