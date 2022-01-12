
CROSS_COMPILE =
CC		:= $(CROSS_COMPILE)gcc

OUTDIR = output
SRCDIR_NETWORK = network
SRCDIR_DETECTOR = detector
SRCDIR_XDMADIR = xdma
LIB = ./

STATIC_DIR      :=

CFLAGS	:= -Wall -D__NO_INLINE__  -std=gnu99
CFLAGS	+= -lpthread 
CFLAGS	+= -O2 -g 
CFLAGS	+= -D__i386__ 
CFLAGS	+= -pg 
CFLAGS	+= -Wno-unused -Wno-attributes -fno-delete-null-pointer-checks 
CFLAGS	+= -DCLEAR_MEM -DNPU_DEBUG
CFLAGS	+= -DCLEAR_MEM
CFLAGS	+= -DTEST_ON_EMBEDDED_LINUX
CFLAGS	+= -fPIC -no-pie
CFLAGS  += -Ienlight -Inetwork

OBJS = xdma/xdma.o
OBJS += network/post_process.o
OBJS += detector/detector.o 
OBJS += detector/yolo_detector.o
OBJS += enlight/enlight_network.o
OBJS += npu_api.o
OBJS += testapp.o

TARGET = testapp

.PHONY: all
all: $(TARGET)

#$(sw_hex): $(ASMS) $(OBJS) $(OBJS_NETWORK) $(OBJS_DETECTOR)
$(TARGET): $(OBJS) 
	$(CC)  $(CFLAGS) -o $@ $^ $(STATIC_DIR)

.PRECIOUS: $(OUTDIR)/%.elf $(OUTDIR)/%.dis $(OUTDIR)/%.bin

.PHONY: clean
clean:
	rm -rf ./*.o $(SRCDIR_NETWORK)/*.o $(SRCDIR_DETECTOR)/*.o $(SRCDIR_XDMADIR)/*.o $(TARGET)
	 	
