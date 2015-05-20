#!/bin/sh
tar czvf BBSMUD-`date "+%d-%m-%Y-%H-%M-%S"`.tgz *.c *.h Makefile .depend *.sh
