#include <udp_server.h>
#include <iostream>

int main(int argc, char **argv)
{
    try
    {
        if(argc != 2)
        {
            std::cout << "Usage: " << argv[0] << " <port>\n";
            return -1;
        }
        char buffer[512];
        common::UDPServer server(std::atoi(argv[1]));
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
    catch(const std::exception &ex)
    {
        std::cerr << ex.what() << "\n";
        return -1;
    }
}

