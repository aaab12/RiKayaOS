# Cross toolchain variables
# If these are not in your path, you can make them absolute.
XT_PRG_PREFIX = mipsel-linux-gnu-
# Compiler
CC = $(XT_PRG_PREFIX)gcc
# linker
LD = $(XT_PRG_PREFIX)ld

# Simplistic search for the umps2 install. prefix. If you have umps2
# installed on some weird location, set UMPS2_DIR_PREFIX by hand.
ifneq ($(wildcard /usr/bin/umps2),)
    UMPS2_DIR_PREFIX = /usr
else
    UMPS2_DIR_PREFIX = /usr/local
endif

# uMPS2-related paths
UMPS2_DATA_DIR = $(UMPS2_DIR_PREFIX)/share/umps2
UMPS2_INCLUDE_DIR = $(UMPS2_DIR_PREFIX)/include/umps2

# Includes directory
IDIR = ../include
# Object files directory
ODIR = obj
# Object files
_OBJ = crtso.o handler.o interrupts.o libumps.o main.o p1.5test_rikaya_v0.o pcb.o scheduler.o syscall.o utils.o
# Object files path
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))
# Kernel file
_KERNEL = kernel
# Kernel folder
KDIR = kernel
# Kernel path
KERNEL = $(patsubst %,$(KDIR)/%,$(_KERNEL))

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
	@echo "==> done."

kernel.core.umps : $(KERNEL)
	@umps2-elf2umps -k $<
	@echo "==> converting..."

$(KERNEL) : $(OBJ)
	@mkdir $(KDIR)
	@$(LD) $(LDFLAGS) -o $@ $^
	@echo "==> linking..."

clean :
	@rm -rf $(ODIR) $(KDIR)
	@echo "==> cleaning..."

# Pattern rule for object files
$(ODIR)/%.o: %.c
	@$(CC) $(CFLAGS) -c -o $@ $<
	@echo "==> compiling" $<
# Pattern rule for crtso module
$(ODIR)/crtso.o : crtso.S
	@mkdir $(ODIR)
	@$(CC) $(CFLAGS) -c -o $@ $<
	@echo "==> compiling" $@
# Pattern rule for libumps module
$(ODIR)/libumps.o : libumps.S
	@$(CC) $(CFLAGS) -c -o $@ $<
	@echo "==> compiling" $@
