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

#include <openblox.h>

#include <QApplication>
#include <QSettings>
#include <QFile>
#include <QTextStream>
#include <QCommandLineParser>
#include <QThread>

#include "StudioWindow.h"

OB::Studio::StudioWindow* win = NULL;

void defaultValues(QSettings* settings){
	settings->setValue("first_run", false);
	settings->setValue("dark_theme", true);
}

int main(int argc, char** argv){
	QApplication app(argc, argv);

	app.setApplicationName("OpenBlox Studio");
	app.setApplicationVersion("0.1.1");
	app.setOrganizationDomain("openblox.org");
	app.setOrganizationName("OpenBlox");

	#ifdef _WIN32
	QSettings settings("OpenBlox", "OpenBloxStudio");
	#elif defined(__linux)
	QSettings settings("openblox-studio", "openblox-studio");
	#else
	QSettings settings;
	#endif
	bool firstRun = settings.value("first_run", true).toBool();
	if(firstRun){
		defaultValues(&settings);
	}

	bool useDarkTheme = settings.value("dark_theme", true).toBool();

	if(useDarkTheme){
		QFile f(":qdarkstyle/style.qss");
		if(f.exists()){
			f.open(QFile::ReadOnly | QFile::Text);
			QTextStream ts(&f);
			app.setStyleSheet(ts.readAll());
		}
	}

	QCommandLineParser parser;
	parser.setApplicationDescription("OpenBlox Studio");
	parser.addHelpOption();
	parser.addVersionOption();
	
	parser.process(app);

    OB::OBEngine* eng = new OB::OBEngine();

	win = new OB::Studio::StudioWindow();

	settings.beginGroup("main_window");
	{
		if(settings.contains("geometry")){
			win->restoreGeometry(settings.value("geometry").toByteArray());
		}
		if(settings.contains("state")){
			win->restoreState(settings.value("state").toByteArray());
		}
	}
	settings.endGroup();

	win->show();

	while(win->isVisible()){
		app.processEvents();
		QThread::msleep(10);
	}

	settings.beginGroup("main_window");
	{
		settings.setValue("geometry", win->saveGeometry());
		settings.setValue("state", win->saveState());
	}
	settings.endGroup();

	return 0;
}


