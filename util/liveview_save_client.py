#!/usr/bin/env python3

import json
import zlib
from PyQt5.QtCore import *
from PyQt5.QtNetwork import *

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
        if self.socket.bytesAvailable() >= 0:
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

def main():
    client = SaveClient("137.79.193.242", "50000")
    res = client.requestSave("Hello.dat", 100)
    if res:
        print("Received a reply!")


if __name__ == '__main__':
    main()
