#!/usr/bin/env python3

# Standard Modules
import argparse
import json
import signal
import sys
from time import sleep
import zlib

# Qt Modules
from PyQt5.QtCore import *
from PyQt5.QtNetwork import *

def signal_handler(sig, frame):
    print("Disconnecting...")
    sys.exit(0)

signal.signal(signal.SIGINT, signal_handler)

class SaveClient(QObject):
    def __init__(self, address, port, parent=None):
        super(SaveClient, self).__init__(parent)

        self.ipAddress = address
        self.portNumber = int(port)

        # create a socket to send and receive quests from the LiveView server.
        self.socket = QTcpSocket(self)
        self.socket.error.connect(self.printError)
        self.socket.connectToHost(self.ipAddress, self.portNumber)
        self.socket.waitForConnected()

    def printError(self, socket_error):
        errors = {
            QTcpSocket.HostNotFoundError:
                "The requested host name could not be found."
                "Please check the host name and port settings and try again.",

            QTcpSocket.ConnectionRefusedError:
                "The connection was refused by the peer. Make sure that "
                "LiveView is running, and check that the host name and port "
                "settings are correct."

        }

        message = errors.get(socket_error,
            "The following error occurred: %s" % self.socket.errorString())
        if message:
            print(message)
            sys.exit(1)

    def requestSave(self, file_name, n_frames, n_avgs=1):
        requestDoc = json.dumps(
            {
                u"requestType": u"Save",
                u"fileName":    u"%s" % file_name,
                u"numFrames":   int(n_frames),
                u"numAvgs":     int(n_avgs)
            }).encode("utf-8")
        reqBlock = qCompress(requestDoc)
        self.socket.write(reqBlock)
        self.socket.waitForReadyRead(2000) # Wait for 2 secs.
        if self.socket.bytesAvailable() > 0:
            respBlock = self.socket.readAll()
            response = qUncompress(respBlock)
            respDoc = json.loads(bytearray(response))
            if "status" in respDoc.keys():
                status = int(respDoc["status"])
                if status != 200:
                    print("Received an error: %s" % respDoc["message"])
                    return False
                print("Sent command to save %d frames to the file %s on %s" % (n_frames, file_name, self.ipAddress))
                return True
            else:
                print("Unexpected response from server: %s" % response)
                return False
        else:
            return False


def main(ipAddress, portNumber, frames=100):
    running = True
    client = SaveClient(ipAddress, portNumber)
    print("Requesting to save frames every 20 seconds indefinitely. Press Ctrl-C to exit.")
    while running == True:
        print("Sending a request to save frames...")
        res = client.requestSave("./Hello.dat", frames)
        if res:
            print("Received a reply!")
        sleep(20)


if __name__ == '__main__':
    parser = argparse.ArgumentParser(
        formatter_class=argparse.RawDescriptionHelpFormatter,
        description='''
        This is an example script for testing the network-based request
        feature of LiveView. It implements basic TCP communication with a
        LiveView server. The client will request to save 100 frames from
        the server to the local directory every 20 seconds indefinitely. 
        ''')
    parser.add_argument("ip", help="IP address of the server to connect to.")
    parser.add_argument("port", type=int, help="Port number of the server.")
    args = parser.parse_args()
    main(args.ip, str(args.port))
