import subprocess
import sys
import PySimpleGUI as sg
from enum import Enum
import argparse
from threading import Thread, Condition
import socket
from zeep import Client

#python3 ./client.py -s <IP> -p <PUERTO>
#python3 ./client.py -s localhost -p 8888
class client :

    # ******************** TYPES *********************
    # *
    # * @brief Return codes for the protocol methods
    class RC(Enum) :
        OK = 0
        ERROR = 1
        USER_ERROR = 2

    # ****************** ATTRIBUTES ******************
    _server = None
    _port = -1
    _quit = 0
    _username = None
    _alias = None
    _date = None
    _listen_socket = None

    # ******************** METHODS *******************
    """Method thar reads a number from a socket"""
    @staticmethod
    def readNumber(sock):
        a = ''
        while True:
            msg = sock.recv(1)
            if (msg == b'\0'):
                break
            a += msg.decode()
        return(int(a,10))
    
    """Method thar reads a message from a socket"""
    @staticmethod
    def readMessage(sock):
        a = ''
        while True:
            msg = sock.recv(1)
            if (msg == b'\0'):
                break
            a += msg.decode()
        return a
    # *
    # * @param user - User name to register in the system
    # *
    # * @return OK if successful
    # * @return USER_ERROR if the user is already registered
    # * @return ERROR if another error occurred
    @staticmethod
    def  register(user, window):
        try:
            sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            print('Socket created in register client\n')
        except socket.error:
            print('Failed to create socket in register client\n')
            return client.RC.ERROR
        try:
            sock.connect((client._server, client._port))
            print('Socket connected in register client\n')
        except socket.error:
            print('Failed to connect to server in register client\n')
            return client.RC.ERROR
        
        sock.sendall(b'REGISTER\0')
        sock.sendall(str(user).encode() + b'\0')
        sock.sendall(str(client._alias).encode() + b'\0')
        sock.sendall(str(client._date).encode() + b'\0')
        
        res = client.readNumber(sock)
        try:
            sock.shutdown(socket.SHUT_RDWR)
            sock.close()
            print('Socket closed in register client\n')
        except socket.error:
            print('Failed to close socket in register\n')

        if res == 0:
            window['_SERVER_'].print("s> REGISTER OK")
            return client.RC.OK
        elif res == 1:
            window['_SERVER_'].print("s> REGISTER IN USE")
            return client.RC.USER_ERROR

        window['_SERVER_'].print("s> REGISTER FAIL")
        return client.RC.ERROR


    # *
    # 	 * @param user - User name to unregister from the system
    # 	 *
    # 	 * @return OK if successful
    # 	 * @return USER_ERROR if the user does not exist
    # 	 * @return ERROR if another error occurred
    @staticmethod
    def  unregister(user, window):
        try:
            sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            print('Socket created in unregister client\n')
        except socket.error:
            print('Failed to create socket in unregister client\n')
            return client.RC.ERROR
        try:
            sock.connect((client._server, client._port))
            print('Socket connected in unregister client\n')
        except socket.error:
            print('Failed to connect to server in unregister client\n')
            return client.RC.ERROR
        
        sock.sendall(b'UNREGISTER\0')
        sock.sendall(str(user).encode() + b'\0')
        
        res = client.readNumber(sock)
        try:
            sock.shutdown(socket.SHUT_RDWR)
            sock.close()
            print('Socket closed in unregister client\n')
        except socket.error:
            print('Failed to close socket in unregister\n')

        if res == 0:
            window['_SERVER_'].print("s> UNREGISTER OK")
            return client.RC.OK
        elif res == 1:
            window['_SERVER_'].print("s> USER DOES NOT EXIST")
            return client.RC.USER_ERROR
        
        window['_SERVER_'].print("s> UNREGISTER FAIL")
        return client.RC.ERROR

    """Method called from thread to listen messages while connected"""
    @staticmethod
    def  listening_messages(listen_socket, window):
        try:
            listen_socket.listen(1)
            print('Listening messages in socket client\n')
        except socket.error:
            print('Failed to listen messages in socket client\n')
            return client.RC.ERROR

        # While can connect to socket, wait connections. When socket error or socket closed, exit thread
        while True:
            try:
                client_socket, address = listen_socket.accept()
                oper = client.readMessage(client_socket)
                if oper == "SEND_MESSAGE_ACK":
                    id = client.readNumber(client_socket)
                    window['_SERVER_'].print(f"s> SEND MESSAGE {id} OK")
                elif oper == "SEND_MESSAGE":
                    sender = client.readMessage(client_socket)
                    identif = client.readNumber(client_socket)
                    message = client.readMessage(client_socket)
                    window['_SERVER_'].print(f"s> MESSAGE {identif} FROM {sender}\n{message}\nEND")

            except socket.error:
                print('Closing thread of socket client\n')
                return client.RC.ERROR


    # *
    # * @param user - User name to connect to the system
    # *
    # * @return OK if successful
    # * @return USER_ERROR if the user does not exist or if it is already connected
    # * @return ERROR if another error occurred
    @staticmethod
    def  connect(user, window):
        try:
            sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            print('Socket to server created in connect client\n')
        except socket.error:
            print('Failed to create socket to server in connect client\n')
            return client.RC.ERROR
        try:
            sock.connect((client._server, client._port))
            print('Socket to server connected in connect client\n')
        except socket.error:
            print('Failed to connect to server in connect client\n')
            return client.RC.ERROR
        try:
            client._listen_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            print('Socket client for listen created in connect client\n')
            client._listen_socket.bind(('0.0.0.0', 0))
            client_port = client._listen_socket.getsockname()[1]
        except socket.error:
            print('Failed to create socket client for listen in connect client\n')
            return client.RC.ERROR

        
        sock.sendall(b'CONNECT\0')
        sock.sendall(str(user).encode() + b'\0')
        sock.sendall(str(client_port).encode() + b'\0')
        
        res = client.readNumber(sock)
        try:
            sock.shutdown(socket.SHUT_RDWR)
            sock.close()
            print('Socket to server closed in connect client\n')
        except socket.error:
            print('Failed to close socket to server in connect\n')

        if res == 0:
            window['_SERVER_'].print("s> CONNECT OK")
            # If connect ok, start thread to listen messages
            listen_thread = Thread(target=client.listening_messages, daemon=True, args=(client._listen_socket, window))
            listen_thread.start()
            print('Thread for listening messages started in connect client\n')
            return client.RC.OK
        elif res == 1:
            window['_SERVER_'].print("s> CONNECT FAIL, USER DOES NOT EXIST")
            return client.RC.USER_ERROR
        elif res == 2:
            window['_SERVER_'].print("s> USER ALREADY CONNECTED")
            return client.RC.USER_ERROR

        window['_SERVER_'].print("s> CONNECT FAIL")
        return client.RC.ERROR


    # *
    # * @param user - User name to disconnect from the system
    # *
    # * @return OK if successful
    # * @return USER_ERROR if the user does not exist
    # * @return ERROR if another error occurred
    @staticmethod
    def  disconnect(user, window):
        try:
            sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            print('Socket created in disconnect client\n')
        except socket.error:
            print('Failed to create socket in disconnect client\n')
            return client.RC.ERROR
        try:
            sock.connect((client._server, client._port))
            print('Socket connected in disconnect client\n')
        except socket.error:
            print('Failed to connect in disconnect client\n')
            return client.RC.ERROR
        
        sock.sendall(b'DISCONNECT\0')
        sock.sendall(str(user).encode() + b'\0')
        
        res = client.readNumber(sock)
        try:
            sock.shutdown(socket.SHUT_RDWR)
            sock.close()
            print('Socket to server closed in disconnect client\n')
        except socket.error:
            print('Failed to close socket to server in disconnect\n')

        if res == 0:
            window['_SERVER_'].print("s> DISCONNECT OK")
            # If disconnect ok, close listening socket
            try:
                client._listen_socket.shutdown(socket.SHUT_RDWR)
                client._listen_socket.close()
                print('Listening socket closed in disconnect client\n')
            except socket.error:
                print('Failed to close listening socket in disconnect\n')
            return client.RC.OK
        elif res == 1:
            window['_SERVER_'].print("s> DISCONNECT FAIL / USER DOES NOT EXIST")
            return client.RC.USER_ERROR
        elif res == 2:
            window['_SERVER_'].print("s> DISCONNECT FAIL / USER NOT CONNECTED")
            return client.RC.USER_ERROR

        window['_SERVER_'].print("s> DISCONNECT FAIL")
        return client.RC.ERROR

    # *
    # * @param user    - Receiver user name
    # * @param message - Message to be sent
    # *
    # * @return OK if the server had successfully delivered the message
    # * @return USER_ERROR if the user is not connected (the message is queued for delivery)
    # * @return ERROR the user does not exist or another error occurred
    @staticmethod
    def  send(user, message, window):
        try:
            sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            print('Socket created in send client\n')
        except socket.error:
            print('Failed to create socket in send client\n')
            return client.RC.ERROR
        try:
            sock.connect((client._server, client._port))
            print('Socket connected in send client\n')
        except socket.error:
            print('Failed to connect to server in send client\n')
            return client.RC.ERROR
        
        # Before sending message, call web service to convert text
        web_service = Client('http://localhost:8000/convert_text?wsdl')
        converted_message = web_service.service.convert_text(message)
        print(f"Message: {message}\nConverted to: {converted_message}\n")
        
        sock.sendall(b'SEND\0')
        sock.sendall(str(client._alias).encode() + b'\0')
        sock.sendall(str(user).encode() + b'\0')
        sock.sendall(str(converted_message).encode() + b'\0')
        
        res = client.readNumber(sock)

        if res == 0:
            identificator = client.readNumber(sock)
            window['_SERVER_'].print(f"s> SEND OK - MESSAGE {identificator}")
            try:
                sock.shutdown(socket.SHUT_RDWR)
                sock.close()
                print('Socket closed in send client\n')
            except socket.error:
                print('Failed to close socket in send\n')
            return client.RC.OK
        elif res == 1:
            window['_SERVER_'].print("s> SEND FAIL / USER DOES NOT EXIST")
            try:
                sock.shutdown(socket.SHUT_RDWR)
                sock.close()
                print('Socket closed in send client\n')
            except socket.error:
                print('Failed to close socket in send\n')
            return client.RC.USER_ERROR
        
        try:
            sock.shutdown(socket.SHUT_RDWR)
            sock.close()
            print('Socket closed in send client\n')
        except socket.error:
            print('Failed to close socket in send\n')
        window['_SERVER_'].print("s> SEND FAIL")
        return client.RC.ERROR

    # *
    # * @param user    - Receiver user name
    # * @param message - Message to be sent
    # * @param file    - file  to be sent

    # *
    # * @return OK if the server had successfully delivered the message
    # * @return USER_ERROR if the user is not connected (the message is queued for delivery)
    # * @return ERROR the user does not exist or another error occurred
    @staticmethod
    def  sendAttach(user, message, file, window):
        window['_SERVER_'].print("s> SENDATTACH MESSAGE OK")
        print("SEND ATTACH " + user + " " + message + " " + file)
        #  Write your code here
        return client.RC.ERROR

    @staticmethod
    def  connectedUsers(window):
        try:
            sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            print('Socket created in connectedusers client\n')
        except socket.error:
            print('Failed to create socket in connectedusers client\n')
            return client.RC.ERROR
        try:
            sock.connect((client._server, client._port))
            print('Socket connected in connectedusers client\n')
        except socket.error:
            print('Failed to connect to server in connectedusers client\n')
            return client.RC.ERROR
        
        sock.sendall(b'CONNECTEDUSERS\0')
        sock.sendall(str(client._alias).encode() + b'\0')
        
        res = client.readNumber(sock)

        if res == 0:
            number = client.readNumber(sock)
            clients = []
            while len(clients) < number:
                clients.append(client.readMessage(sock))
            clients_str = ", ".join(clients)
            window['_SERVER_'].print(f"s> CONNECTED USERS ({number} users connected) OK - {clients_str}")
            try:
                sock.shutdown(socket.SHUT_RDWR)
                sock.close()
                print('Socket closed in connectedusers client\n')
            except socket.error:
                print('Failed to close socket in connectedusers\n')
            return client.RC.OK
        elif res == 1:
            window['_SERVER_'].print("s> CONNECTED USERS FAIL / USER IS NOT CONNECTED")
            try:
                sock.shutdown(socket.SHUT_RDWR)
                sock.close()
                print('Socket closed in connectedusers client\n')
            except socket.error:
                print('Failed to close socket in connectedusers\n')
            return client.RC.USER_ERROR
        
        try:
            sock.shutdown(socket.SHUT_RDWR)
            sock.close()
            print('Socket closed in connectedusers client\n')
        except socket.error:
            print('Failed to close socket in connectedusers\n')
        window['_SERVER_'].print("s> CONNECTED USERS FAIL")
        return client.RC.ERROR

    @staticmethod
    def window_register():
        layout_register = [[sg.Text('Ful Name:'),sg.Input('Text',key='_REGISTERNAME_', do_not_clear=True, expand_x=True)],
                            [sg.Text('Alias:'),sg.Input('Text',key='_REGISTERALIAS_', do_not_clear=True, expand_x=True)],
                            [sg.Text('Date of birth:'),sg.Input('',key='_REGISTERDATE_', do_not_clear=True, expand_x=True, disabled=True, use_readonly_for_disable=False),
                            sg.CalendarButton("Select Date",close_when_date_chosen=True, target="_REGISTERDATE_", format='%d-%m-%Y',size=(10,1))],
                            [sg.Button('SUBMIT', button_color=('white', 'blue'))]
                            ]

        layout = [[sg.Column(layout_register, element_justification='center', expand_x=True, expand_y=True)]]

        window = sg.Window("REGISTER USER", layout, modal=True)
        choice = None

        while True:
            event, values = window.read()

            if (event in (sg.WINDOW_CLOSED, "-ESCAPE-")):
                break

            if event == "SUBMIT":
                if(values['_REGISTERNAME_'] == 'Text' or values['_REGISTERNAME_'] == '' or values['_REGISTERALIAS_'] == 'Text' or values['_REGISTERALIAS_'] == '' or values['_REGISTERDATE_'] == ''):
                    sg.Popup('Registration error', title='Please fill in the fields to register.', button_type=5, auto_close=True, auto_close_duration=1)
                    continue

                client._username = values['_REGISTERNAME_']
                client._alias = values['_REGISTERALIAS_']
                client._date = values['_REGISTERDATE_']
                break
        window.Close()


    # *
    # * @brief Prints program usage
    @staticmethod
    def usage() :
        print("Usage: python3 py -s <server> -p <port>")


    # *
    # * @brief Parses program execution arguments
    @staticmethod
    def  parseArguments(argv) :
        parser = argparse.ArgumentParser()
        parser.add_argument('-s', type=str, required=True, help='Server IP')
        parser.add_argument('-p', type=int, required=True, help='Server Port')
        args = parser.parse_args()

        if (args.s is None):
            parser.error("Usage: python3 py -s <server> -p <port>")
            return False

        if ((args.p < 1024) or (args.p > 65535)):
            parser.error("Error: Port must be in the range 1024 <= port <= 65535");
            return False;

        client._server = args.s
        client._port = args.p

        return True


    def main(argv):

        if (not client.parseArguments(argv)):
            client.usage()
            exit()

        lay_col = [[sg.Button('REGISTER',expand_x=True, expand_y=True),
                sg.Button('UNREGISTER',expand_x=True, expand_y=True),
                sg.Button('CONNECT',expand_x=True, expand_y=True),
                sg.Button('DISCONNECT',expand_x=True, expand_y=True),
                sg.Button('CONNECTED USERS',expand_x=True, expand_y=True)],
                [sg.Text('Dest:'),sg.Input('User',key='_INDEST_', do_not_clear=True, expand_x=True),
                sg.Text('Message:'),sg.Input('Text',key='_IN_', do_not_clear=True, expand_x=True),
                sg.Button('SEND',expand_x=True, expand_y=False)],
                [sg.Text('Attached File:'), sg.In(key='_FILE_', do_not_clear=True, expand_x=True), sg.FileBrowse(),
                sg.Button('SENDATTACH',expand_x=True, expand_y=False)],
                [sg.Multiline(key='_CLIENT_', disabled=True, autoscroll=True, size=(60,15), expand_x=True, expand_y=True),
                sg.Multiline(key='_SERVER_', disabled=True, autoscroll=True, size=(60,15), expand_x=True, expand_y=True)],
                [sg.Button('QUIT', button_color=('white', 'red'))]
            ]


        layout = [[sg.Column(lay_col, element_justification='center', expand_x=True, expand_y=True)]]

        window = sg.Window('Messenger', layout, resizable=True, finalize=True, size=(1000,400))
        window.bind("<Escape>", "-ESCAPE-")


        while True:
            event, values = window.Read()

            if (event in (None, 'QUIT')) or (event in (sg.WINDOW_CLOSED, "-ESCAPE-")):
                sg.Popup('Closing Client APP', title='Closing', button_type=5, auto_close=True, auto_close_duration=1)
                break

            #if (values['_IN_'] == '') and (event != 'REGISTER' and event != 'CONNECTED USERS'):
             #   window['_CLIENT_'].print("c> No text inserted")
             #   continue

            if (client._alias == None or client._username == None or client._alias == 'Text' or client._username == 'Text' or client._date == None) and (event != 'REGISTER'):
                sg.Popup('NOT REGISTERED', title='ERROR', button_type=5, auto_close=True, auto_close_duration=1)
                continue

            if (event == 'REGISTER'):
                #   window['_CLIENT_'].print('c> REGISTER <userName>')
                client.window_register()

                if (client._alias == None or client._username == None or client._alias == 'Text' or client._username == 'Text' or client._date == None):
                    sg.Popup('NOT REGISTERED', title='ERROR', button_type=5, auto_close=True, auto_close_duration=1)
                    continue

                window['_CLIENT_'].print('c> REGISTER ' + client._alias)
                client.register(client._alias, window)

            elif (event == 'UNREGISTER'):
                window['_CLIENT_'].print('c> UNREGISTER ' + client._alias)
                client.unregister(client._alias, window)


            elif (event == 'CONNECT'):
                window['_CLIENT_'].print('c> CONNECT ' + client._alias)
                client.connect(client._alias, window)


            elif (event == 'DISCONNECT'):
                window['_CLIENT_'].print('c> DISCONNECT ' + client._alias)
                client.disconnect(client._alias, window)


            elif (event == 'SEND'):
                window['_CLIENT_'].print('c> SEND ' + values['_INDEST_'] + " " + values['_IN_'])

                if (values['_INDEST_'] != '' and values['_IN_'] != '' and values['_INDEST_'] != 'User' and values['_IN_'] != 'Text') :
                    client.send(values['_INDEST_'], values['_IN_'], window)
                else :
                    window['_CLIENT_'].print("Syntax error. Insert <destUser> <message>")


            elif (event == 'SENDATTACH'):

                window['_CLIENT_'].print('c> SENDATTACH ' + values['_INDEST_'] + " " + values['_IN_'] + " " + values['_FILE_'])

                if (values['_INDEST_'] != '' and values['_IN_'] != '' and values['_FILE_'] != '') :
                    client.sendAttach(values['_INDEST_'], values['_IN_'], values['_FILE_'], window)
                else :
                    window['_CLIENT_'].print("Syntax error. Insert <destUser> <message> <attachedFile>")


            elif (event == 'CONNECTED USERS'):
                window['_CLIENT_'].print("c> CONNECTEDUSERS")
                client.connectedUsers(window)



            window.Refresh()

        window.Close()


if __name__ == '__main__':
    client.main([])
    print("+++ FINISHED +++")
