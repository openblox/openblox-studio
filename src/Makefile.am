bin_PROGRAMS = openblox-studio

# Sources for openblox
openblox_studio_SOURCES = OpenBloxStudio.cpp \
	StudioGLWidget.cpp \
	StudioWindow.cpp \
	InstanceTreeItem.cpp \
	qrc_resources.cpp
# Linker options
openblox_studio_LDADD = @LOPENBLOX_CFLAGS@ @LIRRLICHT_CFLAGS@ @LLUA_LIBS@ @LQT_LIBS@

# Compiler options. Here we are adding the include directory
# to be searched for headers included in the source code.
openblox_studio_CPPFLAGS = @LOPENBLOX_CPPFLAGS@ @LLUA_CFLAGS@ @LQT_CFLAGS@ -fPIC -std=c++11

qrc_%.cpp: %.qrc
	@RCC@ -o $@ $<
