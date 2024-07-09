$(foreach var,ANDROID_ABI ANDROID_HOME ANDROID_HOST_TAG \
    ANDROID_NDK ANDROID_PLATFORM BYTECODE_VER CLANG_TRIPLET,\
    $(if $($(var)),,$(error variable $(var) is not set)))

AAPT ?= aapt2
JAVAC ?= javac
DX ?= d8
CC := $(ANDROID_NDK)/toolchains/llvm/prebuilt/$(ANDROID_HOST_TAG)/bin/$(CLANG_TRIPLET)$(ANDROID_PLATFORM)-clang
ZIP ?= zip
ZIPALIGN ?= zipalign
APKSIGNER ?= apksigner

ANDROIDJAR := $(ANDROID_HOME)/platforms/android-$(ANDROID_PLATFORM)/android.jar

JAVASRCS := $(shell find java -type f -name \*.java)
CLASSES := $(patsubst %.java,%.class,$(JAVASRCS))

SRCS := $(shell find jni -type f -name \*.c)
DEPS := $(patsubst %.c,%.d,$(SRCS))
OBJS := $(patsubst %.c,%.o,$(SRCS))
JNILIB := lib/$(ANDROID_ABI)/libapp_jni.so

KEYS ?= testkey.pk8 testkey.x509.pem
SIGNFLAGS ?= --key testkey.pk8 --cert testkey.x509.pem

res.apk: AndroidManifest.xml $(ANDROIDJAR)
	$(AAPT) link -I $(ANDROIDJAR) \
		--manifest $< \
		--debug-mode \
		--target-sdk-version $(ANDROID_PLATFORM) \
		$(AAPTFLAGS) -o $@

classes.dex: $(ANDROIDJAR) $(JAVASRCS)
	rm -f $(CLASSES)  # XXX: no easy way of knowing class deps
	$(JAVAC) -classpath $(ANDROIDJAR) \
		 --release $(BYTECODE_VER) \
		 -h jni $(JAVAFLAGS) $(JAVASRCS)
	$(DX) --classpath $(ANDROIDJAR) $(DXFLAGS) $(CLASSES)

-include $(DEPS)

# NOTE: headers are updated while building classes.dex
jni/%.o: jni/%.c | classes.dex
	$(CC) -MD -MT $@ -MP -MF $(patsubst %.c,%.d,$<) \
		$(CPPFLAGS) -fPIC -c $(CFLAGS) $< -o $@

$(JNILIB): $(OBJS)
	mkdir -p $(shell dirname $(JNILIB))
	$(CC) -shared $(LDFLAGS) $^ -o $@

merged.apk: res.apk classes.dex $(JNILIB)
	cp -fa $< $@
	zip -u9 $(ZIPFLAGS) $@ classes.dex $(JNILIB)

aligned.apk: merged.apk
	$(ZIPALIGN) -f $(ZAFLAGS) 4 $< $@

signed.apk: aligned.apk $(KEYS)
	cp -fa $< sign_tmp.apk
	$(APKSIGNER) sign $(SIGNFLAGS) sign_tmp.apk
	cp -fa sign_tmp.apk $@
	rm -f sign_tmp.apk
