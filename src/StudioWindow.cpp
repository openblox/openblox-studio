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

#include "StudioWindow.h"

#include <QtWidgets>

//Studio Widgets
#include "InstanceTreeItem.h"
#include "ConfigDialog.h"

// OpenBlox Engine
#include <openblox.h>
#include <instance/Instance.h>
#include <instance/DataModel.h>
#include <instance/Workspace.h>
#include <instance/LogService.h>
#include <type/Event.h>
#include <type/Enum.h>

#ifdef _WIN32
#include "windows.h"
#include "shellapi.h"
#endif

namespace OB{
	namespace Studio{
		std::string StudioWindow::pathToStudioExecutable = "";
	    StudioWindow* StudioWindow::static_win = NULL;
		
	    void handle_log_event(std::vector<shared_ptr<OB::Type::VarWrapper>> evec, void* ud){
			//Temporary
			QColor errorCol(255, 51, 0);
			QColor warnCol(242, 97, 0);

			StudioWindow* win = StudioWindow::static_win;
			
			if(evec.size() == 2){
				QString msg = evec.at(0)->asString().c_str();
				shared_ptr<OB::Type::LuaEnumItem> msgType = dynamic_pointer_cast<OB::Type::LuaEnumItem>(evec.at(1)->asType());

				if(msgType->getValue() == (int)OB::Enum::MessageType::MessageError){
				    win->sendOutput(msg, errorCol);
				}else if(msgType->getValue() == (int)OB::Enum::MessageType::MessageWarning){
					win->sendOutput(msg, warnCol);
				}else{
				    win->sendOutput(msg);
				}
			}
		}
		
		QMap<shared_ptr<Instance::Instance>, InstanceTreeItem*> treeItemMap;
		QMap<QString, QIcon> classIconMap;

		QIcon getClassIcon(QString className){
		    if(classIconMap.contains(className)){
				return classIconMap[className];
			}
		    
			QIcon ico;

			if(className.isEmpty()){
				return ico;
			}

			QString icop = ":rc/class_icons/" + className + ".png";
			
			if(QFile(icop).exists()){
				ico = QIcon(icop);
			}else{
				std::string parentClassStr = ClassFactory::getParentClassName(className.toStdString());
			    ico = getClassIcon(QString(parentClassStr.c_str()));
			}
			
			classIconMap[className] = ico;
			return ico;
		}

		void addChildrenOfInstance(QTreeWidgetItem* parentItem, shared_ptr<Instance::Instance> inst);
		void addChildOfInstance(QTreeWidgetItem* parentItem, shared_ptr<Instance::Instance> kid);
		
		void instance_changed_evt(std::vector<shared_ptr<Type::VarWrapper>> evec, void* ud){
			if(!ud){
				return;
			}
			
		    InstanceTreeItem* kidItem = (InstanceTreeItem*)ud;
			shared_ptr<Instance::Instance> kid = kidItem->GetInstance();
			if(!kid){
				return;
			}

			std::string prop = evec.at(0)->asString();

			if(StudioWindow::static_win){
				std::vector<shared_ptr<Instance::Instance>> selection = StudioWindow::static_win->selectedInstances;

				if(!selection.empty()){
					if(std::find(selection.begin(), selection.end(), kid) != selection.end()){
						StudioWindow::static_win->properties->updateValue(prop);
					}
				}
				
			}

			if(prop == "Name"){
			    const QSignalBlocker sigBlock(kidItem->treeWidget());
				kidItem->setText(0, QString(kid->getName().c_str()));
				return;
			}
			if(prop == "Parent" || prop == "ParentLocked"){
				kidItem->updateFlags();
				return;
			}
		}

		void instance_child_added_evt(std::vector<shared_ptr<Type::VarWrapper>> evec, void* ud){
		    InstanceTreeItem* kidItem = (InstanceTreeItem*)ud;
			
		    if(evec.size() == 1){
			    shared_ptr<Instance::Instance> newGuy = dynamic_pointer_cast<Instance::Instance>(evec[0]->asInstance());
				if(treeItemMap.contains(newGuy)){
					kidItem->addChild(treeItemMap[newGuy]);
				}else{
					addChildOfInstance(kidItem, newGuy);
				}
			}
		}

		void instance_child_removed_evt(std::vector<shared_ptr<Type::VarWrapper>> evec, void* ud){
			InstanceTreeItem* kidItem = (InstanceTreeItem*)ud;
			
		    if(evec.size() == 1){
			    shared_ptr<Instance::Instance> newGuy = dynamic_pointer_cast<Instance::Instance>(evec[0]->asInstance());
				if(treeItemMap.contains(newGuy)){
					kidItem->removeChild(treeItemMap[newGuy]);
				}
			}
		}

		void addChildOfInstance(QTreeWidgetItem* parentItem, shared_ptr<Instance::Instance> kid){
			if(!parentItem || !kid){
				return;
			}

			InstanceTreeItem* kidItem = new InstanceTreeItem(kid);
			treeItemMap[kid] = kidItem;
			kidItem->setIcon(0, getClassIcon(QString(kid->getClassName().c_str())));

		    kid->Changed->Connect(instance_changed_evt, kidItem);
			kid->ChildAdded->Connect(instance_child_added_evt, kidItem);
			kid->ChildRemoved->Connect(instance_child_removed_evt, kidItem);

			addChildrenOfInstance(kidItem, kid);

			parentItem->addChild(kidItem);
		}

		void addChildrenOfInstance(QTreeWidgetItem* parentItem, shared_ptr<Instance::Instance> inst){
			if(!parentItem || !inst){
				return;
			}
			std::vector<shared_ptr<Instance::Instance>> kids = inst->GetChildren();
			for(std::vector<shared_ptr<Instance::Instance>>::size_type i = 0; i < kids.size(); i++){
			    shared_ptr<Instance::Instance> kid = kids[i];
				if(kid){
					addChildOfInstance(parentItem, kid);
				}
			}
		}
		
		void dm_changed_evt(std::vector<shared_ptr<Type::VarWrapper>> evec, void* ud){
			StudioWindow* sw = (StudioWindow*)ud;
			if(!sw){
				return;
			}
			
		    OBEngine* eng = OBEngine::getInstance();
			if(eng){
			    shared_ptr<Instance::DataModel> dm = eng->getDataModel();
				if(dm){
					if(sw->tabWidget && sw->glWidget){
						int studioWidgetIdx = sw->tabWidget->indexOf(sw->glWidget);
						sw->tabWidget->setTabText(studioWidgetIdx, QString(dm->getName().c_str()));
					}
				}
			}
		}

		void addDM(QTreeWidgetItem* parentItem, shared_ptr<Instance::Instance> inst, StudioWindow* sw){			
			if(!parentItem || !inst){
				return;
			}
			std::vector<shared_ptr<Instance::Instance>> kids = inst->GetChildren();
			for(std::vector<shared_ptr<Instance::Instance>>::size_type i = 0; i < kids.size(); i++){
				shared_ptr<Instance::Instance> kid = kids[i];
				if(kid){
					addChildOfInstance(parentItem, kid);
				}
			}

			inst->ChildAdded->Connect(instance_child_added_evt, parentItem);
			inst->ChildRemoved->Connect(instance_child_removed_evt, parentItem);
			inst->Changed->Connect(dm_changed_evt, sw);
		}

		StudioWindow::StudioWindow(){
			static_win = this;
			glWidget = NULL;

			tabWidget = new QTabWidget();
			tabWidget->setMinimumSize(320, 240);
			tabWidget->setTabsClosable(true);
			
			setCentralWidget(tabWidget);

			//Menus
			QMenu* fileMenu = menuBar()->addMenu("File");
			
			QAction* newAction = fileMenu->addAction("New");
			newAction->setIcon(QIcon::fromTheme("document-new"));
			newAction->setShortcut(QKeySequence::New);
			connect(newAction, &QAction::triggered, this, &StudioWindow::newInstance);
			
			QAction* openAction = fileMenu->addAction("Open");
			openAction->setIcon(QIcon::fromTheme("document-open"));
			openAction->setShortcut(QKeySequence::Open);

			fileMenu->addSeparator();

			QAction* saveAction = fileMenu->addAction("Save");
			saveAction->setEnabled(false);
			saveAction->setIcon(QIcon::fromTheme("document-save"));
			saveAction->setShortcut(QKeySequence::Save);
			
			QAction* saveAsAction = fileMenu->addAction("Save As");
			saveAsAction->setEnabled(false);
			saveAsAction->setIcon(QIcon::fromTheme("document-save-as"));
			saveAsAction->setShortcut(QKeySequence::SaveAs);
			
			fileMenu->addSeparator();
			
			QAction* closeAction = fileMenu->addAction("Close");
			closeAction->setIcon(QIcon::fromTheme("window-close"));
			closeAction->setShortcut(QKeySequence::Quit);
			connect(closeAction, &QAction::triggered, this, &StudioWindow::closeStudio);

		    QMenu* editMenu = menuBar()->addMenu("Edit");
		    explorerCtxMenu = new QMenu();
			QAction* undoAction = editMenu->addAction("Undo");
			undoAction->setIcon(QIcon::fromTheme("edit-undo"));
			undoAction->setEnabled(false);
			undoAction->setShortcut(QKeySequence::Undo);
			
			QAction* redoAction = editMenu->addAction("Redo");
			redoAction->setIcon(QIcon::fromTheme("edit-redo"));
			redoAction->setEnabled(false);
			redoAction->setShortcut(QKeySequence::Redo);

			editMenu->addSeparator();

			QAction* cutAction = editMenu->addAction("Cut");
			explorerCtxMenu->addAction(cutAction);
			cutAction->setIcon(QIcon::fromTheme("edit-cut"));
			cutAction->setEnabled(false);
			cutAction->setShortcut(QKeySequence::Cut);
			
			QAction* copyAction = editMenu->addAction("Copy");
			explorerCtxMenu->addAction(copyAction);
			copyAction->setIcon(QIcon::fromTheme("edit-copy"));
			copyAction->setEnabled(false);
			copyAction->setShortcut(QKeySequence::Copy);

			QAction* pasteAction = editMenu->addAction("Paste");
			explorerCtxMenu->addAction(pasteAction);
			pasteAction->setIcon(QIcon::fromTheme("edit-paste"));
			pasteAction->setEnabled(false);
			pasteAction->setShortcut(QKeySequence::Paste);

		    deleteAction = editMenu->addAction("Delete");
			explorerCtxMenu->addAction(deleteAction);
		    deleteAction->setIcon(QIcon::fromTheme("edit-delete"));
			deleteAction->setEnabled(false);
			deleteAction->setShortcut(QKeySequence::Delete);
			connect(deleteAction, &QAction::triggered, this, &StudioWindow::deleteSelection);

			editMenu->addSeparator();

			QAction* settingsAct = editMenu->addAction("Settings");
			settingsAct->setIcon(QIcon::fromTheme("configure-shortcuts", QIcon::fromTheme("configure", QIcon::fromTheme("document-properties"))));
			settingsAct->setShortcut(QKeySequence::Preferences);
			connect(settingsAct, &QAction::triggered, this, &StudioWindow::showSettings);

			QMenu* viewMenu = menuBar()->addMenu("View");

			menuBar()->addSeparator();

			QMenu* helpMenu = menuBar()->addMenu("Help");

			QAction* aboutAct = helpMenu->addAction("About");
			aboutAct->setStatusTip("About OpenBlox Studio");
			aboutAct->setIcon(QIcon::fromTheme("help-about"));
			connect(aboutAct, &QAction::triggered, this, &StudioWindow::about);

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

			explorer = new InstanceTree();
			explorer->setMinimumSize(100, 100);
		    explorer->setContextMenuPolicy(Qt::CustomContextMenu);
			connect(explorer, &QWidget::customContextMenuRequested, this, &StudioWindow::explorerContextMenu);
			connect(explorer, &QTreeWidget::itemSelectionChanged, this, &StudioWindow::selectionChanged);

			dock->setWidget(explorer);
			addDockWidget(Qt::RightDockWidgetArea, dock);

			viewMenu->addAction(dock->toggleViewAction());

			//Properties
			dock = new QDockWidget(tr("Properties"), this);
			dock->setObjectName("studio_properties");
			dock->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable);

			dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::BottomDockWidgetArea);

			properties = new PropertyTreeWidget();
			properties->setMinimumSize(100, 100);

			dock->setWidget(properties);
			addDockWidget(Qt::RightDockWidgetArea, dock);

			viewMenu->addAction(dock->toggleViewAction());

			//Basic Objects
			dock = new QDockWidget(tr("Basic Objects"), this);
			dock->setObjectName("studio_basic_objects");
			dock->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable);

			dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::BottomDockWidgetArea);

			basicObjects = new QListWidget();
			basicObjects->setSortingEnabled(true);
			basicObjects->setDragEnabled(false);
			basicObjects->setSelectionBehavior(QAbstractItemView::SelectItems);
			basicObjects->setSelectionMode(QAbstractItemView::SingleSelection);
			connect(basicObjects, &QListWidget::itemActivated, this, &StudioWindow::insertInstance);

			dock->setWidget(basicObjects);
			addDockWidget(Qt::LeftDockWidgetArea, dock);

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
			cmdBar->setMaxCount(50);
			cmdBar->setEditable(true);
			cmdBar->setInsertPolicy(QComboBox::InsertAtTop);
			cmdBar->setDuplicatesEnabled(true);

			cmdBar->setModel(new QStringListModel());

			//Setup QLineEdit in QComboBox
			QLineEdit* cmdEdit = cmdBar->lineEdit();
			cmdEdit->setPlaceholderText("Run a command");
			connect(cmdEdit, &QLineEdit::returnPressed, this, &StudioWindow::commandBarReturn);
			cmdEdit->setDisabled(true);

			cmdBar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
			commandBar->setBaseSize(300, 20);
			commandBar->setMinimumWidth(300);

			commandBar->addWidget(cmdBar);

			addToolBar(Qt::BottomToolBarArea, commandBar);
			//END COMMAND BAR

			//Begin Model toolbar
			QToolBar* modelToolbar = new QToolBar("Model");
			modelToolbar->setObjectName("studio_model_toolbar");
			modelToolbar->setAllowedAreas(Qt::TopToolBarArea | Qt::BottomToolBarArea);
			viewToolbarsMenu->addAction(modelToolbar->toggleViewAction());
			
		    groupAct = modelToolbar->addAction("Group");
			groupAct->setIcon(QIcon::fromTheme("object-group", QIcon(":studio_icon/icon_group.png")));
			groupAct->setEnabled(false);
			connect(groupAct, &QAction::triggered, this, &StudioWindow::groupSelection);

			ungroupAct = modelToolbar->addAction("Ungroup");
			ungroupAct->setIcon(QIcon::fromTheme("object-ungroup", QIcon(":studio_icon/icon_ungroup.png")));
			ungroupAct->setEnabled(false);
			connect(ungroupAct, &QAction::triggered, this, &StudioWindow::ungroupSelection);

			addToolBar(Qt::TopToolBarArea, modelToolbar);
			//End Model toolbar
		}

		void StudioWindow::about(){
			QMessageBox::about(this, tr("About OpenBlox Studio"),
							   tr("<b>OpenBlox Studio</b><br/>"
								  "Version 0.1.1<br/>"
								  "OpenBlox"));
		}

		void StudioWindow::showSettings(){
		    ConfigDialog* cfg_d = new ConfigDialog(this);
			cfg_d->setModal(true);
			cfg_d->show();
		}

		void StudioWindow::closeStudio(){
		    close();
		}

		void StudioWindow::newInstance(){
			if(glWidget){
				if(pathToStudioExecutable.length() > 0){
					#ifdef _WIN32
					//Windows has to be special

					STARTUPINFO si;
					
					CreateProcess(NULL,
								  (LPSTR)pathToStudioExecutable.c_str(),
								  NULL,
								  NULL,
								  FALSE,
								  0,
								  NULL,
								  NULL,
								  &si,
								  NULL);
					#else
					//Sane systems
					pid_t fr = fork();
					if(fr == 0){
						execlp(pathToStudioExecutable.c_str(), pathToStudioExecutable.c_str(),
							   "--new",
							   NULL);
					}
					#endif
				}
			}else{
				glWidget = new StudioGLWidget();
				
			    int tabIdx = tabWidget->addTab(glWidget, "Game");
			    QTabBar* tabBar = tabWidget->tabBar();
				if(tabBar){
					QWidget* tabBtn = tabBar->tabButton(tabIdx, QTabBar::RightSide);
					if(tabBtn){
						tabBtn->resize(0, 0);
						tabBtn->setVisible(false);
					}
				}

				glWidget->init();
				initGL();
				
				cmdBar->lineEdit()->setDisabled(false);
			}
		}

		void StudioWindow::commandBarReturn(){
			QLineEdit* cmdEdit = cmdBar->lineEdit();
			QString text = cmdEdit->text();

		    sendOutput("> " + text);

		    OB::OBEngine* eng = OB::OBEngine::getInstance();
			if(!eng){
				return;
			}

			lua_State* gL = eng->getGlobalLuaState();
			if(!gL){
				//Prevents segfaults when commands are run before initialization
				return;
			}
			
			lua_State* L = Lua::initThread(gL);

			int s = luaL_loadstring(L, text.toStdString().c_str());
			if(s == 0){
				s = lua_resume(L, NULL, 0);
			}

			if(s != 0 && s != LUA_YIELD){
			    Lua::handle_errors(L);
				Lua::close_state(L);
			}
			if(s == LUA_OK){
				Lua::close_state(L);
			}
		}

		void StudioWindow::selectionChanged(){
			QList<QTreeWidgetItem*> selectedItems = explorer->selectedItems();

			selectedInstances.clear();

			for(int i = 0; i < selectedItems.size(); i++){
				InstanceTreeItem* srcItem = dynamic_cast<InstanceTreeItem*>(selectedItems[i]);
				if(srcItem){
					shared_ptr<Instance::Instance> instPtr = srcItem->GetInstance();
					if(instPtr){
					    selectedInstances.push_back(instPtr);
					}
				}
			}
			
			properties->updateSelection(selectedInstances);

			const int numSelected = selectedInstances.size();
			if(numSelected > 0){
				deleteAction->setEnabled(true);

				if(numSelected > 1){
					basicObjects->setEnabled(false);

				    bool anyParentLocked = false;
					shared_ptr<Instance::Instance> sharedParent = NULL;
					for(int i = 0; i < numSelected; i++){
						shared_ptr<Instance::Instance> selected_inst = selectedInstances.at(i);
						if(selected_inst){
							if(selected_inst->ParentLocked){
								anyParentLocked = true;
								break;
							}
							if(!sharedParent){
								sharedParent = selected_inst->getParent();
							}
							if(selected_inst->getParent() != sharedParent){
								sharedParent = NULL;
								break;
							}
						}
					}
					
				    ungroupAct->setEnabled(false);

					if(anyParentLocked || !sharedParent){
						groupAct->setEnabled(false);
					}else{
						groupAct->setEnabled(true);
					}
				}else{
					basicObjects->setEnabled(true);
					groupAct->setEnabled(false);
					ungroupAct->setEnabled(false);

				    shared_ptr<Instance::Instance> selected_inst = selectedInstances.at(0);
					if(selected_inst){
					    shared_ptr<Instance::Model> selected_model = dynamic_pointer_cast<Instance::Model>(selected_inst);
						if(selected_model){
							if(selected_model->GetChildren().size() > 0){
								ungroupAct->setEnabled(true);
							}
						}
					}
				}
			}else{
				deleteAction->setEnabled(false);
				basicObjects->setEnabled(true);
			}
		}

		void StudioWindow::updateSelectionFromLua(){
			const QSignalBlocker sigBlock(explorer);

			explorer->clearSelection();
			for(int i = 0; i < selectedInstances.size(); i++){
				shared_ptr<Instance::Instance> inst = selectedInstances.at(i);
				if(inst){
					InstanceTreeItem* ti = treeItemMap[inst];
					if(ti){
						ti->setSelected(true);
					}else{
						puts("No ti");
					}
				}
			}
		}

		void StudioWindow::initGL(){
			QTreeWidgetItem* rootItem = explorer->invisibleRootItem();

		    OB::OBEngine* eng = OB::OBEngine::getInstance();
			if(eng){
				eng->init();
				shared_ptr<OB::Instance::DataModel> dm = eng->getDataModel();
				if(dm){
					shared_ptr<OB::Instance::LogService> ls = dm->getLogService();
					if(ls){
						ls->getMessageOut()->Connect(handle_log_event, NULL);
					}
					addDM(rootItem, dynamic_pointer_cast<Instance::Instance>(dm), this);
				}
			}

			std::vector<std::string> registeredInstances = ClassFactory::getRegisteredClasses();
			for(int i = 0; i < registeredInstances.size(); i++){
				std::string classstr = registeredInstances.at(i);
				bool doShow = ClassFactory::canCreate(classstr);
				if(doShow){
					QString classQstr = QString(classstr.c_str());
					QListWidgetItem* wi = new QListWidgetItem(getClassIcon(classQstr), classQstr);
					basicObjects->addItem(wi);
				}
			}
		}

		// Do I think the use of HTML here is horrible? Yes.
		// Am I going to do something about it in the near future? Probably not.
		void StudioWindow::sendOutput(QString msg){
			if(output){
				output->append(msg.toHtmlEscaped().replace('\n', "<br/>") + "<br/>");
			}
		}

		void StudioWindow::sendOutput(QString msg, QColor col){
		    if(output){
				output->append("<font color=\"" + col.name() + "\">" + msg.toHtmlEscaped().replace('\n', "<br/>") + "</font><br/>");
			}
		}

		void StudioWindow::deleteSelection(){
			if(selectedInstances.size() > 0){
				for(int i = 0; i < selectedInstances.size(); i++){
					shared_ptr<Instance::Instance> inst = selectedInstances.at(i);
					if(inst){
						//Let's make a few classes safe..
						std::string className = inst->getClassName();
						if(className != "Workspace" &&
						   className != "Lighting" &&
						   className != "ContentProvider" &&
						   className != "LogService" &&
						   className != "RunService" &&
						   className != "ReplicatedFirst"){
							inst->Destroy();
						}
					}
				}
			}
		}

		void StudioWindow::explorerContextMenu(const QPoint &pos){
			if(explorerCtxMenu){
				explorerCtxMenu->popup(explorer->mapToGlobal(pos));
			}
		}

		void StudioWindow::insertInstance(){
			OBEngine* eng = OBEngine::getInstance();
			if(eng){
			    shared_ptr<Instance::DataModel> dm = eng->getDataModel();
				if(dm){
					shared_ptr<Instance::Instance> parentInstance = dm->getWorkspace();
					
					if(selectedInstances.size() == 1){
						parentInstance = selectedInstances.at(0);
					}

				    QList<QListWidgetItem*> selectedInstanceTypes = basicObjects->selectedItems();
					if(selectedInstanceTypes.size() == 1){
						std::string instanceType = selectedInstanceTypes[0]->text().toStdString();

						shared_ptr<Instance::Instance> newInst = ClassFactory::create(instanceType);

						if(newInst){
							newInst->setParent(parentInstance, true);
						}
					}
				}
			}
		}

		void StudioWindow::groupSelection(){
			
		}
		
		void StudioWindow::ungroupSelection(){
			shared_ptr<Instance::Instance> newPar = NULL;
			
			shared_ptr<Instance::Instance> selectedInst = selectedInstances.at(0);
			if(selectedInst){
				shared_ptr<Instance::Model> selected_model = dynamic_pointer_cast<Instance::Model>(selectedInst);
				if(selected_model){
					newPar = selected_model->getParent();
				}
			}

			if(newPar){
				std::vector<shared_ptr<Instance::Instance>> allKids = selectedInst->GetChildren();
				for(int i = 0; i < allKids.size(); i++){
					shared_ptr<Instance::Instance> kI = allKids.at(i);
					if(kI){
						kI->setParent(newPar, true);
					}
				}
				selectedInst->Destroy();

				selectedInstances = allKids;
				updateSelectionFromLua();
			}
		}
	}
}
