#include "StudioWindow.h"

namespace ob_studio{
	StudioWindow::StudioWindow(){
		setCentralWidget(new StudioGLWidget());
	}
}
