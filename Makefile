# Makefile to rebuild SM64 split image

include util.mk

### Default target ###

default: all

### Build Options ###

# These options can either be changed by modifying the makefile, or
# by building with 'make SETTING=value'. 'make clean' may be required.

# Version of the game to build
VERSION ?= us
# Graphics microcode used
GRUCODE ?= f3dex2
# If COMPARE is 1, check the output sha1sum when building 'all'
COMPARE ?= 1
# If NON_MATCHING is 1, define the NON_MATCHING and AVOID_UB macros when building (recommended)
NON_MATCHING ?= 1
# Build for the N64 (turn this off for ports)
TARGET_N64 ?= 1
COMPILER = gcc

# CONSOLE - selects the console to target
#   bb - Targets the iQue Player (codenamed BB)
#   n64 - Targets the N64
CONSOLE ?= n64
$(eval $(call validate-option,CONSOLE,n64 bb))

ifeq      ($(CONSOLE),n64)
  INCLUDE_DIRS   += include/n64
  LIBS_DIR       := lib/n64
else ifeq ($(CONSOLE),bb)
  INCLUDE_DIRS   += include/ique
  LIBS_DIR       := lib/ique
  DEFINES        += BBPLAYER=1
endif


# Release

ifeq ($(VERSION),jp)
  VERSION_DEF := VERSION_JP
  GRUCODE_DEF := f3dex2
else
ifeq ($(VERSION),us)
  VERSION_DEF := VERSION_US
  GRUCODE_DEF := f3dex2
else
ifeq ($(VERSION),eu)
  VERSION_DEF := VERSION_EU
  GRUCODE_DEF := F3D_NEW
else
ifeq ($(VERSION),sh)
  $(warning Building SH is experimental and is prone to breaking. Try at your own risk.)
  VERSION_DEF := VERSION_SH
  GRUCODE_DEF := F3D_NEW
# TODO: GET RID OF THIS!!! We should mandate assets for Shindou like EU but we dont have the addresses extracted yet so we'll just pretend you have everything extracted for now.
  NOEXTRACT := 1 
else
  $(error unknown version "$(VERSION)")
endif
endif
endif
endif

TARGET := sm64.$(VERSION)
VERSION_CFLAGS := -D$(VERSION_DEF)
VERSION_ASFLAGS := --defsym $(VERSION_DEF)=1

# Microcode

ifeq ($(GRUCODE),f3dex) # Fast3DEX
  GRUCODE_DEF := F3DEX_GBI
  GRUCODE_ASFLAGS := --defsym F3DEX_GBI_SHARED=1
  TARGET := $(TARGET).f3dex
  COMPARE := 0
else
ifeq ($(GRUCODE), f3dex2) # Fast3DEX2
  GRUCODE_DEF := F3DEX_GBI_2
  GRUCODE_ASFLAGS := --defsym F3DEX_GBI_SHARED=1
  TARGET := $(TARGET).f3dex2
  COMPARE := 0
else
ifeq ($(GRUCODE),f3d_new) # Fast3D 2.0H (Shindou)
  GRUCODE_DEF := F3D_NEW
  TARGET := $(TARGET).f3d_new
  COMPARE := 0
else
ifeq ($(GRUCODE),f3dzex) # Fast3DZEX (2.0J / Animal Forest - Dōbutsu no Mori)
  $(warning Fast3DZEX is experimental. Try at your own risk.)
  GRUCODE_DEF := F3DEX_GBI_2
  GRUCODE_ASFLAGS := --defsym F3DEX_GBI_SHARED=1
  TARGET := $(TARGET).f3dzex
  COMPARE := 0
endif
endif
endif
endif

# Whether to hide commands or not
VERBOSE ?= 0
ifeq ($(VERBOSE),0)
  V := @
endif

# Whether to colorize build messages
COLOR ?= 1

GRUCODE_CFLAGS := -D$(GRUCODE_DEF)
GRUCODE_ASFLAGS := $(GRUCODE_ASFLAGS) --defsym $(GRUCODE_DEF)=1

ifeq ($(TARGET_N64),0)
  NON_MATCHING := 1
endif

ifeq ($(NON_MATCHING),1)
  MATCH_CFLAGS := -DNON_MATCHING -DAVOID_UB
  MATCH_ASFLAGS := --defsym AVOID_UB=1
  COMPARE := 0
endif

################### Universal Dependencies ###################

# (This is a bit hacky, but a lot of rules implicitly depend
# on tools and assets, and we use directory globs further down
# in the makefile that we want should cover assets.)

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(MAKECMDGOALS),distclean)

# Make sure assets exist
# CuckyDev: Disable extract by default
NOEXTRACT ?= 1
ifeq ($(NOEXTRACT),0)
DUMMY != ./extract_assets.py $(VERSION) >&2 || echo FAIL
ifeq ($(DUMMY),FAIL)
  $(error Failed to extract assets)
endif
endif

# Make tools if out of date
DUMMY != make -s -C tools >&2 || echo FAIL
ifeq ($(DUMMY),FAIL)
  $(error Failed to build tools)
endif

endif
endif

################ Target Executable and Sources ###############

# BUILD_DIR is location where all build artifacts are placed
BUILD_DIR_BASE := build
BUILD_DIR := $(BUILD_DIR_BASE)/$(VERSION)

ROM := $(BUILD_DIR)/$(TARGET).z64
ELF := $(BUILD_DIR)/$(TARGET).elf
LD_SCRIPT := sm64.ld
MIO0_DIR := $(BUILD_DIR)/bin
SOUND_BIN_DIR := $(BUILD_DIR)/sound
TEXTURE_DIR := textures
ACTOR_DIR := actors
LEVEL_DIRS := $(patsubst levels/%,%,$(dir $(wildcard levels/*/header.h)))

# Directories containing source files
SRC_DIRS := src src/engine src/game src/audio src/menu src/buffers src/objpool actors levels bin data assets
ASM_DIRS := asm lib src/game
BIN_DIRS := bin bin/$(VERSION)

MIPSISET := -mips2
MIPSBIT := -32

ifeq ($(COMPILER),gcc)
  MIPSISET := -mips3
endif

ifeq ($(VERSION),eu)
  OPT_FLAGS := -O2
else
ifeq ($(VERSION),sh)
  OPT_FLAGS := -O2
else
  OPT_FLAGS := -O2
endif
endif

# Use a default opt flag for gcc
ifeq ($(COMPILER),gcc)
  OPT_FLAGS := -Ofast -fno-unroll-loops -fno-peel-loops --param case-values-threshold=20 -fno-inline-functions -finline-functions-called-once

endif

# File dependencies and variables for specific files
include Makefile.split

# Source code files
LEVEL_C_FILES := $(wildcard levels/*/leveldata.c) $(wildcard levels/*/script.c) $(wildcard levels/*/geo.c)
C_FILES := $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.c)) $(LEVEL_C_FILES)
S_FILES := $(foreach dir,$(ASM_DIRS),$(wildcard $(dir)/*.s))
GENERATED_C_FILES := $(BUILD_DIR)/assets/mario_anim_data.c $(BUILD_DIR)/assets/demo_data.c

ifeq ($(VERSION),sh)
SOUND_BANK_FILES := $(wildcard sound/sound_banks/*.json)
SOUND_SEQUENCE_FILES := $(wildcard sound/sequences/jp/*.m64) \
    $(wildcard sound/sequences/*.m64) \
    $(foreach file,$(wildcard sound/sequences/jp/*.s),$(BUILD_DIR)/$(file:.s=.m64)) \
    $(foreach file,$(wildcard sound/sequences/*.s),$(BUILD_DIR)/$(file:.s=.m64))
else
SOUND_BANK_FILES := $(wildcard sound/sound_banks/*.json)
SOUND_SEQUENCE_FILES := $(wildcard sound/sequences/$(VERSION)/*.m64) \
    $(wildcard sound/sequences/*.m64) \
    $(foreach file,$(wildcard sound/sequences/$(VERSION)/*.s),$(BUILD_DIR)/$(file:.s=.m64)) \
    $(foreach file,$(wildcard sound/sequences/*.s),$(BUILD_DIR)/$(file:.s=.m64))
endif

SOUND_SAMPLE_DIRS := $(wildcard sound/samples/*)
SOUND_SAMPLE_AIFFS := $(foreach dir,$(SOUND_SAMPLE_DIRS),$(wildcard $(dir)/*.aiff))
SOUND_SAMPLE_TABLES := $(foreach file,$(SOUND_SAMPLE_AIFFS),$(BUILD_DIR)/$(file:.aiff=.table))
SOUND_SAMPLE_AIFCS := $(foreach file,$(SOUND_SAMPLE_AIFFS),$(BUILD_DIR)/$(file:.aiff=.aifc))
SOUND_OBJ_FILES := $(SOUND_BIN_DIR)/sound_data.o


# Object files
O_FILES := $(foreach file,$(C_FILES),$(BUILD_DIR)/$(file:.c=.o)) \
           $(foreach file,$(S_FILES),$(BUILD_DIR)/$(file:.s=.o)) \
           $(foreach file,$(GENERATED_C_FILES),$(file:.c=.o))

# Automatic dependency files
DEP_FILES := $(O_FILES:.o=.d)  $(BUILD_DIR)/$(LD_SCRIPT).d

# Files with GLOBAL_ASM blocks
ifeq ($(NON_MATCHING),0)
GLOBAL_ASM_C_FILES != grep -rl 'GLOBAL_ASM(' $(wildcard src/**/*.c)
GLOBAL_ASM_O_FILES = $(foreach file,$(GLOBAL_ASM_C_FILES),$(BUILD_DIR)/$(file:.c=.o))
GLOBAL_ASM_DEP = $(BUILD_DIR)/src/audio/non_matching_dep
endif

# Segment elf files
SEG_FILES := $(SEGMENT_ELF_FILES) $(ACTOR_ELF_FILES) $(LEVEL_ELF_FILES)

##################### Compiler Options #######################
ifneq ($(call find-command,mips64-elf-ld),)
  CROSS := mips64-elf-
else ifneq ($(call find-command,mips-n64-ld),)
  CROSS := mips-n64-
else ifneq ($(call find-command,mips64-ld),)
  CROSS := mips64-
else ifneq ($(call find-command,mips-linux-gnu-ld),)
  CROSS := mips-linux-gnu-
else ifneq ($(call find-command,mips64-linux-gnu-ld),)
  CROSS := mips64-linux-gnu-
else ifneq ($(call find-command,mips-ld),)
  CROSS := mips-
else
  $(error Unable to detect a suitable MIPS toolchain installed)
endif

export LD_LIBRARY_PATH=./tools

AS        := $(CROSS)as
CC        := $(CROSS)gcc
CPP       := cpp -P -Wno-trigraphs
ifneq ($(call find-command,mips-n64-ld),)
LD        := mips-n64-ld
else
LD        := tools/mips64-elf-ld
endif
AR        := $(CROSS)ar
OBJDUMP   := $(CROSS)objdump
OBJCOPY   := $(CROSS)objcopy
PYTHON    := python3

ifeq ($(TARGET_N64),1)
  TARGET_CFLAGS := -nostdinc -I include/libc -DTARGET_N64 -D_LANGUAGE_C
  CC_CFLAGS := -fno-builtin
endif

INCLUDE_CFLAGS := -I include -I $(BUILD_DIR) -I $(BUILD_DIR)/include -I src -I . $(foreach i,$(INCLUDE_DIRS),-I$(i))

COMMON_CFLAGS = $(OPT_FLAGS) $(TARGET_CFLAGS) $(INCLUDE_CFLAGS) $(VERSION_CFLAGS) $(MATCH_CFLAGS) $(MIPSISET) $(GRUCODE_CFLAGS) $(foreach d,$(DEFINES),-D$(d))

ASMFLAGS = -G 0 $(OPT_FLAGS) $(TARGET_CFLAGS) -mips3 -mno-shared -march=vr4300 -mfix4300 -mabi=32 -mdivide-breaks -fno-PIC -mno-abicalls
ASFLAGS := -march=vr4300 -mabi=32 -I include -I $(BUILD_DIR) $(VERSION_ASFLAGS) $(MATCH_ASFLAGS) $(GRUCODE_ASFLAGS)
CFLAGS := -mdivide-breaks -mno-check-zero-division -march=vr4300 -mfix4300 -mabi=32 -mno-shared -G 0 -mhard-float -fno-stack-protector -fno-common -fno-zero-initialized-in-bss -I include -I src/ -I $(BUILD_DIR)/include -fno-PIC -mno-abicalls -fno-strict-aliasing -fno-inline-functions -ffreestanding -fwrapv -Wall -Wextra $(COMMON_CFLAGS)
SYMBOL_LINKING_FLAGS := $(addprefix -R ,$(SEG_FILES))
LDFLAGS := -Os -T undefined_syms.txt -T $(BUILD_DIR)/$(LD_SCRIPT) -Map $(BUILD_DIR)/sm64.$(VERSION).map --no-check-sections $(SYMBOL_LINKING_FLAGS)
ENDIAN_BITWIDTH := $(BUILD_DIR)/endian-and-bitwidth


# Prevent a crash with -sopt
export LANG := C

####################### Other Tools #########################

# N64 tools
TOOLS_DIR = tools
MIO0TOOL = $(TOOLS_DIR)/mio0
RNCPACK  = $(TOOLS_DIR)/rncpack
N64CKSUM = $(TOOLS_DIR)/n64cksum
N64GRAPHICS = $(TOOLS_DIR)/n64graphics
N64GRAPHICS_CI = $(TOOLS_DIR)/n64graphics_ci
TEXTCONV = $(TOOLS_DIR)/textconv
AIFF_EXTRACT_CODEBOOK = $(TOOLS_DIR)/aiff_extract_codebook
VADPCM_ENC = $(TOOLS_DIR)/vadpcm_enc
EXTRACT_DATA_FOR_MIO = $(TOOLS_DIR)/extract_data_for_mio
SKYCONV = $(TOOLS_DIR)/skyconv
EMULATOR = mupen64plus
EMU_FLAGS = --noosd
LOADER = loader64
LOADER_FLAGS = -vwf
SHA1SUM = sha1sum
PRINT = printf

ifeq ($(COLOR),1)
NO_COL  := \033[0m
RED     := \033[0;31m
GREEN   := \033[0;32m
BLUE    := \033[0;34m
YELLOW  := \033[0;33m
BLINK   := \033[33;5m
endif

# Common build print status function
define print
  @$(PRINT) "$(GREEN)$(1) $(YELLOW)$(2)$(GREEN) -> $(BLUE)$(3)$(NO_COL)\n"
endef

ifeq (, $(shell which armips 2>/dev/null))
  RSPASM := $(TOOLS_DIR)/armips
else
  RSPASM = armips
endif

# Use Objcopy instead of extract_data_for_mio
ifeq ($(COMPILER),gcc)
EXTRACT_DATA_FOR_MIO := $(OBJCOPY) -O binary --only-section=.data
endif

######################## Targets #############################

all: $(ROM)
	@$(SHA1SUM) $(ROM)
	@$(PRINT) "${GREEN}Build succeeded.\n"

clean:
	$(RM) -r $(BUILD_DIR_BASE)

distclean:
	$(RM) -r $(BUILD_DIR_BASE)
	./extract_assets.py --clean

test: $(ROM)
	$(EMULATOR) $(EMU_FLAGS) $<

load: $(ROM)
	$(LOADER) $(LOADER_FLAGS) $<

$(BUILD_DIR)/asm/boot.o: $(IPL3_RAW_FILES)
$(BUILD_DIR)/src/game/crash_screen.o: $(CRASH_TEXTURE_C_FILES)

$(BUILD_DIR)/lib/rsp.o: $(BUILD_DIR)/rsp/rspboot.bin $(BUILD_DIR)/rsp/fast3d.bin $(BUILD_DIR)/rsp/audio.bin

$(BUILD_DIR)/include/text_strings.h: include/text_strings.h.in
	$(TEXTCONV) charmap.txt $< $@

$(BUILD_DIR)/include/text_menu_strings.h: include/text_menu_strings.h.in
	$(TEXTCONV) charmap_menu.txt $< $@

ifeq ($(COMPILER),gcc)
$(BUILD_DIR)/lib/src/math/%.o: CFLAGS += -fno-builtin
endif

ifeq ($(VERSION),eu)
TEXT_DIRS := text/de text/us text/fr

# EU encoded text inserted into individual segment 0x19 files,
# and course data also duplicated in leveldata.c
$(BUILD_DIR)/bin/eu/translation_en.o: $(BUILD_DIR)/text/us/define_text.inc.c
$(BUILD_DIR)/bin/eu/translation_de.o: $(BUILD_DIR)/text/de/define_text.inc.c
$(BUILD_DIR)/bin/eu/translation_fr.o: $(BUILD_DIR)/text/fr/define_text.inc.c
$(BUILD_DIR)/levels/menu/leveldata.o: $(BUILD_DIR)/text/us/define_courses.inc.c
$(BUILD_DIR)/levels/menu/leveldata.o: $(BUILD_DIR)/text/de/define_courses.inc.c
$(BUILD_DIR)/levels/menu/leveldata.o: $(BUILD_DIR)/text/fr/define_courses.inc.c

else
ifeq ($(VERSION),sh)
TEXT_DIRS := text/jp
$(BUILD_DIR)/bin/segment2.o: $(BUILD_DIR)/text/jp/define_text.inc.c

else
TEXT_DIRS := text/$(VERSION)

# non-EU encoded text inserted into segment 0x02
$(BUILD_DIR)/bin/segment2.o: $(BUILD_DIR)/text/$(VERSION)/define_text.inc.c
endif
endif

$(BUILD_DIR)/text/%/define_courses.inc.c: text/define_courses.inc.c text/%/courses.h
	$(CPP) $(VERSION_CFLAGS) $< -o $@ -I text/$*/
	$(TEXTCONV) charmap.txt $@ $@

$(BUILD_DIR)/text/%/define_text.inc.c: text/define_text.inc.c text/%/courses.h text/%/dialogs.h
	$(CPP) $(VERSION_CFLAGS) $< -o $@ -I text/$*/
	$(TEXTCONV) charmap.txt $@ $@

RSP_DIRS := $(BUILD_DIR)/rsp
ALL_DIRS := $(BUILD_DIR) $(addprefix $(BUILD_DIR)/,$(SRC_DIRS) $(ASM_DIRS) $(BIN_DIRS) $(TEXTURE_DIRS) $(TEXT_DIRS) $(SOUND_SAMPLE_DIRS) $(addprefix levels/,$(LEVEL_DIRS)) include) $(MIO0_DIR) $(addprefix $(MIO0_DIR)/,$(VERSION)) $(SOUND_BIN_DIR) $(SOUND_BIN_DIR)/sequences/$(VERSION) $(RSP_DIRS)

# Make sure build directory exists before compiling anything
DUMMY != mkdir -p $(ALL_DIRS)

$(BUILD_DIR)/include/text_strings.h: $(BUILD_DIR)/include/text_menu_strings.h
$(BUILD_DIR)/src/menu/file_select.o: $(BUILD_DIR)/include/text_strings.h
$(BUILD_DIR)/src/menu/star_select.o: $(BUILD_DIR)/include/text_strings.h
$(BUILD_DIR)/src/game/ingame_menu.o: $(BUILD_DIR)/include/text_strings.h

################################################################
# TEXTURE GENERATION                                           #
################################################################

# RGBA32, RGBA16, IA16, IA8, IA4, IA1, I8, I4
$(BUILD_DIR)/%: %.png
	$(call print,Converting:,$<,$@)
	$(V)$(N64GRAPHICS) -i $@ -g $< -f $(lastword $(subst ., ,$@))

$(BUILD_DIR)/%.inc.c: $(BUILD_DIR)/% %.png
	$(call print,Converting:,$<,$@)
	$(V)hexdump -v -e '1/1 "0x%X,"' $< > $@
	$(V)echo >> $@

# Color Index CI8
$(BUILD_DIR)/%.ci8: %.ci8.png
	$(call print,Converting:,$<,$@)
	$(V)$(N64GRAPHICS_CI) -i $@ -g $< -f ci8

# Color Index CI4
$(BUILD_DIR)/%.ci4: %.ci4.png
	$(call print,Converting:,$<,$@)
	$(V)$(N64GRAPHICS_CI) -i $@ -g $< -f ci4

################################################################

# compressed segment generation

# TODO: ideally this would be `-Trodata-segment=0x07000000` but that doesn't set the address

$(BUILD_DIR)/bin/%.elf: $(BUILD_DIR)/bin/%.o
	$(call print,Linking ELF file:,$<,$@)
	$(V)$(LD) -e 0 -Ttext=$(SEGMENT_ADDRESS) -Map $@.map -o $@ $<
$(BUILD_DIR)/actors/%.elf: $(BUILD_DIR)/actors/%.o
	$(call print,Linking ELF file:,$<,$@)
	$(V)$(LD) -e 0 -Ttext=$(SEGMENT_ADDRESS) -Map $@.map -o $@ $<

# Override for level.elf, which otherwise matches the above pattern
.SECONDEXPANSION:
$(BUILD_DIR)/levels/%/leveldata.elf: $(BUILD_DIR)/levels/%/leveldata.o $(BUILD_DIR)/bin/$$(TEXTURE_BIN).elf
	$(call print,Linking ELF file:,$<,$@)
	$(V)$(LD) -e 0 -Ttext=$(SEGMENT_ADDRESS) -Map $@.map --just-symbols=$(BUILD_DIR)/bin/$(TEXTURE_BIN).elf -o $@ $<

$(BUILD_DIR)/bin/%.bin: $(BUILD_DIR)/bin/%.elf
	$(call print,Extracting compressible data from:,$<,$@)
	$(V)$(EXTRACT_DATA_FOR_MIO) $< $@

$(BUILD_DIR)/actors/%.bin: $(BUILD_DIR)/actors/%.elf
	$(call print,Extracting compressible data from:,$<,$@)
	$(V)$(EXTRACT_DATA_FOR_MIO) $< $@

$(BUILD_DIR)/levels/%/leveldata.bin: $(BUILD_DIR)/levels/%/leveldata.elf
	$(call print,Extracting compressible data from:,$<,$@)
	$(V)$(EXTRACT_DATA_FOR_MIO) $< $@

$(BUILD_DIR)/%.mio0: $(BUILD_DIR)/%.bin
	$(call print,Compressing:,$<,$@)
	$(V)$(RNCPACK) p $< $@ -m1

$(BUILD_DIR)/%.mio0.o: $(BUILD_DIR)/%.mio0
	$(call print,Converting RNC1 to ELF:,$<,$@)
	$(V)$(LD) -r -b binary $< -o $@

$(BUILD_DIR)/%.table: %.aiff
	$(call print,Extracting codebook:,$<,$@)
	$(V)$(AIFF_EXTRACT_CODEBOOK) $< >$@

$(BUILD_DIR)/%.aifc: $(BUILD_DIR)/%.table %.aiff
	$(call print,Encoding ADPCM:,$(word 2,$^),$@)
	$(V)$(VADPCM_ENC) -c $^ $@

$(BUILD_DIR)/rsp/%.bin $(BUILD_DIR)/rsp/%_data.bin: rsp/%.s
	$(call print,Assembling:,$<,$@)
	$(V)$(RSPASM) -sym $@.sym -definelabel $(VERSION_DEF) 1 -definelabel $(GRUCODE_DEF) 1 -strequ CODE_FILE $(BUILD_DIR)/rsp/$*.bin -strequ DATA_FILE $(BUILD_DIR)/rsp/$*_data.bin $<

$(ENDIAN_BITWIDTH): tools/determine-endian-bitwidth.c
	@$(PRINT) "$(GREEN)Generating endian-bitwidth $(NO_COL)\n"
	$(V)$(CC) -c $(CFLAGS) -o $@.dummy2 $< 2>$@.dummy1; true
	$(V)grep -o 'msgbegin --endian .* --bitwidth .* msgend' $@.dummy1 > $@.dummy2
	$(V)head -n1 <$@.dummy2 | cut -d' ' -f2-5 > $@
	$(V)$(RM) $@.dummy1
	$(V)$(RM) $@.dummy2

$(SOUND_BIN_DIR)/sound_data.ctl: sound/sound_banks/ $(SOUND_BANK_FILES) $(SOUND_SAMPLE_AIFCS) $(ENDIAN_BITWIDTH)
	@$(PRINT) "$(GREEN)Generating:  $(BLUE)$@ $(NO_COL)\n"
	$(V)$(PYTHON) tools/assemble_sound.py $(BUILD_DIR)/sound/samples/ sound/sound_banks/ $(SOUND_BIN_DIR)/sound_data.ctl $(SOUND_BIN_DIR)/sound_data.tbl $(VERSION_CFLAGS) $$(cat $(ENDIAN_BITWIDTH))

$(SOUND_BIN_DIR)/sound_data.tbl: $(SOUND_BIN_DIR)/sound_data.ctl
	@true

ifeq ($(VERSION),sh)
$(SOUND_BIN_DIR)/sequences.bin: $(SOUND_BANK_FILES) sound/sequences.json sound/sequences/ sound/sequences/jp/ $(SOUND_SEQUENCE_FILES) $(ENDIAN_BITWIDTH)
	@$(PRINT) "$(GREEN)Generating:  $(BLUE)$@ $(NO_COL)\n"
	$(V)$(PYTHON) tools/assemble_sound.py --sequences $@ $(SOUND_BIN_DIR)/bank_sets sound/sound_banks/ sound/sequences.json $(SOUND_SEQUENCE_FILES) $(VERSION_CFLAGS) $$(cat $(ENDIAN_BITWIDTH))
else
$(SOUND_BIN_DIR)/sequences.bin: $(SOUND_BANK_FILES) sound/sequences.json sound/sequences/ sound/sequences/$(VERSION)/ $(SOUND_SEQUENCE_FILES) $(ENDIAN_BITWIDTH)
	@$(PRINT) "$(GREEN)Generating:  $(BLUE)$@ $(NO_COL)\n"
	$(V)$(PYTHON) tools/assemble_sound.py --sequences $@ $(SOUND_BIN_DIR)/bank_sets sound/sound_banks/ sound/sequences.json $(SOUND_SEQUENCE_FILES) $(VERSION_CFLAGS) $$(cat $(ENDIAN_BITWIDTH))
endif

$(SOUND_BIN_DIR)/bank_sets: $(SOUND_BIN_DIR)/sequences.bin
	@true

$(SOUND_BIN_DIR)/%.m64: $(SOUND_BIN_DIR)/%.o
	$(call print,Converting to M64:,$<,$@)
	$(V)$(OBJCOPY) -j .rodata $< -O binary $@

$(SOUND_BIN_DIR)/%.o: $(SOUND_BIN_DIR)/%.s
	$(call print,Assembling:,$<,$@)
	$(V)$(AS) $(ASFLAGS) -o $@ $<

# Assemble RNC1
# cringeass hack but fuck you i'm not fixing all the asm in this repo to work with preprocessor
$(BUILD_DIR)/src/game/rnc1.o: src/game/rnc1.s
	$(call print,Assembling:,$<,$@)
	$(V)$(CC) -c $(ASMFLAGS) -x assembler-with-cpp -MMD -MF $(BUILD_DIR)/src/game/rnc1.d  -o $@ $<

$(SOUND_BIN_DIR)/%.inc.c: $(SOUND_BIN_DIR)/%
	$(call print,Converting to C:,$<,$@)
	$(V)hexdump -v -e '1/1 "0x%X,"' $< > $@
	$(V)echo >> $@

$(SOUND_BIN_DIR)/sound_data.o: $(SOUND_BIN_DIR)/sound_data.ctl $(SOUND_BIN_DIR)/sound_data.tbl $(SOUND_BIN_DIR)/sequences.bin $(SOUND_BIN_DIR)/bank_sets

$(BUILD_DIR)/levels/scripts.o: $(BUILD_DIR)/include/level_headers.h

$(BUILD_DIR)/include/level_headers.h: levels/level_headers.h.in
	$(call print,Preprocessing level headers:,$<,$@)
	$(V)$(CPP) -I . levels/level_headers.h.in | $(PYTHON) tools/output_level_headers.py > $(BUILD_DIR)/include/level_headers.h

$(BUILD_DIR)/assets/mario_anim_data.c: $(wildcard assets/anims/*.inc.c)
	@$(PRINT) "$(GREEN)Generating animation data $(NO_COL)\n"
	$(V)$(PYTHON) tools/mario_anims_converter.py > $@

$(BUILD_DIR)/assets/demo_data.c: assets/demo_data.json $(wildcard assets/demos/*.bin)
	@$(PRINT) "$(GREEN)Generating demo data $(NO_COL)\n"
	$(V)$(PYTHON) tools/demo_data_converter.py assets/demo_data.json $(VERSION_CFLAGS) > $@

$(BUILD_DIR)/%.o: %.c
	$(call print,Compiling:,$<,$@)
	$(V)$(CC) -c $(CFLAGS) -MMD -MF $(BUILD_DIR)/$*.d  -o $@ $<
$(BUILD_DIR)/%.o: $(BUILD_DIR)/%.c
	$(call print,Compiling:,$<,$@)
	$(V)$(CC) -c $(CFLAGS) -MMD -MF $(BUILD_DIR)/$*.d  -o $@ $<

$(BUILD_DIR)/%.o: %.s
	$(call print,Assembling:,$<,$@)
	$(V)$(AS) $(ASFLAGS) -MD $(BUILD_DIR)/$*.d -o $@ $<

$(BUILD_DIR)/$(LD_SCRIPT): $(LD_SCRIPT)
	$(call print,Preprocessing linker script:,$<,$@)
	$(V)$(CPP) $(VERSION_CFLAGS) -MMD -MP -MT $@ -MF $@.d -I include/ -I . -DBUILD_DIR=$(BUILD_DIR) -o $@ $<

$(ELF): $(O_FILES) $(MIO0_OBJ_FILES) $(SOUND_OBJ_FILES) $(SEG_FILES) $(BUILD_DIR)/$(LD_SCRIPT) undefined_syms.txt
	@$(PRINT) "$(GREEN)Linking ELF file:  $(BLUE)$@ $(NO_COL)\n"
	$(V)$(LD) --gc-sections -L $(BUILD_DIR) $(LDFLAGS) -o $@ $(O_FILES)$(LIBS) -Llib -L$(LIBS_DIR) -lultra_rom -lgcc

$(ROM): $(ELF)
	$(call print,Building ROM:,$<,$@)
ifeq      ($(CONSOLE),n64)
	$(V)$(OBJCOPY) --pad-to=0x800000 --gap-fill=0xFF $< $@ -O binary
else ifeq ($(CONSOLE),bb)
	$(V)$(OBJCOPY) --gap-fill=0x00 $< $@ -O binary
	$(V)dd if=$@ of=tmp bs=16K conv=sync
	$(V)mv tmp $@
endif
	$(V)$(N64CKSUM) $@

$(BUILD_DIR)/$(TARGET).objdump: $(ELF)
	$(OBJDUMP) -D $< > $@

.PHONY: all clean distclean default diff test load
# with no prerequisites, .SECONDARY causes no intermediate target to be removed
.SECONDARY:

# Remove built-in rules, to improve performance
MAKEFLAGS += --no-builtin-rules

-include $(DEP_FILES)

print-% : ; $(info $* is a $(flavor $*) variable set to [$($*)]) @true
