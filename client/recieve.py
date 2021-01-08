from socket import *
import sys
from client.proto import quotes_pb2
from enum import Enum
from client.controller import Controller

MESSAGE_LENGTH = 1024

class MessageType(Enum):
    ASSET = 1
    HISTORY = 2
    ALL = 3

class Recieve:
    def __init__(self, ip, port):
        self.client_socket = socket(AF_INET, SOCK_STREAM, 0)
        self.client_socket.setsockopt(SOL_SOCKET, SO_REUSEADDR, 1)

        connected = False
        while not connected:
            try:
                self.client_socket.connect((ip, port))
                connected = True
            except Exception as e:
                pass #Try again

        self.type_request = quotes_pb2.GET
        self.message_request = MessageType.ASSET

        controller = Controller(clientSocket=self.client_socket)
        is_server_on = True
        while is_server_on:
            controller.console_enter()
            is_server_on = self._recieve_message(controller.data)

    def _recieve_message(self, data={}):
        receive_uncompleted = 0
        full_message = bytearray()
        message = quotes_pb2.Packet()
        while receive_uncompleted == 0:
            message_recieved = self.client_socket.recv(MESSAGE_LENGTH)

            if not len(message_recieved):
                print('Connection closed by the server')
                return False
                #sys.exit()
            full_message.extend(message_recieved)

            receive_uncompleted = message.ParseFromString(full_message)

        self.checkServerResponse(data, full_message, self.message_request)
        return True

    def checkServerResponse(self, data, message, messageRequest):
        if not (message.type == quotes_pb2.Type.GET):
            self._recieve_add_delete(data=data, message=message)
        else:
            self._recieve_get(message=message, messageRequest=messageRequest)

    def _recieve_add_delete(self, data, message):
            data_recieved = {}
            for asset in message.assets:
                data_recieved[asset.name] = {}
                points_number = 0
                for history_point in asset.history:
                    print(history_point)
                    points_number += 1
                    data_recieved[asset.name][history_point.time] = history_point.value
                if (points_number == 0):
                    data_recieved[asset.name][0] = 0

            if (data == data_recieved):
                print("All good")
            else:
                if (data.keys() != data_recieved.keys()):
                    print("Wrong asset")
                else:
                    print("Wrong history")

    def _recieve_get(self, message, messageRequest):
        if (messageRequest == MessageType.ALL):
            for asset in message.assets:
                print(asset.name)
        elif (messageRequest == MessageType.HISTORY):
            for asset in message.assets:
                print(asset.name)
                for history_point in asset.history:
                    print("time: ", history_point.time,
                            " value: ", history_point.value)
        else:
            for asset in message.assets:
                print(asset.name)
                points = []
                for history_point in asset.history:
                    points.append(history_point.value)
                print("absolute: ", points[1]-points[0], " relative: ", (points[1]/points[0]*100)-100)