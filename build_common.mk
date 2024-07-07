$(foreach var,ANDROID_HOME ANDROID_PLATFORM BYTECODE_VER,\
    $(if $($(var)),,$(error variable $(var) is not set)))

AAPT ?= aapt2
JAVAC ?= javac
DX ?= d8
ZIP ?= zip
ZIPALIGN ?= zipalign
APKSIGNER ?= apksigner

ANDROIDJAR := $(ANDROID_HOME)/platforms/android-$(ANDROID_PLATFORM)/android.jar

JAVASRCS := $(shell find java -type f -name \*.java)
CLASSES := $(patsubst %.java,%.class,$(JAVASRCS))

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
		 $(JAVAFLAGS) $(JAVASRCS)
	$(DX) --classpath $(ANDROIDJAR) $(DXFLAGS) $(CLASSES)

merged.apk: res.apk classes.dex
	cp -fa $< $@
	zip -u9 $(ZIPFLAGS) $@ classes.dex

aligned.apk: merged.apk
	$(ZIPALIGN) -f $(ZAFLAGS) 4 $< $@

signed.apk: aligned.apk $(KEYS)
	cp -fa $< sign_tmp.apk
	$(APKSIGNER) sign $(SIGNFLAGS) sign_tmp.apk
	cp -fa sign_tmp.apk $@
	rm -f sign_tmp.apk
