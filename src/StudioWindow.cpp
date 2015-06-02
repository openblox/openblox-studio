#include "StudioWindow.h"

namespace ob_studio{
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

		viewMenu->addAction("Explorer")->setEnabled(false);
		viewMenu->addAction("Properties")->setEnabled(false);
		viewMenu->addAction("Output")->setEnabled(false);

		viewMenu->addSeparator();

		QMenu* viewToolbarsMenu = viewMenu->addMenu("Toolbars");
		viewToolbarsMenu->addAction("Command");

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
		QDockWidget *dock = new QDockWidget(tr("Output"), this);
		dock->setFloating(false);
		dock->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable);

		dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::BottomDockWidgetArea);

		output = new QTextEdit();
		output->setReadOnly(true);
		output->acceptRichText();

		dock->setWidget(output);
		addDockWidget(Qt::BottomDockWidgetArea, dock);
	}

	void StudioWindow::about(bool checked){
		QMessageBox::about(this, tr("About Dock Widgets"),
		tr("The <b>Dock Widgets</b> example demonstrates how to "
		"use Qt's dock widgets. You can enter your own text, "
		"click a customer to add a customer name and "
		"address, and click standard paragraphs to add them."));
	}
}
