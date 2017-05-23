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
#include <QComboBox>
#include <QSettings>
#include <QListWidget>

#include "InstanceTree.h"
#include "StudioGLWidget.h"
#include "PropertyTreeWidget.h"

#define OB_STUDIO_DEFAULT_PORT 4490

namespace OB{
	namespace Studio{
	    class StudioWindow: public QMainWindow{
		  public:
			StudioWindow();

			static std::string pathToStudioExecutable;
			static StudioWindow* static_win;

			static QIcon getClassIcon(QString className);

			QTabWidget* tabWidget;
			QTextEdit* output;
			InstanceTree* explorer;
			PropertyTreeWidget* properties;
			QComboBox* cmdBar;
			QListWidget* basicObjects;

			QMenu* explorerCtxMenu;

			QSettings* settingsInst;

			//Actions
			QAction* saveAction;
			QAction* saveAsAction;

			QAction* deleteAction;
			
			QAction* ungroupAct;
			QAction* groupAct;

			void updateSelectionFromLua(OBEngine* eng);
			void update_toolbar_usability();
			void populateBasicObjects();
			
			OBEngine* getCurrentEngine();
			StudioGLWidget* getCurrentGLWidget(OBEngine* eng);
			void tickEngines();

			void sendOutput(QString str);
			void sendOutput(QString str, QColor col);

		    public slots:
				void about();
				void showSettings();
				void newInstance();
				void closeStudio();
				void commandBarReturn();
				void selectionChanged();
				void insertInstance();

				void explorerContextMenu(const QPoint &pos);

				//Action handlers
				void deleteSelection();
				void groupSelection();
				void ungroupSelection();

				void saveAct();
				void saveAsAct();
		};
	}
}
#endif
