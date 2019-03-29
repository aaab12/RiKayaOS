# Cross toolchain variables
# If these are not in your path, you can make them absolute.
XT_PRG_PREFIX = mipsel-linux-gnu-
# Compiler
CC = $(XT_PRG_PREFIX)gcc
# linker
LD = $(XT_PRG_PREFIX)ld

# uMPS2-related paths

# Simplistic search for the umps2 install. prefix. If you have umps2
# installed on some weird location, set UMPS2_DIR_PREFIX by hand.
ifneq ($(wildcard /usr/bin/umps2),)
    UMPS2_DIR_PREFIX = /usr
else
    UMPS2_DIR_PREFIX = /usr/local
endif

UMPS2_DATA_DIR = $(UMPS2_DIR_PREFIX)/share/umps2
UMPS2_INCLUDE_DIR = $(UMPS2_DIR_PREFIX)/include/umps2

IDIR = ../include
_DEPS = *.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

ODIR = obj
_OBJ = crtso.o handler.o interrupts.o libumps.o main.o p1.5test_rikaya_v0.o pcb.o scheduler.o syscall.o utils.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

# Compiler options
CFLAGS_LANG = -ffreestanding -ansi
CFLAGS_MIPS = -mips1 -mabi=32 -mno-gpopt -G 0 -mno-abicalls -fno-pic -mfp32 -std=gnu11
CFLAGS = $(CFLAGS_LANG) $(CFLAGS_MIPS) -I$(UMPS2_INCLUDE_DIR) -I$(IDIR) -Wall -O0

# Linker options
LDFLAGS = -G 0 -nostdlib -T $(UMPS2_DATA_DIR)/umpscore.ldscript

# Add the location of crt*.S to the search path
VPATH = $(UMPS2_DATA_DIR)

.PHONY : all clean

all : kernel.core.umps

kernel.core.umps : kernel
	umps2-elf2umps -k $<

kernel : $(OBJ)
	$(LD) -o $@ $^ $(LDFLAGS)

clean :
	-rm -f $(ODIR)/*.o kernel kernel.*.umps

$(ODIR)/%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

# Pattern rule for assembly modules
#$(ODIR)/crtso.o : crtso.S
#	$(CC) $(CFLAGS) -c -o $@ $<

$(ODIR)/libumps.o : libumps.S
	$(CC) $(CFLAGS) -c -o $@ $<
