CONTIKI_PROJECT = node
CONTIKI = /opt/contiki
#CONTIKI = /home/erik/skola/wcnes/contiki

CONTIKI_SOURCEFILES += callbacks.c proto.c
APPS = 

CFLAGS += -DPROJECT_CONF_H=1 -I../common

all: $(CONTIKI_PROJECT)

upload: $(CONTIKI_PROJECT).upload

include $(CONTIKI)/Makefile.include

