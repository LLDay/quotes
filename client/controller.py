import argparse
from enum import Enum
import client.proto.quotes_pb2 as quotes_pb2
from client.send import Send

class Controller:
    def __init__(self, clientSocket):
        self.client_socket = clientSocket
        self.type_request = quotes_pb2.GET
        self.action_incrimentation = False
        self.data = {}

    def _default(*args):
        pass

    def console_enter(self):
        parser = argparse.ArgumentParser(description='Client action')
        parser.add_argument('action', choices=['add', 'get', 'delete', 'incrimentation'])
        parser.add_argument('names', metavar='Name', type=str, nargs='*', help='name of the asssets')
        parser.add_argument('-p', '--points', metavar='Int', type=int, nargs='*', help='points for exact asset')
        parser.add_argument('-t', '--time', metavar='Int', type=int, nargs=2, help='time from - time to')

        input_data = input("Enter action:add, get or delete\n")
        args = parser.parse_args(input_data.split())
        print(args.names)
        print(args.points)

        action = args.action
        if action == "add":
            self.type_request = quotes_pb2.ADD
        elif action == "delete":
            self.type_request = quotes_pb2.DELETE
        elif action == "get":
            self.type_request = quotes_pb2.GET
        elif action == "incrimentation":
            self.type_request = quotes_pb2.GET
            self.action_incrimentation = True

        if (args.time is not None):
            self._retrieve(args, onlyNames=False, time=True)
        elif (args.points is not None):
            self._retrieve(args, onlyNames=False)
        else:
            self._retrieve(args)


    def _retrieve(self, args, onlyNames=True, time=False):
        data_temp = {}

        if onlyNames:
            for name in args.names:
                if (args.action == "incrimentation"):
                    data_temp[name] = {}
                else:
                    data_temp[name][0] = 2
        else:
            name = args.names
            data_temp[name[0]] = {}
            if not time:
                points = args.points
                for time, value in zip(points[0::2], points[1::2]):
                    data_temp[name[0]][time] = value #add int()
            elif time:
                data_temp[name[0]][args.time[0]] = 0 #from
                data_temp[name[0]][args.time[1]] = 0 #to

        self.data = data_temp
        self._send_model()

    def _send_model(self):
        print("before sending")
        Send(self.client_socket, self.type_request, self.data)
