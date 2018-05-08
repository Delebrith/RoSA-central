#include <udp_client.h>
#include <iostream>
#include <cstring>

int main(int argc, char **argv)
{
    (void)argc; // unused
    (void)argv;
    char message[] = "hello";
    common::UDPClient client("localhost", "9000");
    client.getSocket().setSendTimeout(5000);
    client.getSocket().setReceiveTimeout(2000);
    std::cout << "UDP client started\n";
    int retval = client.send(message, sizeof message);
    if(retval < 0)
    {
        std::cout << "error, no data sent\n";
        return -1;
    }
    std::cout << "Sent message\n";
    std::memset(message, 0, sizeof message); // clear buffer to see what we will receive
    retval = client.receive(message, sizeof message);
    if(retval < 0)
    {
        std::cout << "error, no data received\n";
        return -2;
    }
    std::cout << "Received: " << message << "\n";
    return 0;
}
