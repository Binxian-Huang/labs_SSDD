# Compile and run service
## Server
To compile server use Makefile:
make
To run server:
./server -p <port>
IP and port where server listen will show in terminal

## Web service
To run web service:
python3 web_service.py

## Client
To run client, with IP adress and port provided from server:
python3 ./client.py -s <server_ip> -p <server_port>
