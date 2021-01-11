from socket import *
import sys
import time
import datetime
import client.proto.quotes_pb2 as quotes_pb2
from enum import Enum
from client.controller import Controller

MESSAGE_LENGTH = 1024

class Recieve:
    def __init__(self, ip, port):
        self.client_socket = socket(AF_INET, SOCK_STREAM, 0)
        self.client_socket.setsockopt(SOL_SOCKET, SO_REUSEADDR, 1)

        while True:
            connected = False
            while not connected:
                try:
                    self.client_socket.connect((ip, port))
                    connected = True
                except Exception as e:
                    pass #Try again

            self.type_request = quotes_pb2.GET

            controller = Controller(clientSocket=self.client_socket)
            is_server_on = True

            while is_server_on:
                controller.console_enter()
                is_server_on = self._recieve_message(controller.data, controller.action_incrimentation)

    def _recieve_message(self, data, actionIncrimentation):
        receive_uncompleted = 0
        full_message = bytearray()
        message = quotes_pb2.Packet()
        while receive_uncompleted == 0:

            #try catch
            message_recieved = self.client_socket.recv(MESSAGE_LENGTH)
            if not len(message_recieved):
                print('Connection closed by the server')
                return False
                #sys.exit()
            full_message.extend(message_recieved)
            receive_uncompleted = message.ParseFromString(full_message)

        self.checkServerResponse(data, message, actionIncrimentation)
        return True

    def checkServerResponse(self, data, message, actionIncrimentation):
        if message.type != quotes_pb2.Type.GET:
            self._recieve_add_delete(data=data, message=message, messageType=message.type)
        else:
            self._recieve_get(message, actionIncrimentation)

    def _recieve_add_delete(self, data, message, messageType):
        data_recieved = {}
        for asset in message.assets:
            data_recieved[asset.name] = {}
            for history_point in asset.history:
                data_recieved[asset.name][history_point.time / 10**6] = history_point.value / 10**6

        #add delete ok if 0
        if messageType == quotes_pb2.Type.DELETE and not data_recieved:
            print("All assets were deleted")
        elif data == data_recieved:
            print("All good")
        #else:
        #    if data.keys() != data_recieved.keys():
        #        print("Wrong asset")
        #    else:
        #        print("Wrong history")

        #print(data_recieved)

    def _recieve_get(self, message, actionIncrimentation):
        for asset in message.assets:
            print(asset.name)
            if actionIncrimentation:
                points = []
                for history_point in asset.history:
                    points.append(history_point.value / 10**6)
                if len(points) == 2:
                    print("absolute: ", points[1]-points[0], " relative: ", (points[1]/points[0]*100)-100)
                else:
                    print("not enough points")
            else:
                for history_point in asset.history:
                    print("time: ", datetime.datetime.fromtimestamp(history_point.time / (10**6)), " value: ", history_point.value / 10**6)
