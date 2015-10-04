bin = stereotunnel
root = ..
src = $(wildcard $(root)/src/*.c)
ccsrc = $(wildcard $(root)/src/*.cc)

include $(root)/libs/Makefile

# --- android settings ---
android_platform = android-19
name = $(bin)
pkgprefix = com.mutantstargoat
# ------------------------

src += $(wildcard $(root)/src/android/*.c)
jsrc = $(wildcard $(root)/src/android/*.java)

obj = $(src:.c=.o) $(ccsrc:.cc=.o)
lib = libs/armeabi/lib$(name).so
apk-release = bin/$(name).apk
apk-debug = bin/$(name)-debug.apk

pkg = $(pkgprefix).$(name)
act = $(pkg).MainActivity

CC = arm-linux-androideabi-gcc
CXX = arm-linux-androideabi-g++

android_libs = -lstdc++ -llog -landroid -lEGL -lGLESv2

defs += -DANDROID -DAPP_NAME=\"$(name)\" -DNO_FREETYPE -DHAVE_OPENGL_H
CXXFLAGS = -Wall -g $(defs) -march=armv7-a -mfloat-abi=softfp -mfpu=vfpv3-d16 \
		 $(android_inc) -I$(root)/src -I$(root)/src/android $(incdir)
CFLAGS = -std=c99 $(CXXFLAGS)
LDFLAGS = -Wl,--fix-cortex-a8 -Wl,-z,defs $(android_libs) $(libpaths) -lm

.PHONY: debug
debug: $(apk-debug)

.PHONY: release
release: $(apk-release)

$(apk-debug): $(lib) $(jsrc) AndroidManifest.xml assets/data assets/sdr
	ant debug

$(apk-release): $(lib) $(jsrc) AndroidManifest.xml assets/data assets/sdr
	ant release

assets/data:
	mkdir -p assets
	cd assets && rm -f data && ln -s ../../data .

assets/sdr:
	mkdir -p assets
	cd assets && rm -f sdr && ln -s ../../sdr .

.PHONY: lib
lib: $(lib)

$(lib): $(obj)
	@mkdir -p libs/armeabi
	$(CXX) -o $@ -shared $(obj) $(LDFLAGS)

ant_files = *.xml *.properties proguard-project.txt
ant_dirs = bin libs res obj src gen

.PHONY: clean
clean:
	rm -f $(obj) $(lib) $(apk-release) $(apk-debug) $(ant_files)
	rm -rf $(ant_dirs)
	rm -f assets/data assets/sdr
	[ -d assets ] && rmdir assets || true

.PHONY: install
install: install-debug

.PHONY: install-debug
install-debug:
	adb install -r $(apk-debug)

.PHONY: install-release
install-release:
	adb install -r $(apk-release)

.PHONY: uninstall
uninstall:
	adb uninstall $(pkg)

.PHONY: run
run:
	adb shell am start -n $(pkg)/$(act)

.PHONY: stop
stop:
	adb shell am force-stop $(pkg)

AndroidManifest.xml: manifest.xml.in
	android create project -p . -t $(android_platform) -k $(pkg) -a NativeActivity -n $(name)
	cat manifest.xml.in | sed 's/$$APPNAME/$(name)/g' | sed 's/$$APPTITLE/$(title)/g' >$@
	cd src && rm -f *.java && ln -s ../../src/android/*.java .

.PHONY: update-project
update-project: build.xml
	android update project -p . -t $(android_platform)

.PHONY: logcat
logcat:
	adb logcat $(name):V AndroidRuntime:V DEBUG:V '*:S'
