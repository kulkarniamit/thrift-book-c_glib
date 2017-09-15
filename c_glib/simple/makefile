TARGET = server client
IDL = hello.thrift
SVRSRC = hello_server.c gen-c_glib/hello_svc.c gen-c_glib/hello_types.c
CLISRC = hello_client.c gen-c_glib/hello_svc.c gen-c_glib/hello_types.c

CC = gcc
LIBS = -lthrift_c_glib `pkg-config --libs glib-2.0 gobject-2.0`
CFLAGS = -Wall -ggdb3 `pkg-config --cflags glib-2.0 gobject-2.0`
THRIFT = thrift --gen c_glib

all: server client

gen-c_glib/hello_svc.c: $(IDL)
	$(THRIFT) $(IDL)

server: $(SVRSRC)
	$(CC) -o $@ $(SVRSRC) $(CFLAGS) $(LIBS)

client: $(CLISRC)
	$(CC) -o $@ $(CLISRC) $(CFLAGS) $(LIBS)

clean:
	$(RM) -r gen-c_glib
	$(RM) $(TARGET)
