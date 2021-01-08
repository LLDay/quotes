from enum import Enum
from client.proto import quotes_pb2
from client.send import Send
#from client.recieve import MessageType

class MessageType(Enum):
    ASSET = 1
    HISTORY = 2
    ALL = 3

class Controller:
    def __init__(self, clientSocket):
        self.client_socket = clientSocket
        self.type_request = quotes_pb2.GET
        self.message_request = MessageType.ASSET
        self.data = {}

    def _default(*args):
        pass

    def console_enter(self):
        input_data = input("Enter action:add, get or delete\n")
        handler = {
            "add": self._on_add,
            "get": self._on_get,
            "delete": self._on_delete
        }.get(input_data, self._default)
        handler()

    def _on_add(self):
        self.type_request = quotes_pb2.ADD
        input_data = input("Enter action: asset or history\n")
        handler = {
            "asset": self._on_add_asset,
            "history": self._on_add_history
        }.get(input_data, self._default)
        handler()

    def _on_add_asset(self):
        self.message_request = MessageType.ASSET
        self._input_names(isHistory=False)

    def _on_add_history(self):
        self.message_request = MessageType.HISTORY
        self._input_names()

    def _on_get(self):
        self.type_request = quotes_pb2.GET
        input_data = input("Enter action: all, asset, history\n")
        handler = {
            "all": self._on_get_all,
            "asset": self._on_get_asset,
            "history": self._on_get_history
        }.get(input_data, self._default)
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

    def _input_names(self, isHistory: bool = True):
        bool_unstop = True
        while bool_unstop:
            input_data = input("Enter name of asset: name or stop\n")
            if (input_data == "stop"):
                bool_unstop = False
                self._send_model()

            if isHistory:
                check_exit = True
                while check_exit:
                    history = input(
                        "Enter point for asset: time_space_value or stop\n")
                    if (history == "stop"):
                        check_exit = False
                    else:
                        history_point = history.split()
                        if input_data in self.data:
                            self.data[input_data][history_point[0]] = history_point[1]
                        else:
                            self.data[input_data] = {
                                history_point[0], history_point[1]}
            else:
                self.data[input_data] = {0: 0}

    def _send_model(self):
        Send(self.client_socket, self.type_request, self.message_request, self.data)
