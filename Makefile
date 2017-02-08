
APP_DIR 	= ./

OPTION =


PLATFORM=$(shell uname -p)

ifeq ("$(PLATFORM)", "x86_64")	
	LINUX   = -DLINUX64
	TARGET   = redis_r_trace
endif


DEPS_FILE  	   = $(TARGET).dep 

all:$(TARGET)
CC     =gcc
WARN   = -w -W -Werror -Wall

INC_DIR = -I./ -I./inc_pcap -I./inc_pcap/pcap
LIBS    = -L/usr/lib/
CFLAGS  = $(WARN) $(LINUX) $(INC_DIR) -fPIC $(OPTION)
         
ACTION = release

ifeq ($(ACTION),release)
CFLAGS += -O2
else
CFLAGS += -g
endif

#CFLAGS += -DINCLUDE_ZEROCOPY -DCHECK_DOG

#CC_DEF += 

C_RULE_FILE	   = c.rule
######################################################################################### 
#��������

#c and cpp file dependence compute rule
%.dcpp: %.cpp
	$(CC) $(CFLAGS) -M $< >> $(DEPS_FILE)
	@cat $(C_RULE_FILE) >> $(DEPS_FILE)

%.dc: %.c
	$(CC) $(CFLAGS) -M $< >> $(DEPS_FILE)
	@cat $(C_RULE_FILE) >> $(DEPS_FILE)


#�������������������ļ�
APP_SRCS 	:= $(foreach dir, $(APP_DIR), $(wildcard  $(dir)/*.c))
APP_SRCS 	+= $(foreach dir, $(APP_DIR), $(wildcard  $(dir)/*.cpp))
APP_SRCS  	:= $(filter %.c %.cpp, $(APP_SRCS))
APP_OBJS 	:= $(patsubst %.cpp,%.o, $(filter %.cpp, $(notdir $(APP_SRCS)))) 
APP_OBJS 	+= $(patsubst %.c,%.o, $(filter %.c, $(notdir $(APP_SRCS))))
 
APP_DEPS 	:= $(patsubst %.cpp,%.dcpp, $(filter %.cpp, $(APP_SRCS))) 
APP_DEPS 	+= $(patsubst %.c,%.dc, $(filter %.c, $(APP_SRCS)))

#########################################################################################
#����������ϵ
dep: del_dep $(APP_DEPS)
	@echo building all files dependency done

del_dep:
	@$(RM) $(DEPS_FILE) -rf

#########################################################################################
#���빤��
ifneq ($(wildcard $(DEPS_FILE)),)
include $(DEPS_FILE)
endif

$(TARGET):$(APP_OBJS)  
	rm -f /usr/lib/libpcap.so*
	rm -f /usr/lib64/libcapture.so
	cp libpcap.so.1 /usr/lib/
	ln -s /usr/lib/libpcap.so.1 /usr/lib/libpcap.so

	$(CC) $(CFLAGS) $(LIBS) $(APP_OBJS) -LS -o $(TARGET) -lpcap -lpthread


install:
	cd ../
	mkdir bin
	mv *.o ../bin -f              
clean:
	rm -f *.o *dep $(TARGET)

