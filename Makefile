# ================================ project options
PROJECT := snake
GAME    := snake
INFO    := gameinfo.properties
BUILD_DIR := build
DATA_DIR  := data
GEN_DIR   := gen

BIN_DIR := ../../bin

UZEM_RUN_OPTS := -f

AUX_TARGETS := $(GAME).hex # $(GAME).uze $(GAME).lss $(GAME).eep
AUX_DEPS := Makefile $(wildcard $(DATA_DIR)/*)

KERNEL_OPTS  := -DVIDEO_MODE=1 -DINTRO_LOGO=0 -DSOUND_MIXER=0 -DMODE1_FAST_VSYNC=0
KERNEL_OPTS  += -DTILE_WIDTH=8 -DTILE_HEIGHT=8
AUX_LD_FLAGS := # -Wl,--section-start,.noinit=0x800100 -Wl,--section-start,.data=0x800500

FILES_C := $(shell find . -type f -iname '*.c')

# --------------------------------
FILES_C := $(patsubst ./%,%,$(FILES_C))
AUX_TARGETS := $(patsubst %,$(BUILD_DIR)/%,$(AUX_TARGETS))
DEP_DIR := $(BUILD_DIR)/dep

# ================================ other globals
MCU     := atmega644
CC      := avr-gcc

UZEBOX := ../../uzebox
KERNEL := $(UZEBOX)/kernel

# https://stackoverflow.com/a/8942216/
export PATH := $(PATH):$(UZEBOX)/bin:../bin

TARGET := $(BUILD_DIR)/$(GAME).elf

HEX_FLASH_FLAGS  := -R .eeprom
HEX_EEPROM_FLAGS := -j .eeprom
HEX_EEPROM_FLAGS += --set-section-flags=.eeprom="alloc,load"
HEX_EEPROM_FLAGS += --change-section-lma .eeprom=0 --no-change-warnings

# ================================ build flags
# options common to compile, link, and assembly rules
COMMON := -mmcu=$(MCU)

CFLAGS = $(COMMON)
CFLAGS += -Wall -gdwarf-2 -std=gnu99 -DF_CPU=28636360UL -Os
CFLAGS += -fsigned-char -ffunction-sections -fno-toplevel-reorder
CFLAGS += -MD -MP -MT $(*F).o -MF $(DEP_DIR)/$(@F).d
CFLAGS += $(KERNEL_OPTS)

ASMFLAGS = $(COMMON)
ASMFLAGS += $(CFLAGS)
ASMFLAGS += -x assembler-with-cpp -Wa,-gdwarf2

LDFLAGS := $(COMMON)
LDFLAGS += -Wl,-Map=$(BUILD_DIR)/$(GAME).map
LDFLAGS += -Wl,-gc-sections
LDFLAGS += $(AUX_LD_FLAGS)

INCLUDES := -I"$(KERNEL)"

# ================================ required objects
FILES_KERNEL := uzeboxVideoEngineCore.s uzeboxSoundEngineCore.s
FILES_KERNEL += uzeboxCore.c uzeboxSoundEngine.c uzeboxVideoEngine.c
FILES_KERNEL := $(patsubst %,$(KERNEL)/%,$(FILES_KERNEL))

# objects that must be built in order to link
OBJECTS := $(notdir $(FILES_KERNEL)) $(FILES_C)
OBJECTS := $(patsubst %.c,%.o,$(OBJECTS))
OBJECTS := $(patsubst %.s,%.o,$(OBJECTS))
OBJECTS := $(patsubst %,$(BUILD_DIR)/%,$(OBJECTS))

# ================================ build
.PHONY: all clean size run uze

all: $(TARGET) $(AUX_TARGETS)

# -------------------------------- ensure build dir gets created first
$(BUILD_DIR):
	mkdir $(BUILD_DIR)
	mkdir $(DEP_DIR)
	mkdir $(GEN_DIR)

# order-only prerequisite; OBJECTS don't depend on modification date anymore
$(OBJECTS): $(AUX_DEPS) | $(BUILD_DIR)

# -------------------------------- compile kernel files
$(BUILD_DIR)/%.o: $(KERNEL)/%.c
	@echo "CC $<"
	@$(CC) $(INCLUDES) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(KERNEL)/%.s
	@echo "CC $<"
	@$(CC) $(INCLUDES) $(ASMFLAGS) -c $< -o $@

# -------------------------------- compile or generate game files
$(BUILD_DIR)/%.o: %.c | $(GEN_DIR)
	@echo "CC $<"
	@$(CC) $(INCLUDES) $(CFLAGS) -c $< -o $@

# TODO: figure out how to get just the C files that actually depend on the inc/h files to have rules
# depending on them
$(GEN_DIR): $(patsubst $(DATA_DIR)/%.png,$(GEN_DIR)/%.inc, \
	$(shell find $(DATA_DIR) -type f -iname '*.png'))
$(DATA_DIR)/%.png: $(DATA_DIR)/%.map.json
$(GEN_DIR)/%.inc $(GEN_DIR)/%.h: $(DATA_DIR)/%.png # $(DATA_DIR)/%.map.json
	$(BIN_DIR)/tile_converter.py $< $(GEN_DIR)

# -------------------------------- final targets
$(TARGET): $(OBJECTS)
	@$(CC) $(LDFLAGS) $(OBJECTS) -o $@

%.hex: $(TARGET)
	avr-objcopy -O ihex $(HEX_FLASH_FLAGS)  $< $@

%.eep: $(TARGET)
	-avr-objcopy $(HEX_EEPROM_FLAGS) -O ihex $< $@ || exit 0

%.lss: $(TARGET)
	avr-objdump -h -S $< > $@

uze: $(TARGET:.elf=.uze)
%.uze: $(TARGET:.elf=.hex)
	-packrom $< $@ $(INFO)

# ================================ utility rules
clean:
	-$(RM) -rf $(BUILD_DIR)
	-$(RM) -rf $(GEN_DIR)

UNAME := $(shell sh -c 'uname -s 2>/dev/null || echo not')
AVRSIZEFLAGS := -A $(TARGET)
ifneq (,$(findstring MINGW,$(UNAME)))
AVRSIZEFLAGS := -C --mcu=$(MCU) $(TARGET)
endif

size: $(TARGET)
	@avr-size $(AVRSIZEFLAGS)

run: $(BUILD_DIR)/$(GAME).hex
	uzem $(UZEM_RUN_OPTS) $<

# ================================ add any other dependencies
-include $(wildcard $(DEP_DIR)/*)
