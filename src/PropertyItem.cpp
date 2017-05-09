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

#include "PropertyItem.h"

#include <QLineEdit>

namespace OB{
	namespace Studio{
		PropertyItem::PropertyItem(QString name) : PropertyItem(NULL, name){}

		PropertyItem::PropertyItem(PropertyItem* parentItem, QString name){
		    propertyName = name.toStdString();
		    propertyType = "unknown";

			setText(0, name);
			setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
		}

	    PropertyItem::~PropertyItem(){}

		std::string PropertyItem::getPropertyName(){
			return propertyName;
		}
		
		std::string PropertyItem::getPropertyType(){
			return propertyType;
		}
		
		void PropertyItem::setPropertyType(std::string type){
			propertyType = type;
		}

		shared_ptr<Type::VarWrapper> PropertyItem::getValue(){
			return NULL;
		}
		
		void PropertyItem::setValue(shared_ptr<Type::VarWrapper> val){}
		
		QString PropertyItem::getTextValue(){
			return "unknown";
		}
		
		void PropertyItem::setTextValue(QString val){}

		QWidget* PropertyItem::createEditor(QWidget* parent, const QStyleOptionViewItem &option){
		    return NULL;
		}

		void PropertyItem::setEditorData(QWidget* editor){}

	    void PropertyItem::setModelData(QWidget* editor){}

		bool PropertyItem::editorEvent(QEvent* evt){}

		//StringPropertyItem

	    StringPropertyItem::StringPropertyItem(QString name) : PropertyItem(NULL, name){
			setPropertyType("string");
			val = "";
			setText(1, QString(val.c_str()));
		}

		shared_ptr<Type::VarWrapper> StringPropertyItem::getValue(){
			return make_shared<Type::VarWrapper>(val);
		}
		
		void StringPropertyItem::setValue(shared_ptr<Type::VarWrapper> val){
			this->val = val->asString();
			setText(1, getTextValue());
		}
		
		QString StringPropertyItem::getTextValue(){
			return QString(val.c_str());
		}
		
		void StringPropertyItem::setTextValue(QString val){
			this->val = val.toStdString();
			setText(1, getTextValue());
		}

		QWidget* StringPropertyItem::createEditor(QWidget* parent, const QStyleOptionViewItem &option){
			QLineEdit* lineEdit = new QLineEdit(parent);
			lineEdit->setGeometry(option.rect);
			lineEdit->setFrame(false);
			
		    return lineEdit;
		}

		void StringPropertyItem::setEditorData(QWidget* editor){
			QLineEdit* lineEdit = dynamic_cast<QLineEdit*>(editor);
			if(lineEdit){
				lineEdit->setText(getTextValue());
			}
		}

	    void StringPropertyItem::setModelData(QWidget* editor){
			QLineEdit* lineEdit = dynamic_cast<QLineEdit*>(editor);

			if(lineEdit){
				setTextValue(lineEdit->text());
			}
		}
	}
}
