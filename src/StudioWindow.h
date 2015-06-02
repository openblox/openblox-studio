#ifndef STUDIOWINDOW_H_
#define STUDIOWINDOW_H_

#include <QMainWindow>

#include "StudioGLWidget.h"

namespace ob_studio{
	class StudioWindow: public QMainWindow{
		public:
			StudioWindow();

			StudioGLWidget* glWidget;
			QTextEdit* output;

		private slots:
			void about(bool checked);
	};
}
#endif
