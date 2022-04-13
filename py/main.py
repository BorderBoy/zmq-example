import time
import zmq
import base64
import json

context = zmq.Context()
socket = context.socket(zmq.REQ)
socket.bind("tcp://127.0.0.1:5555")



for request in range(6):
    image = open("sample" + str(request) + ".jpg", 'rb')
    bytes = bytearray(image.read())
    strng = base64.b64encode(bytes)

    # print(f"Sending request {strng} ...")
    socket.send(strng)

    #  Get the reply.
    message = socket.recv()
    # print(f"Received reply {request} [ {message} ]")

    data = json.loads(message)

    print(data)

image.close()
    