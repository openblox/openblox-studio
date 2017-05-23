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

//Studio services
#include "Selection.h"

// OpenBlox Engine
#include <openblox.h>
#include <OBSerializer.h>

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
	    
		QMap<QString, QIcon> classIconMap;

		QIcon StudioWindow::getClassIcon(QString className){
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

		StudioWindow::StudioWindow(){
			static_win = this;

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

		    saveAction = fileMenu->addAction("Save");
			saveAction->setEnabled(false);
			saveAction->setIcon(QIcon::fromTheme("document-save"));
			saveAction->setShortcut(QKeySequence::Save);
			connect(saveAction, &QAction::triggered, this, &StudioWindow::saveAct);
			
		    saveAsAction = fileMenu->addAction("Save As");
			saveAsAction->setEnabled(false);
			saveAsAction->setIcon(QIcon::fromTheme("document-save-as"));
			saveAsAction->setShortcut(QKeySequence::SaveAs);
			connect(saveAsAction, &QAction::triggered, this, &StudioWindow::saveAsAct);
			
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
			/*if(glWidget){
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
			}else{*/
			OBEngine* eng = new OBEngine();
			StudioGLWidget* glWidget = new StudioGLWidget(eng);

			int tabIdx = tabWidget->addTab(glWidget, "Game");
			QTabBar* tabBar = tabWidget->tabBar();
			if(tabBar){
				QWidget* tabBtn = tabBar->tabButton(tabIdx, QTabBar::RightSide);
				if(tabBtn){
					tabBtn->resize(0, 0);
					tabBtn->setVisible(false);
				}
			}

			glWidget->do_init();

			cmdBar->lineEdit()->setDisabled(false);
			//}
		}

		void StudioWindow::commandBarReturn(){
			QLineEdit* cmdEdit = cmdBar->lineEdit();
			QString text = cmdEdit->text();

		    sendOutput("> " + text);

		    OB::OBEngine* eng = getCurrentEngine();
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

		void StudioWindow::update_toolbar_usability(){
			StudioGLWidget* gW = getCurrentGLWidget(getCurrentEngine());
			if(!gW){
				return;
			}
			std::vector<shared_ptr<Instance::Instance>> selectedInstances = gW->selectedInstances;
			
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

		OBEngine* StudioWindow::getCurrentEngine(){
			StudioTabWidget* tW = (StudioTabWidget*)tabWidget->currentWidget();
			if(tW){
				return tW->getEngine();
			}
			return NULL;
		}

		StudioGLWidget* StudioWindow::getCurrentGLWidget(OBEngine* eng){
			// No GLWidget should be associated with NULL
			if(!eng){
				return NULL;
			}

			// If there's a current, that's *probably* the one we're looking for
		    StudioTabWidget* tW = (StudioTabWidget*)tabWidget->currentWidget();
			if(tW){
				if(tW->getEngine() == eng){
					StudioGLWidget* gW = NULL;
					if((gW = dynamic_cast<StudioGLWidget*>(tW))){
						return gW;
					}
				}
			}

			// Otherwise, iterate through
			int numTabs = tabWidget->count();
			for(int i = 0; i < numTabs; i++){
			    StudioTabWidget* tw = (StudioTabWidget*)tabWidget->widget(i);
				if(tw && tw != tW){
					StudioGLWidget* gW = NULL;
					if((gW = dynamic_cast<StudioGLWidget*>(tw))){
						if(gW->getEngine() == eng){
							return gW;
						}
					}
				}
			}
			return NULL;
		}

		void StudioWindow::tickEngines(){
			int numTabs = tabWidget->count();
			for(int i = 0; i < numTabs; i++){
			    StudioTabWidget* tw = (StudioGLWidget*)tabWidget->widget(i);
				StudioGLWidget* gW = NULL;
				if((gW = dynamic_cast<StudioGLWidget*>(tw))){
				    OBEngine* eng = gW->getEngine();
					if(eng){
						eng->tick();
					}
				}
			}

			StudioTabWidget* tW = (StudioTabWidget*)tabWidget->currentWidget();
			if(tW){
				StudioGLWidget* gW = NULL;
				if((gW = dynamic_cast<StudioGLWidget*>(tW))){
				    OBEngine* eng = gW->getEngine();
					if(eng){
						eng->render();
					}
				}
			}
		}

		void StudioWindow::selectionChanged(){
			QList<QTreeWidgetItem*> selectedItems = explorer->selectedItems();

			OBEngine* eng = getCurrentEngine();
			StudioGLWidget* sW = getCurrentGLWidget(eng);
			if(!sW){
				return;
			}
			
			sW->selectedInstances.clear();

			for(int i = 0; i < selectedItems.size(); i++){
				InstanceTreeItem* srcItem = dynamic_cast<InstanceTreeItem*>(selectedItems[i]);
				if(srcItem){
					shared_ptr<Instance::Instance> instPtr = srcItem->GetInstance();
					if(instPtr){
					    sW->selectedInstances.push_back(instPtr);
					}
				}
			}
			
			properties->updateSelection(sW->selectedInstances);
			update_toolbar_usability();

			shared_ptr<Instance::DataModel> dm = eng->getDataModel();
			if(dm){
				shared_ptr<Instance::Selection> selectionService = dynamic_pointer_cast<Instance::Selection>(dm->FindService("Selection"));
				if(selectionService){
					selectionService->getSelectionChanged()->Fire(eng);
				}
			}
		}

		void StudioWindow::updateSelectionFromLua(OBEngine* eng){
			const QSignalBlocker sigBlock(explorer);

			StudioGLWidget* gW = getCurrentGLWidget(eng);
			if(!gW){
				return;
			}

			std::vector<shared_ptr<Instance::Instance>> newSelection = gW->selectedInstances;

			explorer->clearSelection();
			for(int i = 0; i < newSelection.size(); i++){
				shared_ptr<Instance::Instance> inst = gW->selectedInstances.at(i);
				if(inst){
					shared_ptr<Instance::Instance> parInst = inst->getParent();
					
				    InstanceTreeItem* ti = gW->treeItemMap.value(inst);
					if(ti){
					    InstanceTreeItem* pTi = (InstanceTreeItem*)ti->parent();
						if(pTi){
							if(pTi->GetInstance() != parInst){
								std::vector<shared_ptr<Type::VarWrapper>> argVector({make_shared<Type::VarWrapper>(inst)});
								gW->instance_child_removed_evt(argVector, pTi);
							    if(parInst){
								    InstanceTreeItem* tiParent = gW->treeItemMap.value(parInst);
									if(tiParent){
										gW->instance_child_added_evt(argVector, tiParent);
									}
								}
							}
						}
						ti->setSelected(true);
					}else{
						if(parInst){
							InstanceTreeItem* tiParent = gW->treeItemMap.value(parInst);
							if(tiParent){
								std::vector<shared_ptr<Type::VarWrapper>> argVector({make_shared<Type::VarWrapper>(inst)});
								gW->instance_child_added_evt(argVector, tiParent);
								ti = gW->treeItemMap.value(inst);
								if(ti){
									ti->setSelected(true);
								}
							}
						}
					}
				}
			}
		}

		void StudioWindow::populateBasicObjects(){
		    if(basicObjects->count() > 0){
				return;
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
			StudioGLWidget* gW = getCurrentGLWidget(getCurrentEngine());
			if(gW){
				gW->sendOutput(msg);
			}
		}

		void StudioWindow::sendOutput(QString msg, QColor col){
		   StudioGLWidget* gW = getCurrentGLWidget(getCurrentEngine());
			if(gW){
				gW->sendOutput(msg, col);
			} 
		}

		void StudioWindow::deleteSelection(){
			StudioGLWidget* sW = getCurrentGLWidget(getCurrentEngine());
			if(!sW){
				return;
			}
			
			std::vector<shared_ptr<Instance::Instance>> selectedInstances = sW->selectedInstances;

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
			OBEngine* eng = getCurrentEngine();
			if(eng){
				StudioGLWidget* gW = getCurrentGLWidget(eng);
				if(!gW){
					return;
				}
				
			    shared_ptr<Instance::DataModel> dm = eng->getDataModel();
				if(dm){
					shared_ptr<Instance::Instance> parentInstance = dm->getWorkspace();
					
					if(gW->selectedInstances.size() == 1){
						parentInstance = gW->selectedInstances.at(0);
					}

				    QList<QListWidgetItem*> selectedInstanceTypes = basicObjects->selectedItems();
					if(selectedInstanceTypes.size() == 1){
						std::string instanceType = selectedInstanceTypes[0]->text().toStdString();

						shared_ptr<Instance::Instance> newInst = ClassFactory::create(instanceType, eng);

						if(newInst){
							newInst->setParent(parentInstance, true);
						}
					}
				}
			}
		}

		void StudioWindow::groupSelection(){
			const QSignalBlocker sigBlock(explorer);

			OBEngine* eng = getCurrentEngine();
			StudioGLWidget* sW = getCurrentGLWidget(eng);
			if(!sW){
				return;
			}
			
			std::vector<shared_ptr<Instance::Instance>> selectedInstances = sW->selectedInstances;
			
		    shared_ptr<Instance::Instance> selectedInst = selectedInstances.at(0);
			if(selectedInst){
				shared_ptr<Instance::Instance> newPar = selectedInst->getParent();
				
				shared_ptr<Instance::Instance> newModel = ClassFactory::create("Model", eng);
				if(newModel){
					newModel->setParent(newPar, true);

					std::vector<shared_ptr<Instance::Instance>> toGroup = selectedInstances;
					for(int i = 0; i < toGroup.size(); i++){
						shared_ptr<Instance::Instance> kI = toGroup.at(i);
						if(kI){
							std::vector<shared_ptr<Type::VarWrapper>> argVector({make_shared<Type::VarWrapper>(kI)});
							
						    shared_ptr<Instance::Instance> oPar = kI->getParent();
							if(oPar){
								InstanceTreeItem* pTi = sW->treeItemMap.value(oPar);
								if(pTi){
									sW->instance_child_removed_evt(argVector, pTi);
								}
							}

							kI->setParent(newModel, false);
						}
					}

					sW->selectedInstances.clear();
					sW->selectedInstances.push_back(newModel);

					update_toolbar_usability();
				}
			}
		}
		
		void StudioWindow::ungroupSelection(){
			shared_ptr<Instance::Instance> newPar = NULL;

			OBEngine* eng = getCurrentEngine();
			StudioGLWidget* gW = getCurrentGLWidget(eng);
			if(!gW){
				return;
			}
			
			shared_ptr<Instance::Instance> selectedInst = gW->selectedInstances.at(0);
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
			    InstanceTreeItem* pti = gW->treeItemMap.value(newPar);
				if(pti){
					std::vector<shared_ptr<Type::VarWrapper>> argVector({make_shared<Type::VarWrapper>(selectedInst)});
					gW->instance_child_removed_evt(argVector, pti);
				}

				gW->selectedInstances = allKids;
				updateSelectionFromLua(eng);
				update_toolbar_usability();
			}
		}

		void StudioWindow::saveAct(){
			OBEngine* eng = getCurrentEngine();
			StudioGLWidget* gW = getCurrentGLWidget(eng);
			if(!gW){
				return;
			}
			
			if(gW->fileOpened.length() > 0){
			    // We use this instead of Save(file) to allow saving
				// to remote locations such as network drives/webdav
				shared_ptr<OBSerializer> serializer = eng->getSerializer();
				if(!serializer){
					// This should never happen
					statusBar()->showMessage("No serialization support.");
					// This error message is, of course, totally bogus.
					QMessageBox::critical(this, "Error", "Serialization failed due to lack of binary executable data.");
					return;
				}
				
				std::string strToWrite = serializer->SaveInMemory();
				if(strToWrite.length() == 0){
					QString errMsg = "Failed to serialize game.";
					statusBar()->showMessage(errMsg);
					QMessageBox::critical(this, "Error", errMsg);
				}else{
					QFile file(gW->fileOpened);
					std::cout << "Saving to " << gW->fileOpened.toStdString() << std::endl;
					if(file.open(QIODevice::WriteOnly)){
						QTextStream stream(&file);
						stream << QString(strToWrite.c_str()) << flush;
						statusBar()->showMessage("Saved.");
					}else{
						QString errMsg = "Failed to open file";
						statusBar()->showMessage(errMsg);
						QMessageBox::critical(this, "Error", errMsg);
					}
				}
			}else{
				// No file open
				saveAsAct();
			}
		}

		void StudioWindow::saveAsAct(){
			OBEngine* eng = getCurrentEngine();
			StudioGLWidget* gW = getCurrentGLWidget(eng);
			if(!gW){
				return;
			}
			
			QFileDialog* fileDia = new QFileDialog(this);
			fileDia->setAcceptMode(QFileDialog::AcceptSave);
			fileDia->setDefaultSuffix("obgx");
			fileDia->setFileMode(QFileDialog::AnyFile);
			fileDia->setFilter(QDir::Files | QDir::Writable);
			fileDia->setNameFilter("OpenBlox Game (*.obgx)");

			if(fileDia->exec()){
				QList<QUrl> selected = fileDia->selectedUrls();
				if(selected.size() > 0){
				    gW->fileOpened = selected[0].toLocalFile();
					saveAct();
				}else{
					statusBar()->showMessage("Operation canceled.");
				}
			}
		}
	}
}
