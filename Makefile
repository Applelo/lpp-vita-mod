TARGET		:= lpp-vita-mod
SOURCES		:= source/include/lua source/include/ftp source/include source \
			source/include/draw
INCLUDES	:= include

LIBS =	-lftpvita -lvita2d -lpng -ljpeg -lz -lm -lc -lfreetype \
	-lSceAppMgr_stub -lSceAppUtil_stub -lSceAudio_stub -lSceCommonDialog_stub \
	-lSceCtrl_stub -lSceDisplay_stub -lSceGxm_stub -lSceIme_stub \
	-lSceKernel_stub -lSceMusicExport_stub -lSceNet_stub -lSceNetCtl_stub \
	-lSceSysmodule_stub -lScePower_stub -lSceTouch_stub -lScePgf_stub \
	-lScePvf_stub -lUVLoader_stub

CFILES   := $(foreach dir,$(SOURCES), $(wildcard $(dir)/*.c))
CPPFILES   := $(foreach dir,$(SOURCES), $(wildcard $(dir)/*.cpp))
BINFILES := $(foreach dir,$(DATA), $(wildcard $(dir)/*.bin))
OBJS     := $(addsuffix .o,$(BINFILES)) $(CFILES:.c=.o) $(CPPFILES:.cpp=.o) 

PREFIX   = arm-vita-eabi
CC       = $(PREFIX)-gcc
CXX      = $(PREFIX)-g++
CFLAGS   += -Wl,-q -Wall -O3 -Wno-unused-variable -Wno-unused-but-set-variable \
	$(foreach dir, $(FEXDIRS), -I$(dir))
CXXFLAGS = $(CFLAGS) -std=c++11 -fno-rtti -fno-exceptions
ASFLAGS  = $(CFLAGS)

all: $(TARGET).velf

%.velf: %.elf
	$(PREFIX)-strip -g $<
	vita-elf-create $< $@ > /dev/null

$(TARGET).elf: $(OBJS)
	$(CXX) $(CXXFLAGS) $^ $(LIBS) -o $@

clean:
	@rm -rf $(TARGET).velf $(TARGET).elf $(OBJS)

run: $(TARGET).velf
	@sh run_homebrew_unity.sh $(TARGET).velf