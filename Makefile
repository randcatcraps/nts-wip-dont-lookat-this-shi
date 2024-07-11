ANDROID_PLATFORM := 34

BYTECODE_VER := 17
DXFLAGS += --no-desugaring
#DXFLAGS += --release

CFLAGS += -O2 -Wall -Wextra
#CFLAGS += -Werror
#LDFLAGS += -s

#KEYS := $(KEYSTORE)
#SIGNFLAGS := --ks $(KEYSTORE)

all: signed.apk

include build_common.mk

clean:
	$(call clean_classes)
	rm -f *.apk *.dex *.idsig $(DEPS) $(OBJS) $(JNILIB)

.PHONY: all clean
