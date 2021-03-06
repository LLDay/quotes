import argparse
from datetime import datetime
import client.proto.quotes_pb2 as quotes_pb2
from client.send import Send

class Controller:
    def __init__(self, clientSocket):
        self.client_socket = clientSocket
        self.type_request = quotes_pb2.GET
        self.action_incrimentation = False
        self.data = {}

    def console_enter(self):
        self.action_incrimentation = False
        parser = argparse.ArgumentParser(description='Client action')
        parser.add_argument('action', choices=['add', 'get', 'delete', 'inc'])
        parser.add_argument('names', metavar='Name', type=str, nargs='*', help='name of the asssets')
        parser.add_argument('-p', '--points', metavar='Int', type=str, nargs='*', help='points for exact asset')
        parser.add_argument('-f', '--timefrom', metavar='From', type=str, nargs=1, help='time from')
        parser.add_argument('-t', '--timeto', metavar='To', type=str, nargs=1, help='time to')

        right_form = False
        while not right_form:
            input_data = input("Enter action: add, get, delete or inc\n")
            try:
                args = parser.parse_args(input_data.split())
                if (len(args.names) != 0 or args.points is not None or args.timefrom is not None or args.timeto is not None):
                    right_form = True
            except:
                right_form = False
                print("Your request is wrong! Write another one:\n")


        action = args.action
        if action == "add":
            self.type_request = quotes_pb2.ADD
        elif action == "delete":
            self.type_request = quotes_pb2.DELETE
        elif action == "get":
            self.type_request = quotes_pb2.GET
        elif action == "inc":
            self.type_request = quotes_pb2.GET
            self.action_incrimentation = True

        if args.timefrom is not None or args.timeto is not None:
            return self._retrieve(args, onlyNames=False, time=True)
        elif args.points is not None:
            return self._retrieve(args, onlyNames=False)
        else:
            return self._retrieve(args)

    def _parsing_date(self, time):
        for time_format in ('%Y-%m-%d', '%Y.%m.%d', '%Y/%m/%d', '%Y-%m-%d:%H:%M:%S', '%Y/%m/%d:%H:%M:%S', '%Y.%m.%d:%H:%M:%S'):
            try:
                return datetime.strptime(time, time_format)
            except ValueError:
                pass

    def _retrieve(self, args, onlyNames=True, time=False):
        data_temp = {}
        try:
            if onlyNames:
                for name in args.names:
                    if (args.action != "inc"):
                        data_temp[name] = {}
                    else:
                        data_temp[name] = {}
                        data_temp[name][0] = 2
            else:
                name = args.names
                data_temp[name[0]] = {}
                if not time:
                    points = args.points
                    for time, value in zip(points[0::2], points[1::2]):
                        print(time, value)
                        time_parce = self._parsing_date(time)
                        epoch_time = (time_parce - datetime(1970, 1, 1)).total_seconds()
                        data_temp[name[0]][epoch_time] = value
                elif time:
                    if args.timefrom is not None:
                        time_parce = self._parsing_date(args.timefrom[0])
                        from_epoch_time = (time_parce - datetime(1970, 1, 1)).total_seconds()
                        data_temp[name[0]][from_epoch_time] = 0 #from
                    else:
                        min_time = str(float(0))
                        data_temp[name[0]][min_time] = 0 #from
                    if args.timeto is not None:
                        time_parce = self._parsing_date(args.timeto[0])
                        to_epoch_time = (time_parce - datetime(1970, 1, 1)).total_seconds()
                        data_temp[name[0]][to_epoch_time] = 0 #to
                    else:
                        max_time = str(float(2**64-1))
                        data_temp[name[0]][max_time] = 0 #to
            self.data = data_temp
            self._send_model()
            return True
        except TypeError:
            print('no valid date format found')
            return False

    def _send_model(self):
        Send(self.client_socket, self.type_request, self.data)
