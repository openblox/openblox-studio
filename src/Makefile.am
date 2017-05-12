bin_PROGRAMS = openblox-studio

# Sources for openblox
openblox_studio_SOURCES = OpenBloxStudio.cpp \
	StudioGLWidget.cpp \
	StudioWindow.cpp \
	InstanceTree.cpp \
	InstanceTreeItem.cpp \
	PropertyItem.cpp \
	PropertyTreeItemDelegate.cpp \
	PropertyTreeWidget.cpp \
	ConfigDialog.cpp \
	ConfigPage.cpp \
	ColorDialog.cpp \
	qrc_resources.cpp
# Linker options
openblox_studio_LDADD = $(LOPENBLOX_LIBS) $(LIRRLICHT_LIBS) $(LBULLET_LIBS) $(LQT_LIBS)

# Compiler options. Here we are adding the include directory
# to be searched for headers included in the source code.
openblox_studio_CPPFLAGS = $(LOPENBLOX_CPPFLAGS) $(LBULLET_CFLAGS) $(LQT_CFLAGS) -fPIC -std=c++11

qrc_%.cpp: %.qrc
	$(RCC) -o $@ $<
