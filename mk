#!/bin/bash
gcc -w -g -o ser Server.c project1.h
gcc -w -g -o cli TCPclient.c project1.h
./ser 1100
