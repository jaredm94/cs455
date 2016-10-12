#!/bin/bash
gcc -w -o ser Server.c
gcc -w -o cli TCPclient.c project1.h
./ser 1100
