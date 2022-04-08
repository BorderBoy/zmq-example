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
    zmq::socket_t sock(ctx, ZMQ_REQ);
    //sock.bind("inproc://test");
    sock.connect("tcp://localhost:5555");
    
    
    sock.send(zmq::str_buffer("Hello, world"), zmq::send_flags::dontwait);
    
    // while(true){
    //     zmq::message_t reply;
    //     sock.recv (&reply, 0);
    //     printf ("Received Word %d bytes: \"%s\"\n", reply.size(), reply.data());
    // }

}
