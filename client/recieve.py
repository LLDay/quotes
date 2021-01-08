from socket import *
import select
import errno
import sys
import time
import client.proto.quotes_pb2 
from datetime import datetime
from enum import Enum
from .send import *

MESSAGE_LENGTH = 1024

class MessageType(Enum):
    ASSET = 1
    HISTORY = 2
    ALL = 3

class Recieve:
    def __init__(self, ip, port):
        self.client_socket = socket(AF_INET, SOCK_STREAM, 0)
        self.client_socket.setsockopt(SOL_SOCKET, SO_REUSEADDR, 1)
        self.client_socket.connect((ip, port))
        self.type_request = quotes_pb2.GET
        self.message_request = MessageType.ASSET

        while True:
            input_data = input("Enter action:add, get or delete")
            handler = {
                "add": self._on_add,
                "get": self._on_get,
                "delete": self._on_delete
            }[input_data]
            handler()
    
    def _on_add(self):
        self.type_request = quotes_pb2.ADD
        while True:
            input_data = input("Enter action: asset or history")
            handler = {
                "asset": self._on_add_asset,
                "history": self._on_add_history
            }[input_data]
            handler()
    
    def _on_add_asset(self):
        self.message_request = MessageType.ASSET
        self._input_names(isHistory=False)
    
    def _on_add_history(self):
        self.message_request = MessageType.HISTORY
        self._input_names()

    def _on_get(self):
        self.type_request = quotes_pb2.GET
        while True:
            input_data = input("Enter action: all, asset or history")
            handler = {
                "all": self._on_get_all,
                "asset": self._on_get_asset,
                "history": self._on_get_history
            }[input_data]
            handler()
    
    def _on_get_all(self):
        self.message_request = MessageType.ALL
        self._send_model()

    def _on_get_asset(self):
        self.message_request = MessageType.ASSET
        self._send_model()

    def _on_get_history(self):
        self.message_request = MessageType.HISTORY
        self._input_names()

    def _on_delete(self):
        self.type_request = quotes_pb2.DELETE
        self._input_names(isHistory=False)

    def _input_names(self, isHistory: bool=True):
        data = {}
        while True:
            input_data = input("Enter name of asset: name or stop")
            handler = {
                "stop": self._send_model
            }[input_data]
            handler(data)
            if isHistory:
                check_exit = True
                while check_exit:
                    history = input("Enter point for asset: time_space_value or stop")
                    if (history == "stop"):
                        check_exit = False
                    else:
                        history_point = history.split()
                        if input_data in data:
                            data[input_data][history_point[0]] = history_point[1]
                        else:
                            data[input_data] = {history_point[0], history_point[1]}
            else:
                data[input_data] = {0: 0}

    def _send_model(self, data={}):
        Send(self.client_socket, self.type_request, self.message_request, data)
        receive_uncompleted = True
        data = bytearray()
        message = quotes_pb2.Packet()
        while receive_uncompleted:
            message_recieved = self.client_socket.recv(MESSAGE_LENGTH)

            if not len(message_recieved):
                print('Connection closed by the server')
                sys.exit()

            data.append(message_recieved)  
            receive_uncompleted = message.ParseFromString(data)

        self._checkOkServerResponse(data, message, self.message_request)
        
    def _checkOkServerResponse(self, data, message, messageRequest):
        if not (message.type == quotes_pb2.MessageType.GET):
            data_recieved = {}
            for asset in message.assets:
                data_recieved[asset.name] = {}
                points_number = 0
                for history_point in asset.history:
                    points_number += 1
                    data_recieved[asset.name][history_point[0]] = history_point[1]
                if (points_number == 0):
                    data_recieved[asset.name][0] = 0
            
            if (data == data_recieved):
                print("All good")
            else:
                if (data.keys() != data_recieved.keys()):
                    print("Wrong asset")
                else:
                    print("Wrong history")
        else:
            if (messageRequest == MessageType.ALL):
                for asset in message.assets:
                    print(asset.name)
            elif (messageRequest == MessageType.HISTORY):
                for asset in message.assets:
                    print(asset.name)
                    for history_point in asset.history:
                        print("time: ", history_point[0], " value: ", history_point[1])
            else:
                for asset in message.assets:
                    print(asset.name)
                    points = []
                    for history_point in asset.history:
                        points.append(history_point[1])
                    print("absolute: ", points.get(1)-points.get(0), " relative: ", (points.get(1)/points.get(0)*100)-100)
