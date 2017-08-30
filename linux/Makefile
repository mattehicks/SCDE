# Makefile for SCDE Linux build
# start with ./scde
#

TARGET:=scde

OBJS:=\
scde_task.o \
main.o \
logging.o \
SCDE.o \
c_timeutils.o \
Help_Command.o \
Telnet_Module.o \
libtelnet.o \
#duk_trans_socket_unix.o \
#duk_module_duktape.o \
#dukf_utils.o \
#duktape.o \
#duktape_event.o \
#duktape_utils.o \
#modules.o 
#module_dukf.o \
#module_fs.o \
#module_os.o


CFLAGS:=-g
INCLUDES:= \
-I../main/include \
-I../components/Help_Command \
-I../components/Help_Command/include \
-I../components/Telnet_Module \
-I../components/Telnet_Module/include \
#-I../components/duktape/extras/module-duktape \
#-I../components/duktape/examples/debug-trans-socket \

#LIBS:=-lm -lcrypto

define cc-command
@echo "CC $<"
@$(CC) -c $(CFLAGS) $< $(INCLUDES)
endef


all: $(TARGET)

$(TARGET): $(OBJS)
	@echo "CC -o $(TARGET)"
	@$(CC) -o $(TARGET) $(OBJS) $(LIBS)

c_timeutils.o: ../main/c_timeutils.c
	$(cc-command)	

#duk_trans_socket_unix.o: ../components/duktape/examples/debug-trans-socket/duk_trans_socket_unix.c
#	$(cc-command)
		
#duk_module_duktape.o: ../components/duktape/extras/module-duktape/duk_module_duktape.c
#	$(cc-command)
	
#dukf_utils.o: ../main/dukf_utils.c
#	$(cc-command)

#duktape.o: ../components/duktape/src/duktape.c
#	$(cc-command)	

#duktape_event.o: ../main/duktape_event.c
#	$(cc-command)
	
scde_task.o: ../main/scde_task.c
	$(cc-command)

SCDE.o: ../main/SCDE.c
	$(cc-command)

#duktape_utils.o: ../main/duktape_utils.c
#	$(cc-command)

logging.o: ../main/logging.c
	$(cc-command)
		
main.o: main.c
	$(cc-command)	
	
#modules.o: ../main/modules.c
#	$(cc-command)

#module_dukf.o: ../main/module_dukf.c
#	$(cc-command)

#module_fs.o: ../main/module_fs.c
#	$(cc-command)

#module_os.o: ../main/module_os.c
#	$(cc-command)	
	


Help_Command.o: ../components/Help_Command/Help_Command.c
	$(cc-command)	

Telnet_Module.o: ../components/Telnet_Module/Telnet_Module.c
	$(cc-command)	

libtelnet.o: ../components/Telnet_Module/libtelnet.c
	$(cc-command)	

.c.o:
	@echo "CC $<"
	@$(CC) -c $(CFLAGS) $< $(INCLUDES)
	
clean:
	rm -f $(OBJS)
	rm -f $(TARGET)
