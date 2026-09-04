# Default variables

GAME_VERSION  ?= FIRERED
GAME_REVISION ?= 0
GAME_LANGUAGE ?= ENGLISH

# Builds the ROM using a modern compiler
MODERN        ?= 0
# Compares the ROM to a checksum of the original - only makes sense using when non-modern
COMPARE       ?= 0

KEEP_TEMPS    ?= 0

# A testing build. It gets its own ROM name and therefore its own save, so a
# debug run can never touch a real playthrough -- the same rule the Game Boy
# side follows. It is not part of `make all` and it will not match a .sha1.
DAEMONS_DEBUG ?= 0

ifeq (modern,$(MAKECMDGOALS))
  MODERN := 1
endif
ifeq (compare,$(MAKECMDGOALS))
  COMPARE := 1
endif

# For gbafix
MAKER_CODE := 01

BUILD_DIR := build

# Version
ifeq ($(GAME_VERSION),FIRERED)
  TITLE       := CONTENT
  GAME_CODE   := BPR
  BUILD_NAME  := firered
else
ifeq ($(GAME_VERSION),LEAFGREEN)
  TITLE       := CONTEXT
  GAME_CODE   := BPG
  BUILD_NAME  := leafgreen
else
  $(error unknown version $(GAME_VERSION))
endif
endif

# Revision
ifeq ($(GAME_REVISION),1)
  BUILD_NAME  := $(BUILD_NAME)_rev1
endif

ifeq ($(GAME_REVISION),10)
  BUILD_NAME  := $(BUILD_NAME)_switch
endif

# Modern GCC
ifeq ($(MODERN),1)
  BUILD_NAME := $(BUILD_NAME)_modern
endif

ifeq ($(DAEMONS_DEBUG),1)
  BUILD_NAME := $(BUILD_NAME)_debug
  # So two mGBA windows are telling apart. The header title is 12 bytes and
  # "CONTENT DEBUG" is 13, so it abbreviates.
  TITLE      := $(TITLE) DBG
endif

# Language
ifeq ($(GAME_LANGUAGE),ENGLISH)
  GAME_CODE  := $(GAME_CODE)E
endif
