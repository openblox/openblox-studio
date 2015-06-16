#include "StudioWindow.h"

#include "InstanceTreeItem.h"

namespace ob_studio{
	QMap<ob_instance::Instance*, InstanceTreeItem*> treeItemMap;
	QMap<QString, QIcon> classIconMap;

	QIcon getClassIcon(QString className){
		if(classIconMap.contains(className)){
			return classIconMap[className];
		}
		QIcon ico = QIcon(":rc/class_icons/_unknown.png");
		if(QFile(":rc/class_icons/" + className + ".png").exists()){
			ico = QIcon(":rc/class_icons/" + className + ".png");
		}
		classIconMap[className] = ico;
		return ico;
	}

	void addChildrenOfInstance(QTreeWidgetItem* parentItem, ob_instance::Instance* inst);

	void addChildOfInstance(QTreeWidgetItem* parentItem, ob_instance::Instance* kid){
		if(!parentItem || !kid){
			return;
		}

		InstanceTreeItem* kidItem = new InstanceTreeItem(kid);
		treeItemMap[kid] = kidItem;
		kidItem->setIcon(0, getClassIcon(kid->getClassName()));

		kid->Changed->Connect([=](std::vector<ob_type::VarWrapper> evec){
			Q_UNUSED(evec)

			kidItem->setText(0, kid->getName());
		});
		kid->ChildAdded->Connect([=](std::vector<ob_type::VarWrapper> evec){
			if(evec.size() == 1){
				ob_instance::Instance* newGuy = reinterpret_cast<ob_instance::Instance*>(evec[0].wrapped);
				if(treeItemMap.contains(newGuy)){
					kidItem->addChild(treeItemMap[newGuy]);
				}else{
					addChildOfInstance(kidItem, newGuy);
				}
			}
		});
		kid->ChildRemoved->Connect([=](std::vector<ob_type::VarWrapper> evec){
			if(evec.size() == 1){
				ob_instance::Instance* newGuy = reinterpret_cast<ob_instance::Instance*>(evec[0].wrapped);
				if(treeItemMap.contains(newGuy)){
					kidItem->removeChild(treeItemMap[newGuy]);
				}
			}
		});

		addChildrenOfInstance(kidItem, kid);

		parentItem->addChild(kidItem);
	}

	void addChildrenOfInstance(QTreeWidgetItem* parentItem, ob_instance::Instance* inst){
		if(!parentItem || !inst){
			return;
		}
		std::vector<ob_instance::Instance*> kids = inst->GetChildren();
		for(std::vector<ob_instance::Instance*>::size_type i = 0; i < kids.size(); i++){
			ob_instance::Instance* kid = kids[i];
			if(kid){
				addChildOfInstance(parentItem, kid);
			}
		}
	}

	StudioWindow::StudioWindow(){
		glWidget = new StudioGLWidget();
		setCentralWidget(glWidget);

		//Menus
		QMenu* fileMenu = menuBar()->addMenu("File");
		fileMenu->addAction("New");
		fileMenu->addAction("Open...");
		fileMenu->addAction("Close")->setEnabled(false);

		fileMenu->addSeparator();

		fileMenu->addAction("Save")->setEnabled(false);
		fileMenu->addAction("Save As...")->setEnabled(false);
		fileMenu->addSeparator();
		fileMenu->addAction("Exit");

		QMenu* editMenu = menuBar()->addMenu("Edit");
		editMenu->addAction("Undo");
		editMenu->addAction("Redo");

		editMenu->addSeparator();

		editMenu->addAction("Cut")->setEnabled(false);
		editMenu->addAction("Copy")->setEnabled(false);
		editMenu->addAction("Paste")->setEnabled(false);
		editMenu->addAction("Duplicate")->setEnabled(false);

		editMenu->addSeparator();

		editMenu->addAction("Settings");

		QMenu* viewMenu = menuBar()->addMenu("View");

		menuBar()->addSeparator();

		QAction* aboutAct = new QAction("About", this);
		aboutAct->setStatusTip("About OpenBlox Studio");
		connect(aboutAct, &QAction::triggered, this, &StudioWindow::about);

		QMenu* helpMenu = menuBar()->addMenu("Help");
		helpMenu->addAction(aboutAct);

		//Dock Components

		setCorner(Qt::TopLeftCorner, Qt::LeftDockWidgetArea);
		setCorner(Qt::TopRightCorner, Qt::RightDockWidgetArea);

		//Output
		QDockWidget* dock = new QDockWidget(tr("Output"), this);
		dock->setObjectName("studio_output");
		dock->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable);

		dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::BottomDockWidgetArea);

		output = new QTextEdit();
		output->setReadOnly(true);

		dock->setWidget(output);
		addDockWidget(Qt::BottomDockWidgetArea, dock);

		viewMenu->addAction(dock->toggleViewAction());

		//Explorer
		dock = new QDockWidget(tr("Explorer"), this);
		dock->setObjectName("studio_explorer");
		dock->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable);

		dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::BottomDockWidgetArea);

		explorer = new QTreeWidget();
		explorer->setSelectionMode(QAbstractItemView::ExtendedSelection);
		explorer->setSelectionBehavior(QAbstractItemView::SelectItems);
		explorer->header()->close();

		QTreeWidgetItem* rootItem = explorer->invisibleRootItem();

		OpenBlox::OBGame* game = OpenBlox::OBGame::getInstance();
		if(game){
			addChildrenOfInstance(rootItem, (ob_instance::Instance*)game->getDataModel());
		}

		dock->setWidget(explorer);
		addDockWidget(Qt::RightDockWidgetArea, dock);

		viewMenu->addAction(dock->toggleViewAction());

		//Last View Menu things
		viewMenu->addSeparator();

		QMenu* viewToolbarsMenu = viewMenu->addMenu("Toolbars");

		statusBar();

		QToolBar* commandBar = new QToolBar("Command");
		commandBar->setObjectName("studio_command_bar");
		commandBar->setAllowedAreas(Qt::TopToolBarArea | Qt::BottomToolBarArea);
		viewToolbarsMenu->addAction(commandBar->toggleViewAction());

		//START COMMAND BAR
		cmdBar = new QComboBox();
		cmdBar->setEditable(true);

		//Setup QLineEdit in QComboBox
		QLineEdit* cmdEdit = cmdBar->lineEdit();
		cmdEdit->setPlaceholderText("Run a command");
		connect(cmdEdit, &QLineEdit::returnPressed, this, &StudioWindow::commandBarReturn);

		cmdBar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
		commandBar->setBaseSize(300, 20);
		commandBar->setMinimumWidth(300);

		commandBar->addWidget(cmdBar);

		addToolBar(Qt::BottomToolBarArea, commandBar);
		//END COMMAND BAR
	}

	void StudioWindow::about(bool checked){
		Q_UNUSED(checked)

		QMessageBox::about(this, tr("About OpenBlox Studio"),
		tr("<b>OpenBlox Studio</b><br/>"
		"Version 0.1.1<br/>"
		"OpenBlox"));
	}

	void StudioWindow::commandBarReturn(){
		QLineEdit* cmdEdit = cmdBar->lineEdit();
		QString text = cmdEdit->text();

		output->append("> " + text + "<br/>");

		OpenBlox::OBGame* game = OpenBlox::OBGame::getInstance();
		if(!game){
			return;
		}

		lua_State* L = game->newLuaState();
		lua_resume(L, NULL, 0);

		int s = luaL_loadstring(L, text.toStdString().c_str());
		if(s == 0){
			s = lua_pcall(L, 0, LUA_MULTRET, 0);
		}

		if(s != 0){
			game->handle_lua_errors(L);
		}

		lua_pop(game->getGlobalLuaState(), 1);//Pop that state off the global state.
		LOGI("Returned from global");
	}
}
