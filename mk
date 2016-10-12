#!/bin/bash
gcc -w -o ser Server.c project1.h
gcc -w -o cli TCPclient.c project1.h
./ser 1110
