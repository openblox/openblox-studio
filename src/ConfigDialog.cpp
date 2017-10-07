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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the Lesser GNU General Public License
 * along with OpenBlox Studio. If not, see <https://www.gnu.org/licenses/>.
 */

#include "ConfigDialog.h"

#include <QHBoxLayout>
#include <QPushButton>
#include <QVBoxLayout>
#include <QListWidgetItem>

#include "ConfigPage.h"

namespace OB{
	namespace Studio{
		ConfigDialog::ConfigDialog(QWidget* parent) : QDialog(parent){
			this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);

			contentsWidget = new QListWidget();
			contentsWidget->setViewMode(QListView::IconMode);
			contentsWidget->setIconSize(QSize(96, 84));
			contentsWidget->setMovement(QListView::Static);
			contentsWidget->setMaximumWidth(128);
			contentsWidget->setSpacing(12);

			pagesWidget = new QStackedWidget();
			pagesWidget->addWidget(new GeneralConfigPage(this));
			pagesWidget->addWidget(new OutputConfigPage(this));

			QPushButton* closeButton = new QPushButton("Close");
			applyButton = new QPushButton("Apply");
			applyButton->setEnabled(false);
			saveButton = new QPushButton("Save");
			saveButton->setEnabled(false);

			createIcons();

			contentsWidget->setCurrentRow(0);

			connect(closeButton, &QAbstractButton::clicked, this, &QWidget::close);
			connect(applyButton, &QAbstractButton::clicked, this, &ConfigDialog::applyButtonAct);
			connect(saveButton, &QAbstractButton::clicked, this, &ConfigDialog::saveButtonAct);

			QHBoxLayout* horizontalLayout = new QHBoxLayout();
			horizontalLayout->addWidget(contentsWidget);
			horizontalLayout->addWidget(pagesWidget, 1);

			QHBoxLayout* buttonsLayout = new QHBoxLayout();
			buttonsLayout->addStretch(1);
			buttonsLayout->addWidget(saveButton);
			buttonsLayout->addWidget(applyButton);
			buttonsLayout->addWidget(closeButton);

			QVBoxLayout* mainLayout = new QVBoxLayout();
			mainLayout->addLayout(horizontalLayout);
			mainLayout->addStretch(1);
			mainLayout->addSpacing(12);
			mainLayout->addLayout(buttonsLayout);
			setLayout(mainLayout);

			setWindowTitle("Configuration");

			opt_changed = false;
		}

		void ConfigDialog::createIcons(){
			QListWidgetItem* generalConfig = new QListWidgetItem(contentsWidget);
			generalConfig->setText("Configuration");
			generalConfig->setTextAlignment(Qt::AlignHCenter);
			generalConfig->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

			QListWidgetItem* outputConfig = new QListWidgetItem(contentsWidget);
			outputConfig->setText("Output");
			outputConfig->setTextAlignment(Qt::AlignHCenter);
			outputConfig->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

			connect(contentsWidget, &QListWidget::currentItemChanged, this, &ConfigDialog::changePage);
		}

		void ConfigDialog::optionChanged(){
			opt_changed = true;
			applyButton->setEnabled(true);
			saveButton->setEnabled(true);
		}

		void ConfigDialog::saveButtonAct(){
			applyButtonAct();
			close();
		}

		void ConfigDialog::applyButtonAct(){
			if(opt_changed){
				opt_changed = false;
				applyButton->setEnabled(false);
				saveButton->setEnabled(false);

				ConfigPage* cpage = (ConfigPage*)pagesWidget->currentWidget();
				cpage->saveChanges();
			}
		}

		void ConfigDialog::changePage(QListWidgetItem* current, QListWidgetItem* previous){
			if(!current){
				current = previous;
			}

			pagesWidget->setCurrentIndex(contentsWidget->row(current));
		}

		void ConfigDialog::showEvent(QShowEvent* evt){
			QWidget::showEvent(evt);

			QWidget* par = parentWidget();
			if(par){
				QRect parGeom = par->geometry();
				QPoint parCenter = parGeom.center();

				QRect myGeom = geometry();
				myGeom.moveCenter(parCenter);
				setGeometry(myGeom);
			}
		}
	}
}
