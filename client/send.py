from socket import *
import numpy as np
from enum import Enum
import client.proto.quotes_pb2 as quotes_pb2
from datetime import datetime

class Send:
    def __init__(self, clientSocket, typeRequest, data={}):
        self.client_socket = clientSocket
        self.message = quotes_pb2.Packet()
        self.message.source = quotes_pb2.CLIENT
        self.message.type = typeRequest
        self.data = data
        self._on_incoming_packet()

    def _on_incoming_packet(self):
        for name in self.data:
            asset = self.message.assets.add()
            asset.name = name
            for time, value in self.data[name].items():
                history_point = asset.history.add()
                history_point.time = int(float(time)) * 10**6
                history_point.value = int(float(value)) * 10**6
        self._send_to_server()

    def _send_to_server(self):
        #print(self.message)
        self.client_socket.sendall(self.message.SerializeToString())
