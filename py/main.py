import time
import zmq
import base64

context = zmq.Context()
socket = context.socket(zmq.REQ)
socket.bind("tcp://127.0.0.1:5555")

image = open("image.jpg", 'rb')
bytes = bytearray(image.read())
strng = base64.b64encode(bytes)

for request in range(1):
    
    # print(f"Sending request {strng} ...")
    socket.send(strng)

    #  Get the reply.
    message = socket.recv()
    print(f"Received reply {request} [ {message} ]")

image.close()
    