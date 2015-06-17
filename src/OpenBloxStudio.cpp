#include <OpenBlox.h>

#include <QApplication>

#include "StudioWindow.h"

#include <DataModel.h>
#include <LogService.h>

ob_studio::StudioWindow* win = NULL;

namespace OpenBlox{
#ifdef OPENBLOX_STUDIO
	void GetDisplaySize(int* width, int* height){
		if(!win){
			return;
		}
		ob_studio::StudioGLWidget* gl = win->glWidget;

		*width = gl->width();
		*height = gl->height();
	}
#endif

	bool mw = true;
}

void checkSDLError(int line = -1){
	#ifndef NDEBUG
	{
		const char *error = SDL_GetError();
		if(*error != '\0'){
			if(line != -1){
				LOGW("SDL Error: %s (Line: %i)", error, line);
			}else{
				LOGW("SDL Error: %s", error);
			}
			SDL_ClearError();
		}
	}
	#endif
}

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
			win->output->append("<font color=\"#FF3300\">" + msg + "</font><br/>");
		}else if(msgType == MessageWarning){
			win->output->append("<font color=\"#F26100\">" + msg + "</font><br/>");
		}else{
			win->output->append(msg + "<br/>");
		}
	}
}

void defaultValues(QSettings* settings){
	settings->setValue("first_run", false);
	settings->setValue("dark_theme", true);
}

int main(int argc, char** argv){
	QApplication app(argc, argv);

	app.setApplicationName("OpenBlox Studio");
	app.setApplicationVersion("0.1.1");
	app.setOrganizationDomain("myzillawr.tk");
	app.setOrganizationName("Myzilla Web Resources");

	#ifdef _WIN32
	QSettings settings("Myzilla Web Resources", "OpenBloxStudio");
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

	QCommandLineOption initScriptOption("script", "Script to run on initialization.", "null");
	parser.addOption(initScriptOption);

	parser.process(app);

	QString initScript = QString::null;

	if(parser.isSet(initScriptOption)){
		initScript = parser.value(initScriptOption).trimmed();
	}

	OpenBlox::OBGame* gameInst = new OpenBlox::OBGame(initScript, true);

	OpenBlox::static_init::execute();

	ob_instance::DataModel* dm = gameInst->getDataModel();
	if(dm){
		ob_instance::LogService* ls = dm->logService;
		if(ls){
			ls->MessageOut->Connect(handle_log_event);
		}
	}

	SDL_SetMainReady();
	if(SDL_Init(SDL_INIT_VIDEO) < 0){
		LOGE("%s: %s", "Unable to initialize SDL", "");
		SDL_Quit();
		return 1;
	}

	checkSDLError(__LINE__);

	if(TTF_Init() == -1){
		LOGE("%s: %s", "Unable to init SDL_TTF", TTF_GetError());
		SDL_Quit();
		return 1;
	}

	int flags = IMG_INIT_JPG | IMG_INIT_PNG;
	if((IMG_Init(flags) & flags) != flags){
		LOGE("Failed to load required JPEG and PNG support.");
		SDL_Quit();
		return 1;
	}

	win = new ob_studio::StudioWindow();

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

	QThread* taskThread = new OpenBlox::TaskThread();
	taskThread->start();

	while(win->isVisible()){
		SDL_Event event;
		while(SDL_PollEvent(&event)){
			OpenBlox::ProcessEvent(gameInst, event);
		}
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


