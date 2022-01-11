
CROSS_COMPILE = riscv64-unknown-linux-gnu-
CC		:= $(CROSS_COMPILE)gcc

OUTDIR = output
SRCDIR_NETWORK = network
SRCDIR_DETECTOR = detector

CFLAGS	:= -Wall -D__NO_INLINE__ -mcmodel=medany -std=gnu99
CFLAGS	+= -O0 -g 
CFLAGS	+= -Wno-unused -Wno-attributes -fno-delete-null-pointer-checks -march=rv64imafdc -mabi=lp64d
CFLAGS	+= -DCLEAR_MEM -DNPU_DEBUG
CFLAGS	+= -DSUPPORT_MALLOC

LIBS	:= -lm

OBJS = testapp.o
OBJS += network/post_process.o
OBJS += detector/detector.o


TARGET = testapp

.PHONY: all
all: $(TARGET)

#$(sw_hex): $(ASMS) $(OBJS) $(OBJS_NETWORK) $(OBJS_DETECTOR)
$(TARGET): $(OBJS) 

.PRECIOUS: $(OUTDIR)/%.elf $(OUTDIR)/%.dis $(OUTDIR)/%.bin

.PHONY: clean
clean:
	rm -rf ./*.o $(SRCDIR_NETWORK)/*.o $(SRCDIR_DETECTOR)/*.o $(TARGET)
	 	
