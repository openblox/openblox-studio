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

#include <instance/DataModel.h>
/*#include <instance/LogService.h>*/

OB::Studio::StudioWindow* win = NULL;

/*
void handle_log_event(std::vector<ob_type::VarWrapper> evec){
	if(!win){
		LOGI("Lost output message");
		return;
	}
	if(evec.size() == 2){
		QString msg = reinterpret_cast<ob_type::StringWrapper*>(evec[0].wrapped)->val;
		ob_enum::LuaEnumItem* msgType = reinterpret_cast<ob_enum::LuaEnumItem*>(evec[1].wrapped);

		//ob_enum::LuaEnumItem* MessageOutput = ob_enum::LuaMessageType->getEnumItem((int)ob_enum::MessageType::MessageOutput);
		//ob_enum::LuaEnumItem* MessageInfo = ob_enum::LuaMessageType->getEnumItem((int)ob_enum::MessageType::MessageInfo);
		ob_enum::LuaEnumItem* MessageWarning = ob_enum::LuaMessageType->getEnumItem((int)ob_enum::MessageType::MessageWarning);
		ob_enum::LuaEnumItem* MessageError = ob_enum::LuaMessageType->getEnumItem((int)ob_enum::MessageType::MessageError);

		if(msgType == MessageError){
			win->output->append("<font color=\"#FF3300\">" + msg.toHtmlEscaped().replace('\n', "<br/>") + "</font><br/>");
		}else if(msgType == MessageWarning){
			win->output->append("<font color=\"#F26100\">" + msg.toHtmlEscaped().replace('\n', "<br/>") + "</font><br/>");
		}else{
			win->output->append(msg.toHtmlEscaped().replace('\n', "<br/>") + "<br/>");
		}
	}
}
*/

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

	shared_ptr<OB::Instance::DataModel> dm = eng->getDataModel();
	if(dm){
		/*OB::Instance::LogService* ls = dm->logService;
		if(ls){
			ls->MessageOut->Connect(handle_log_event);
			}*/
	}

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


