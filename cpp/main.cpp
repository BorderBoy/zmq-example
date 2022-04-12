#include <string>
#include <chrono>
#include <thread>
#include <iostream>

#include <zmq.hpp>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

int main() 
{
    zmq::context_t ctx;
    zmq::socket_t sock(ctx, ZMQ_REP);
    //sock.bind("inproc://test");
    sock.connect("tcp://localhost:5555");
    
    
    while (true) {
        zmq::message_t request;

        //  Wait for next request from client
        sock.recv (request, zmq::recv_flags::none);
        std::string rpl = std::string(static_cast<char*>(request.data()), request.size());
        std::cout << rpl << std::endl;

        //  Do some 'work'
        sleep(1);

        //  Send reply back to client
        zmq::message_t reply (5);
        memcpy (reply.data (), "World", 5);
        sock.send (reply, zmq::send_flags::none);
    }
    return 0;

}
