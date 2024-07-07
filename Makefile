ANDROID_PLATFORM := 34
BYTECODE_VER := 17
DXFLAGS += --no-desugaring
#DXFLAGS += --release

#KEYS := $(KEYSTORE)
#SIGNFLAGS := --ks $(KEYSTORE)

all: signed.apk

include build_common.mk

clean:
	rm -f *.apk $(CLASSES) *.dex *.idsig

.PHONY: all clean
