QT = core gui widgets websockets opengl
win32{
	LIBS += -lws2_32 -lcurldll -lidn -lfmodex
}else{
	LIBS += -lGL -lcurl -lfmodex64
}
LIBS += -lcrypto -lssl -lz -lSDL2 -lSDL2_ttf -lSDL2_image
HEADERS = OpenBlox/lua_src/*.h OpenBlox/raknet/*.h OpenBlox/src/ob_enum/*.h OpenBlox/src/ob_type/*.h OpenBlox/src/ob_lua/*.h OpenBlox/src/ob_instance/*.h OpenBlox/src/openblox/*.h src/*.h
SOURCES = OpenBlox/lua_src/*.c OpenBlox/raknet/*.cpp OpenBlox/src/ob_enum/*.cpp OpenBlox/src/ob_type/*.cpp OpenBlox/src/ob_lua/*.cpp OpenBlox/src/ob_instance/*.cpp OpenBlox/src/openblox/*.cpp src/*.cpp
CONFIG += c++11 debug
DEFINES += GXX_EXPERIMENTAL_CXX0X OPENBLOX_STUDIO
RESOURCES =
INCLUDEPATH += OpenBlox/src/ob_enum OpenBlox/src/ob_instance OpenBlox/src/ob_lua OpenBlox/src/ob_type OpenBlox/src/openblox src

_JENKINS = $$(JENKINS_HOME)
!isEmpty(_JENKINS){
	contains(CONFIG, debug){
		CONFIG -= debug
	}
}

win32{
	# Copy required DLLs to output directory

	CONFIG(debug, debug|release){
		icudt53.commands = cp $$[QT_INSTALL_BINS]/icudt53.dll debug
		icudt53.target = debug/icudt53.dll
		
		icuin53.commands = cp $$[QT_INSTALL_BINS]/icuin53.dll debug
		icuin53.target = debug/icuin53.dll
		
		icuuc53.commands = cp $$[QT_INSTALL_BINS]/icuuc53.dll debug
		icuuc53.target = debug/icuuc53.dll
		
		libgcc_s_dw2-1.commands = cp $$[QT_INSTALL_BINS]/libgcc_s_dw2-1.dll debug
		libgcc_s_dw2-1.target = debug/libgcc_s_dw2-1.dll
		
		libstdc++-6.commands = cp $$[QT_INSTALL_BINS]/libstdc++-6.dll debug
		libstdc++-6.target = debug/libstdc++-6.dll
		
		libwinpthread-1.commands = cp $$[QT_INSTALL_BINS]/libwinpthread-1.dll debug
		libwinpthread-1.target = debug/libwinpthread-1.dll
		
		libcurl.commands = cp $$[QT_INSTALL_BINS]/libcurl.dll debug
		libcurl.target = debug/libcurl.dll
		
		libidn-11.commands = cp $$[QT_INSTALL_BINS]/libidn-11.dll debug
		libidn-11.target = debug/libidn-11.dll
	
		Qt5Cored.commands = cp $$[QT_INSTALL_BINS]/Qt5Cored.dll debug
		Qt5Cored.target = debug/Qt5Cored.dll
		
		Qt5WebSocketsd.commands = cp $$[QT_INSTALL_BINS]/Qt5WebSocketsd.dll debug
		Qt5WebSocketsd.target = debug/Qt5WebSocketsd.dll
		
		Qt5Networkd.commands = cp $$[QT_INSTALL_BINS]/Qt5Networkd.dll debug
		Qt5Networkd.target = debug/Qt5Networkd.dll
		
		fmodex.commands = cp $$[QT_INSTALL_BINS]/fmodex.dll debug
		fmodex.target = debug/fmodex.dll

		Qt5Guid.commands = cp $$[QT_INSTALL_BINS]/Qt5Guid.dll debug
		Qt5Guid.target = debug/Qt5Guid.dll

		!isEmpty(_JENKINS){
			SDL2.commands = cp $$[QT_INSTALL_BINS]/SDL2.dll release
			SDL2.target = release/SDL2.dll

			SDL2_image.commands = cp $$[QT_INSTALL_BINS]/SDL2_image.dll release
			SDL2_image.target = release/SDL2_image.dll

			SDL2_ttf.commands = cp $$[QT_INSTALL_BINS]/SDL2_ttf.dll release
			SDL2_ttf.target = release/SDL2_ttf.dll

			QMAKE_EXTRA_TARGETS += SDL2 SDL2_image SDL2_ttf
			PRE_TARGETDEPS += release/SDL2.dll release/SDL2_image.dll release/SDL2_ttf.dll
		}
		
		QMAKE_EXTRA_TARGETS += icudt53 icuin53 icuuc53 libgcc_s_dw2-1 libstdc++-6 libwinpthread-1 libcurl libidn-11 Qt5Cored Qt5Guid Qt5WebSocketsd Qt5Networkd fmodex
		PRE_TARGETDEPS += debug/icudt53.dll debug/icuin53.dll debug/icuuc53.dll debug/libgcc_s_dw2-1.dll debug/libstdc++-6.dll debug/libwinpthread-1.dll debug/libcurl.dll debug/libidn-11.dll debug/Qt5Cored.dll debug/Qt5Guid.dll debug/Qt5WebSocketsd.dll debug/Qt5Networkd.dll debug/fmodex.dll
	}else:CONFIG(release, debug|release){
		icudt53.commands = cp $$[QT_INSTALL_BINS]/icudt53.dll release
		icudt53.target = release/icudt53.dll
		
		icuin53.commands = cp $$[QT_INSTALL_BINS]/icuin53.dll release
		icuin53.target = release/icuin53.dll
		
		icuuc53.commands = cp $$[QT_INSTALL_BINS]/icuuc53.dll release
		icuuc53.target = release/icuuc53.dll
		
		libgcc_s_dw2-1.commands = cp $$[QT_INSTALL_BINS]/libgcc_s_dw2-1.dll release
		libgcc_s_dw2-1.target = release/libgcc_s_dw2-1.dll
		
		libstdc++-6.commands = cp $$[QT_INSTALL_BINS]/libstdc++-6.dll release
		libstdc++-6.target = release/libstdc++-6.dll
		
		libwinpthread-1.commands = cp $$[QT_INSTALL_BINS]/libwinpthread-1.dll release
		libwinpthread-1.target = release/libwinpthread-1.dll
		
		libcurl.commands = cp $$[QT_INSTALL_BINS]/libcurl.dll release
		libcurl.target = release/libcurl.dll
		
		libidn-11.commands = cp $$[QT_INSTALL_BINS]/libidn-11.dll release
		libidn-11.target = release/libidn-11.dll
	
		Qt5Core.commands = cp $$[QT_INSTALL_BINS]/Qt5Core.dll release
		Qt5Core.target = release/Qt5Core.dll
		
		Qt5WebSockets.commands = cp $$[QT_INSTALL_BINS]/Qt5WebSockets.dll release
		Qt5WebSockets.target = release/Qt5WebSockets.dll
		
		Qt5Network.commands = cp $$[QT_INSTALL_BINS]/Qt5Network.dll release
		Qt5Network.target = release/Qt5Network.dll
		
		fmodex.commands = cp $$[QT_INSTALL_BINS]/fmodex.dll release
		fmodex.target = release/fmodex.dll

		Qt5Gui.commands = cp $$[QT_INSTALL_BINS]/Qt5Gui.dll release
		Qt5Gui.target = release/Qt5Gui.dll

		QMAKE_EXTRA_TARGETS += icudt53 icuin53 icuuc53 libgcc_s_dw2-1 libstdc++-6 libwinpthread-1 libcurl libidn-11 Qt5Core Qt5Gui Qt5WebSockets Qt5Network fmodex
		PRE_TARGETDEPS += release/icudt53.dll release/icuin53.dll release/icuuc53.dll release/libgcc_s_dw2-1.dll release/libstdc++-6.dll release/libwinpthread-1.dll release/libcurl.dll release/libidn-11.dll release/Qt5Core.dll release/Qt5Gui.dll release/Qt5WebSockets.dll release/Qt5Network.dll release/fmodex.dll

		!isEmpty(_JENKINS){
			SDL2.commands = cp $$[QT_INSTALL_BINS]/SDL2.dll release
			SDL2.target = release/SDL2.dll

			SDL2_image.commands = cp $$[QT_INSTALL_BINS]/SDL2_image.dll release
			SDL2_image.target = release/SDL2_image.dll

			SDL2_ttf.commands = cp $$[QT_INSTALL_BINS]/SDL2_ttf.dll release
			SDL2_ttf.target = release/SDL2_ttf.dll

			QMAKE_EXTRA_TARGETS += SDL2 SDL2_image SDL2_ttf
			PRE_TARGETDEPS += release/SDL2.dll release/SDL2_image.dll release/SDL2_ttf.dll
		}
	}else{
		error(Unknown set of dependencies.)
	}
}
