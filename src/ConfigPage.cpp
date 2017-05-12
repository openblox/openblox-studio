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

#include "ConfigPage.h"
#include "ConfigDialog.h"

#include "StudioWindow.h"

#include <QVBoxLayout>
#include <QLabel>

namespace OB{
	namespace Studio{
		ConfigPage::ConfigPage(ConfigDialog* dia){
			this->dia = dia;
		}

		ConfigDialog* ConfigPage::getConfigDialog(){
			return dia;
		}
		
	    GeneralConfigPage::GeneralConfigPage(ConfigDialog* dia) : ConfigPage(dia){
			QVBoxLayout* mainLayout = new QVBoxLayout();

		    opt_useDarkTheme = new QCheckBox("Use dark theme (Requires restart)");
			mainLayout->addWidget(opt_useDarkTheme);

			StudioWindow* win = StudioWindow::static_win;
			if(win){
				if(win->settingsInst){
				    opt_useDarkTheme->setChecked(win->settingsInst->value("dark_theme").toBool());
				}
			}

			if(dia){
				connect(opt_useDarkTheme, &QCheckBox::stateChanged, dia, &ConfigDialog::optionChanged);
			}

			setLayout(mainLayout);
		}

		void GeneralConfigPage::saveChanges(){
		    StudioWindow* win = StudioWindow::static_win;
			if(win){
				if(win->settingsInst){
					QSettings* settings = win->settingsInst;
				    settings->setValue("dark_theme", opt_useDarkTheme->isChecked());
					settings->sync();
				}
			}
		}

		OutputConfigPage::OutputConfigPage(ConfigDialog* dia) : ConfigPage(dia){
			QVBoxLayout* mainLayout = new QVBoxLayout();

			QLabel* historyLabel = new QLabel("Max command history");
			mainLayout->addWidget(historyLabel);
			
		    opt_history = new QSpinBox();
		    opt_history->setMinimum(0);
		    opt_history->setMaximum(INT_MAX);
			mainLayout->addWidget(opt_history);

			StudioWindow* win = StudioWindow::static_win;
			if(win){
				if(win->cmdBar){
					opt_history->setValue(win->cmdBar->maxCount());
				}
			}

			if(dia){
				connect(opt_history, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), dia, &ConfigDialog::optionChanged);
			}

			setLayout(mainLayout);
		}

		void OutputConfigPage::saveChanges(){
		    StudioWindow* win = StudioWindow::static_win;
			if(win){
				if(win->cmdBar){
				    win->cmdBar->setMaxCount(opt_history->value());
				}
				if(win->settingsInst){
					QSettings* settings = win->settingsInst;
					settings->beginGroup("command_history");
					{
					    settings->setValue("max_history", opt_history->value());
					}
					settings->endGroup();
				}
			}
		}
	}
}
