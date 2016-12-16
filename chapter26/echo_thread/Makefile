all:client server

client:client.c ../../chapter11/lib_function.c
	gcc -o client client.c ../../chapter11/lib_function.c -lpthread

server:server.c ../../chapter11/lib_function.c
	gcc -o server server.c ../../chapter11/lib_function.c -lpthread
