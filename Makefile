
PRO = OpenBloxStudio.pro

ifeq ($(OS),Windows_NT)
	QMAKE = C:\Qt\5.4\mingw491_32\bin\qmake.exe
else
	QMAKE = qmake
endif

all:	QtMakefile
	$(MAKE) -f QtMakefile

clean:
	rm -fr QtMakefile QtMakefile.Debug QtMakefile.debug QtMakefile.Release QtMakefile.release object_script.OpenBloxStudio.Debug object_script.OpenBloxStudio.Release debug release OpenBloxStudio

QtMakefile:
	$(QMAKE) -o QtMakefile $(PRO) CONFIG+=debug_and_release

debug:	QtMakefile
	$(MAKE) -f QtMakefile debug

release:	QtMakefile
	$(MAKE) -f QtMakefile release

.PHONY: all clean debug clean-debug release clean-release

symlinks:
	rm -fr lua_src
	ln -s OpenBlox/lua_src lua_src
	rm -fr raknet
	ln -s OpenBlox/raknet raknet
	rm -fr res
	ln -s OpenBlox/res res