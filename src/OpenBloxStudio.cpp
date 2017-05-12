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
#include <QStringListModel>

#include "StudioWindow.h"

#include <instance/NetworkServer.h>
#include <instance/NetworkClient.h>

#define DARK_THEME_DEFAULT true
#ifndef _WIN32
#undef DARK_THEME_DEFAULT
#define DARK_THEME_DEFAULT false
#endif

void defaultValues(QSettings* settings){
	settings->setValue("first_run", false);
	settings->setValue("dark_theme", DARK_THEME_DEFAULT);
}

int main(int argc, char** argv){
	QApplication app(argc, argv);

	OB::Studio::StudioWindow::pathToStudioExecutable = std::string(argv[0]);

	app.setApplicationName("OpenBlox Studio");
	app.setApplicationVersion("0.1.1");
	app.setOrganizationDomain("openblox.org");
	app.setOrganizationName("OpenBlox");

	#ifdef _WIN32
	QSettings* settings = new QSettings("OpenBlox", "OpenBloxStudio");
	#elif defined(__linux)
	QSettings* settings = new QSettings("openblox-studio", "openblox-studio");
	#else
	QSettings* settings = new QSettings();
	#endif
	bool firstRun = settings->value("first_run", true).toBool();
	if(firstRun){
		defaultValues(settings);
	}

    bool useDarkTheme = settings->value("dark_theme", DARK_THEME_DEFAULT).toBool();

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

	QCommandLineOption newOpt("new", "Starts a new game instance on initialization.");
	parser.addOption(newOpt);

	QCommandLineOption serverOpt("server", "Starts a NetworkServer on initialization.");
	serverOpt.setDefaultValue("4490");
	parser.addOption(serverOpt);

	QCommandLineOption clientOpt("client", "Starts a NetworkClient on initialization.");
    clientOpt.setDefaultValue("localhost:4490");
	parser.addOption(clientOpt);
	
	parser.process(app);

    OB::OBEngine* eng = new OB::OBEngine();

	OB::Studio::StudioWindow* win = new OB::Studio::StudioWindow();
	win->settingsInst = settings;

	#ifdef WIN32
	settings->beginGroup("main_window");
	{
		if(settings->contains("geometry")){
			win->restoreGeometry(settings->value("geometry").toByteArray());
		}
		if(settings->contains("state")){
			win->restoreState(settings->value("state").toByteArray());
		}
	}
	settings->endGroup();
	#endif

	win->show();

	QComboBox* cmdBar = win->cmdBar;
	settings->beginGroup("command_history");
	{
		if(settings->contains("max_history")){
			cmdBar->setMaxCount(settings->value("max_history").toInt());
		}
		if(settings->contains("history")){
			cmdBar->addItems(settings->value("history").toStringList());
			cmdBar->setCurrentIndex(cmdBar->count());
			cmdBar->setCurrentText("");
		}
	}
	settings->endGroup();

	if(parser.isSet(newOpt) || parser.isSet(serverOpt) || parser.isSet(clientOpt)){
		win->newInstance();

		if(parser.isSet(serverOpt)){
			bool isInt;
		    int port = parser.value(serverOpt).toInt(&isInt);

			if(!isInt){
			    port = OB_STUDIO_DEFAULT_PORT;
			}

			shared_ptr<OB::Instance::DataModel> dm = eng->getDataModel();
			if(dm){
				shared_ptr<OB::Instance::NetworkServer> ns = dynamic_pointer_cast<OB::Instance::NetworkServer>(dm->GetService("NetworkServer"));
				if(ns){
					ns->Start(port);
				}
			}
		}

		if(parser.isSet(clientOpt)){
		    QStringList hostParts = parser.value(clientOpt).split(":");

			std::string hostName = "localhost";
		    int hostPort = OB_STUDIO_DEFAULT_PORT;
			
			if(hostParts.size() > 0){
				hostName = hostParts[0].toStdString();
				if(hostParts.size() > 1){
					bool isInt;
					hostPort = hostParts[1].toInt(&isInt);
					if(!isInt){
						hostPort = OB_STUDIO_DEFAULT_PORT;
					}
				}
			}

			shared_ptr<OB::Instance::DataModel> dm = eng->getDataModel();
			if(dm){
				shared_ptr<OB::Instance::NetworkClient> nc = dynamic_pointer_cast<OB::Instance::NetworkClient>(dm->GetService("NetworkClient"));
				if(nc){
					nc->Connect(hostName, hostPort);
				}
			}
		}
	}

	while(win->isVisible()){
		app.processEvents();
		if(eng->isRunning()){
			eng->tick();
		}
		QThread::msleep(10);
	}

	#ifdef WIN32
	settings->beginGroup("main_window");
	{
		settings->setValue("geometry", win->saveGeometry());
		settings->setValue("state", win->saveState());
	}
	settings.endGroup();
	#endif

	settings->beginGroup("command_history");
	{
		settings->setValue("max_history", cmdBar->maxCount());
	    settings->setValue("history", ((QStringListModel*)(cmdBar->model()))->stringList());
	}
	settings->endGroup();

	return 0;
}


