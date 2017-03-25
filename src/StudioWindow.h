/*
 * Copyright (C) 2017 John M. Harris, Jr. <johnmh@openblox.org>
 *
 * This file is part of OpenBlox Studio.
 *
 * OpenBlox Studio is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * OpenBlox Studio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the Lesser GNU General Public License
 * along with OpenBlox Studio.	 If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef OB_STUDIO_STUDIOWINDOW_H_
#define OB_STUDIO_STUDIOWINDOW_H_

#include <QMainWindow>

#include <QTabWidget>
#include <QTextEdit>
#include <QTreeWidget>
#include <QComboBox>

#include "StudioGLWidget.h"

namespace OB{
	namespace Studio{
		class StudioWindow: public QMainWindow{
		  public:
			StudioWindow();

			QTabWidget* tabWidget;
			StudioGLWidget* glWidget;
			QTextEdit* output;
			QTreeWidget* explorer;
			QComboBox* cmdBar;

			void initGL();

			private slots:
				void about();
				void showSettings();
				void newInstance();
				void closeStudio();
				void commandBarReturn();
		};
	}
}
#endif
