#include <OpenBlox.h>

#include <QApplication>

#include "StudioWindow.h"

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

int main(int argc, char** argv){
	QApplication app(argc, argv);

	app.setApplicationName("OpenBlox Studio");
	app.setApplicationVersion("0.1.1");
	app.setOrganizationDomain("myzillawr.tk");
	app.setOrganizationName("Myzilla Web Resources");
	app.setAttribute(Qt::AA_NativeWindows, true);

	QCommandLineParser parser;
	parser.setApplicationDescription("OpenBlox Studio");
	parser.addHelpOption();
	parser.addVersionOption();

	QCommandLineOption initScriptOption("script", "Script to run on initialization.", "res://init.lua");
	parser.addOption(initScriptOption);

	parser.process(app);

	QString initScript = QString::null;

	if(parser.isSet(initScriptOption)){
		initScript = parser.value(initScriptOption).trimmed();
	}

	OpenBlox::OBGame* gameInst = new OpenBlox::OBGame(initScript, true);

	OpenBlox::static_init::execute();

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

	QThread* taskThread = new OpenBlox::TaskThread();
	taskThread->start();

	ob_studio::StudioWindow win;

	win.show();

	while(win.isVisible()){
		SDL_Event event;
		while(SDL_PollEvent(&event)){
			OpenBlox::ProcessEvent(gameInst, event);
		}
		app.processEvents();
		//gameInst->render();
		//SDL_GL_SwapWindow(OpenBlox::mw);
		QThread::msleep(10);
	}
}


