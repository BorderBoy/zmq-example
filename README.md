# zmq-example
Small working example of communication between python and c++ code. The python client reads images from files and encodes them into base64. These are then sent to OpenFace. OpenFace extracts the action units (AU) and sends them back to the python client.

##  Python
The python code can be found in `py/`. When the python client is executed it sends the first image, waits for the reply and sends the next one. This is done with all 6 images. Start the OpenFace extractor first, wait until the models are loaded and then start the python client.

## OpenFace
The code can be found in `open-face-extractor/`. There also a binary can be found. To build the code the contents of `open-face-extractor/` must be placed into the `exe/` folder of [OpenFace](https://github.com/TadasBaltrusaitis/OpenFace). To install OpenFace follow the instructions in the [wiki](https://github.com/TadasBaltrusaitis/OpenFace/wiki).

When the binary is executed it waits for an image and replies. Then it waits for another image.

**Used Libraries:**
- [cppzmq](https://github.com/zeromq/cppzmq)
- [pyzmq](https://github.com/zeromq/pyzmq)
