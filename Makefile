
#**************************************#
#process input parameters
ifndef EXTENDED_LISTING
EXTENDED_LISTING = NO
endif

ifndef OPT_LEVEL
OPT_LEVEL = 0
endif
#**************************************#
#Define the used toolchain
TCHAIN = arm-none-eabi

#make with V=1 will enable verbose messages
ifeq ($(V), 1)
Q =
else
Q = @
endif

#**************************************#
#compiler, assembler and linker to use
GCC = $(TCHAIN)-gcc
GAS = $(TCHAIN)-as
LD = $(TCHAIN)-ld
SIZE = $(TCHAIN)-size
OBJCOPY = $(TCHAIN)-objcopy
OBJDUMP = $(TCHAIN)-objdump
NM = $(TCHAIN)-nm
WC = wc

#**************************************#
#some other tools that might be helpfull
MKDIR = mkdir
RM = rm -rf
COPY = cp

#**************************************#
#define directory structure
SRCDIR = src
OUTDIR = build
OBJDIR = $(OUTDIR)/obj
LISTDIR = $(OUTDIR)/list


#**************************************#
#define source files and linker script

#Add sources from spiffs
SPIFFS_SRC = ../3rdparty/spiffs/spiffs_cache.c ../3rdparty/spiffs/spiffs_check.c
SPIFFS_SRC+= ../3rdparty/spiffs/spiffs_gc.c ../3rdparty/spiffs/spiffs_hydrogen.c
SPIFFS_SRC+= ../3rdparty/spiffs/spiffs_nucleus.c

TARGET = main

CSRC = $(TARGET).c
CSRC+= lpc17xx/lpc17xx_sysinit.c cortex_m3/systick/systick.c
CSRC+= drivers/gpio/gpio.c board/landtiger/board.c drivers/uart/uart.c
CSRC+= drivers/ringbuf/ringbuf.c drivers/console/printf.c drivers/console/console.c
CSRC+= lib/mem_access.c lib/getopt.c tasks/task.c drivers/console/apps/mem_read.c
CSRC+= lib/sha3.c drivers/lpc_iap/iap.c lib/hash.c drivers/spiflash/spiflash.c
CSRC += $(SPIFFS_SRC) drivers/spiflash/flashfs.c
CSRC += drivers/rtc/rtc.c lockctrl/profile.c lockctrl/user.c lockctrl/entry_ctrl.c
CSRC += lockctrl/lock_output.c
CSRC += drivers/i2c/i2c.c drivers/mpr121/mpr121.c

ASMSRC = lpc17xx/lpc17xx.S

LDSCRIPT = scripts/lpc17xx.ld

VERSION_FILE = build/buildver.h

#**************************************#
#define additional include directories
ADDINCDIRS = . cortex_m3 lpc17xx cortex_m3/systick drivers drivers/console
ADDINCDIRS += include ../3rdparty/spiffs drivers/i2c drivers/mpr121
EXTRAINCDIR = $(addprefix $(SRCDIR)/, $(ADDINCDIRS))

#**************************************#
#define additional libraries
EXTRALIB = 

#**************************************#
#declare all sourcefiles seperated from path
ASMSRCFILES = $(notdir $(ASMSRC))
CSRCFILES = $(notdir $(CSRC))
ALLSRCFILES = $(ASMSRCFILES) $(CSRCFILES)

ALL_SRC_FILES = $(ASMSRC) $(CSRC)
PATHS = $(addprefix $(SRCDIR)/, $(sort $(dir $(ALL_SRC_FILES))))
VPATH =$(PATHS)

#**************************************#
#declare all object files derived from source files
ASMOBJ = $(addprefix $(OBJDIR)/, $(ASMSRCFILES:.S=.o))
COBJ = $(addprefix $(OBJDIR)/, $(CSRCFILES:.c=.o))
LST = $(ASMSRCFILES:%.S=%.lst) $(CSRC:%.c=%.lst)

#**************************************#
#define some flags to auto generate dependencies
GENDEPFLAGS = -MD -MP -MF .dep/$(@F).d
#**************************************#
#define all the flags
CPU = cortex-m3
DEBUG = dwarf-2
ASFLAGS =  -mcpu=$(CPU) -I. -mthumb
ASFLAGS += -adhlns=$(LISTDIR)/$(notdir $(<:.S=.lst)) -g$(DEBUG)


CFLAGS =  -O$(OPT_LEVEL) -g$(DEBUG) -mcpu=$(CPU) -mthumb -mthumb-interwork 
CFLAGS += -mlong-calls -ffunction-sections -fdata-sections --std=gnu99
CFLAGS += -Wall -Wextra -Wimplicit -Wpointer-arith -Wcast-align 
CFLAGS += -mfix-cortex-m3-ldrd $(GENDEPFLAGS)
CFLAGS += -Wredundant-decls -Wshadow -Wcast-qual -Wnested-externs 
CFLAGS += -Wa,-adhlns=$(LISTDIR)/$(notdir $(subst $(suffix $<),.lst,$<)) 
CFLAGS += $(patsubst %,-I%,$(EXTRAINCDIR)) --specs=rdimon.specs
CFLAGS += -include $(VERSION_FILE)

CDEFS = -D


EXTRA_LIBDIRS = /usr/arm-none-eabi/lib/armv7-m

EXTRA_LIBS = 

LDFLAGS = -nostartfiles -Wl,-Map=$(OUTDIR)/$(TARGET).map,--cref
LDFLAGS += -lc -lm -lrdimon -mcpu=$(CPU) -mthumb
LDFLAGS += $(patsubst %,-L%,$(EXTRA_LIBDIRS))
LDFLAGS += $(patsubst %,-l%,$(EXTRA_LIBS))
LDFLAGS +=-T$(LDSCRIPT)
ifeq ($(V), 1)
LDFLAGS += -Wl,--verbose
endif

#**************************************#
#Get the code size
ELFSIZE = $(SIZE) -A $(OUTDIR)/$(TARGET).elf -x
BINSIZE = $(WC) -c < $(OUTDIR)/$(TARGET).bin

#**************************************#
#make-targets
all: makedir genver build codesize

	

makedir:
	$(Q)$(MKDIR) -p $(OUTDIR)
	$(Q)$(MKDIR) -p $(OBJDIR)
	$(Q)$(MKDIR) -p $(LISTDIR)
	
genver:
	$(Q)scripts/genversion.sh $(VERSION_FILE)
	
ifeq ($(EXTENDED_LISTING), YES)	
build: $(OUTDIR)/$(TARGET).elf $(OUTDIR)/$(TARGET).hex $(OUTDIR)/$(TARGET).bin $(OUTDIR)/$(TARGET).sym $(OUTDIR)/$(TARGET).lss
else
build: $(OUTDIR)/$(TARGET).elf $(OUTDIR)/$(TARGET).hex $(OUTDIR)/$(TARGET).bin $(OUTDIR)/$(TARGET).sym
endif

codesize:
	@if [ -f $(OUTDIR)/$(TARGET).elf ]; then echo; echo Codesize of; $(ELFSIZE); fi
	@if [ -f $(OUTDIR)/$(TARGET).bin ]; then echo Binary size in bytes:; $(BINSIZE); fi
	$(Q)$(SIZE) -B -d $(ASMOBJ) $(COBJ) > $(OUTDIR)/$(TARGET).mem
	
#Convert to some usefull binary formats
#Create .hex-file from .elf
$(OUTDIR)/$(TARGET).hex: $(OUTDIR)/$(TARGET).elf
	$(Q)echo
	@echo Converting to $@
	$(Q)$(OBJCOPY) -O ihex $< $@
	
#Create .bin-file from .elf
$(OUTDIR)/$(TARGET).bin: $(OUTDIR)/$(TARGET).elf
	$(Q)echo
	@echo Converting to $@
	$(Q)$(OBJCOPY) -O binary $< $@

# Create extended listing file from ELF output file.
$(OUTDIR)/$(TARGET).lss: $(OUTDIR)/$(TARGET).elf
	$(Q)echo
	@echo Create extended listing $@
	$(Q)$(OBJDUMP) -h -S -C $< > $@

# Create a symbol table from ELF output file.
$(OUTDIR)/$(TARGET).sym: $(OUTDIR)/$(TARGET).elf
	$(Q)echo
	@echo Create symbol table $@
	$(Q)$(NM) -n $< > $@

#rule for linking
$(OUTDIR)/$(TARGET).elf: $(ASMOBJ) $(COBJ) $(LDSCRIPT) Makefile
	$(Q)echo
	@echo -n Linking $@...
	$(Q)$(GCC) $(ASMOBJ) $(COBJ) --output $@ $(LDFLAGS)
	@echo done!

#default rules for compiling assembler files
$(ASMOBJ): $(OBJDIR)/%.o:%.S Makefile
	$(Q)echo
	@echo Assembling $<
	$(Q)$(GAS) -c $(ASFLAGS) $< -o $@	
	
#default rules for compiling C files
$(COBJ) : $(OBJDIR)/%.o : %.c Makefile
	$(Q)echo
	@echo Compiling $<
	$(Q)$(GCC) -c $(CFLAGS) -s $< -o $@  
	
clean:
	@echo -n Cleaning up...
	$(Q)$(RM) $(OUTDIR) .dep
	@echo done!

# Include the dependency files.
-include $(shell mkdir .dep 2>/dev/null) $(wildcard .dep/*)


# Listing of phony targets.
.PHONY : all makedir genver build codesize clean