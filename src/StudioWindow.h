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
			QTreeWidget* explorer;
			QComboBox* cmdBar;

		private slots:
			void about(bool checked);
			void showSettings(bool checked);
			void commandBarReturn();
	};
}
#endif
