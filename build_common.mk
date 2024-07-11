define clean_classes
find java -type f -name \*.class -delete
endef

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

SRCS := $(shell find jni -type f -name \*.c)
DEPS := $(patsubst %.c,%.d,$(SRCS))
OBJS := $(patsubst %.c,%.o,$(SRCS))
JNILIB := lib/$(ANDROID_ABI)/libapp_jni.so

KEYS ?= testkey.pk8 testkey.x509.pem
SIGNFLAGS ?= --key testkey.pk8 --cert testkey.x509.pem

res.zip: $(shell find res -type f)
	$(AAPT) compile --dir res $(AAPTCFLAGS) -o $@

res.apk: AndroidManifest.xml res.zip $(ANDROIDJAR)
	$(AAPT) link -I $(ANDROIDJAR) \
		--manifest $< \
		--debug-mode \
		--target-sdk-version $(ANDROID_PLATFORM) \
		--java java \
		$(AAPTLDFLAGS) res.zip -o $@

# NOTE: R.java is updated while building res.apk
classes.dex: $(ANDROIDJAR) $(JAVASRCS) | res.apk
	$(call clean_classes)  # XXX: no easy way of knowing class deps
	find java -type f -name \*.java \
		  -exec $(JAVAC) -classpath $(ANDROIDJAR) \
				 --release $(BYTECODE_VER) \
				 -h jni $(JAVAFLAGS) {} +
	find java -type f -name \*.class \
		  -exec $(DX) --classpath $(ANDROIDJAR) \
				$(DXFLAGS) {} +

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
