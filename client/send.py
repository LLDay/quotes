from socket import *
import select
import errno
import sys
import time
import client.proto.quotes_pb2
from datetime import datetime
from .recieve import MessageType

class Send:
    def __init__(self, clientSocket, typeRequest, messageRequest, data={}):
        self.client_socket = clientSocket
        self.message = quotes_pb2.Packet()
        self.message.source = quotes_pb2.CLIENT
        self.message_request = messageRequest
        self.asset_data = data
        self._on_incoming_packet(typeRequest)
    
    def _on_incoming_packet(self, typeRequest):
        handler = {quotes_pb2.ADD: self._on_add_message,
                    quotes_pb2.GET: self._on_get_message,
                    quotes_pb2.DELETE: self._on_delete_message
                    }[typeRequest]
        handler()
        #self._send_to_server()

    def _on_add_message(self):
        self._set_type(quotes_pb2.ADD)
        if (self.message_request == MessageType.ASSET):
            self._on_add_asset()
        else:
            self._on_add_history()
            
    def _on_add_asset(self):
        self._add_asset()

    def _on_add_history(self):
        self._add_asset()
    
    def _on_get_message(self):
        self._set_type(quotes_pb2.GET)
        if (self.message_request == MessageType.ALL):
            self._on_get_all()
        elif (self.message_request == MessageType.ASSET):
            self._on_get_asset()
        else:
            self._on_get_history()
    
    def _on_get_all(self):
        self._add_asset(getAll=True)

    def _on_get_asset(self):
        self._add_asset(getAll=True, value=2)

    def _on_get_history(self):
        self._add_asset()

    def _on_delete_message(self):
        self._set_type(quotes_pb2.DELETE)
        self._add_asset()

    def _set_type(self, messageType):
        self.message.type = messageType
    
    def _add_asset(self, getAll: bool=False, value=0):
        if not getAll:
            values_dict = self.asset_data.values()
            for i, asset_name in enumerate(self.asset_data.keys()):
                asset = self.message.assets.add()
                asset.name = asset_name
                values_to_asset = values_dict[i]
                for time_to_asset in values_to_asset:
                    history_point = asset.history.add()
                    history_point.time = time_to_asset
                    history_point.value = values_to_asset[time]
        else:
            asset = self.message.assets.add()
            asset.name = "*"   
            history_point = asset.history.add()
            history_point.time=0
            history_point.value=value   
        self._send_to_server()

    def _send_to_server(self):
        self.client_socket.sendall(self.message.SerializeToString())