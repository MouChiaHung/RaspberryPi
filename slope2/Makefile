#
# Makefile:
#	
#################################################################################
# This file is part of wiringPi:
#	Wiring Compatable library for the Raspberry Pi
#
#    wiringPi is free software: you can redistribute it and/or modify
#    it under the terms of the GNU Lesser General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    wiringPi is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU Lesser General Public License for more details.
#
#    You should have received a copy of the GNU Lesser General Public License
#    along with wiringPi.  If not, see <http://www.gnu.org/licenses/>.
#################################################################################

DEBUG = -O3
CC = gcc
INCLUDE	= -I/usr/local/include
CFLAGS = $(DEBUG) -Wall $(INCLUDE) -Winline -pipe

LDFLAGS	= -L/usr/local/lib
LDLIBS = -lwiringPi -lwiringPiDev -lpthread -lm -lcrypt -lrt
OBJS =  main.o
EXE	= qbi

all: $(EXE)
	
$(EXE): $(OBJS)	
	echo [link]
	$(CC) -o $@ $(OBJS) $(LDFLAGS) $(LDLIBS)
	rm -r $(OBJS) 
	
.c.o:
	echo [CC] $<
	$(CC) -c $(CFLAGS) $< -o $@		
	
clean:
	rm -r $(EXE) $(OBJS) 
	
# DO NOT DELETE