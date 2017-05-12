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

#ifndef OB_STUDIO_CONFIGDIALOG_H_
#define OB_STUDIO_CONFIGDIALOG_H_

#include <QDialog>
#include <QListWidget>
#include <QListWidgetItem>
#include <QStackedWidget>

namespace OB{
	namespace Studio{
		class ConfigDialog: public QDialog{
		  public:
			ConfigDialog(QWidget* parent);

			void optionChanged();

			void saveButtonAct();
			void applyButtonAct();

		  public slots:
			void changePage(QListWidgetItem* current, QListWidgetItem* previous);

		  private:
			void createIcons();

			QPushButton* applyButton;
			QPushButton* saveButton;
			
			bool opt_changed;

			QListWidget* contentsWidget;
			QStackedWidget* pagesWidget;
		};
	}
}

#endif
