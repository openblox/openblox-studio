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

#ifndef OB_STUDIO_PROPERTYITEM_H_
#define OB_STUDIO_PROPERTYITEM_H_

#include <PropertyTreeWidget.h>
#include <QStyledItemDelegate>

namespace OB{
	namespace Studio{
		class PropertyItem: public QTreeWidgetItem{
		  public:
		    PropertyItem(QString name);
			PropertyItem(PropertyItem* parentItem, QString name);
			virtual ~PropertyItem();

		    std::string getPropertyName();
		    std::string getPropertyType();
			void setPropertyType(std::string type);

			shared_ptr<Type::VarWrapper> getValue();
			void setValue(shared_ptr<Type::VarWrapper> val);
		    QString getTextValue();
			void setTextValue(QString val);
			
			virtual QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem &option);
			virtual void setEditorData(QWidget* editor);
			virtual void setModelData(QWidget* editor);
			virtual bool editorEvent(QEvent* evt);

		  private:
			std::string propertyName;
			std::string propertyType;
		};
	}
}

#endif
