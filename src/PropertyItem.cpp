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
#include <QPixmap>
#include <QPixmapCache>
#include <QApplication>
#include <QPainter>
#include <QMouseEvent>
#include <QSpinBox>

namespace OB{
	namespace Studio{
		PropertyItem::PropertyItem(PropertyTreeWidget* tree, QString name){
			this->tree = tree;
		    propertyName = name.toStdString();
		    propertyType = "unknown";

			setText(0, name);
			setFlags(Qt::ItemIsEnabled | Qt::ItemIsEditable);
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

		bool PropertyItem::editorEvent(QEvent* evt){
			return false;
		}

		// StringPropertyItem

	    StringPropertyItem::StringPropertyItem(PropertyTreeWidget* tree, QString name) : PropertyItem(tree, name){
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

				tree->setProp(propertyName, getValue());
			}
		}

		// BoolPropertyItem

	    QIcon getCheckBox(int val, bool disabled){
			QPixmap pixmap;
			if(!QPixmapCache::find(QString("CheckBoxState%1-%2").arg(val).arg(disabled), &pixmap)){
			    QStyleOptionButton opt;
				if(val < 0){
					opt.state |= QStyle::State_NoChange;
				}else{
					opt.state |= val ? QStyle::State_On : QStyle::State_Off;
				}
				opt.state |= disabled ? QStyle::State_ReadOnly : QStyle::State_Enabled;
				const QStyle* style = QApplication::style();
				const int indicatorWidth = style->pixelMetric(QStyle::PM_IndicatorWidth, &opt);
				const int indicatorHeight = style->pixelMetric(QStyle::PM_IndicatorHeight, &opt);
				const int listViewIconSize = indicatorWidth;
				const int pixmapWidth = indicatorWidth;
				const int pixmapHeight = qMax(indicatorHeight, listViewIconSize);

				opt.rect = QRect(0, 0, indicatorWidth, indicatorHeight);
				pixmap = QPixmap(pixmapWidth, pixmapHeight);
				pixmap.fill(Qt::transparent);
				{
					const int xoff = (pixmapWidth  > indicatorWidth)  ? (pixmapWidth  - indicatorWidth)  / 2 : 0;
					const int yoff = (pixmapHeight > indicatorHeight) ? (pixmapHeight - indicatorHeight) / 2 : 0;
					QPainter painter(&pixmap);
					painter.translate(xoff, yoff);
					style->drawPrimitive(QStyle::PE_IndicatorCheckBox, &opt, &painter);
				}

				QPixmapCache::insert(QString("CheckBoxState%1-%2").arg(val).arg(disabled), pixmap);
			}

			return QIcon(pixmap);
		}

	    BoolPropertyItem::BoolPropertyItem(PropertyTreeWidget* tree, QString name) : PropertyItem(tree, name){
			setPropertyType("bool");
			val = false;
			setIcon(1, getCheckBox(val, flags() & Qt::ItemIsEnabled));
		}

		shared_ptr<Type::VarWrapper> BoolPropertyItem::getValue(){
			return make_shared<Type::VarWrapper>(val);
		}
		
		void BoolPropertyItem::setValue(shared_ptr<Type::VarWrapper> val){
			this->val = val->asBool();
			setIcon(1, getCheckBox(this->val, !(flags() & Qt::ItemIsEnabled)));
		}
		
		QString BoolPropertyItem::getTextValue(){
		    if(val){
				return "true";
			}else{
				return "false";
			}
		}
		
		void BoolPropertyItem::setTextValue(QString val){
		    if(val == "true"){
				val = true;
			}else{
				val = false;
			}
			setIcon(1, getCheckBox(this->val, !(flags() & Qt::ItemIsEnabled)));
		}

	    bool BoolPropertyItem::editorEvent(QEvent* evt){
			if((flags() & Qt::ItemIsEnabled) && (evt->type() == QEvent::MouseButtonRelease)){
				QMouseEvent* mEvt = static_cast<QMouseEvent*>(evt);
				if(mEvt && (mEvt->button() & Qt::LeftButton)){
					val = !val;
					setIcon(1, getCheckBox(this->val, !(flags() & Qt::ItemIsEnabled)));
					
					tree->setProp(propertyName, getValue());
				}
			}
		}

		// IntPropertyItem

	    IntPropertyItem::IntPropertyItem(PropertyTreeWidget* tree, QString name) : PropertyItem(tree, name){
			setPropertyType("int");
			val = 0;
			setText(1, getTextValue());
		}

		shared_ptr<Type::VarWrapper> IntPropertyItem::getValue(){
			return make_shared<Type::VarWrapper>(val);
		}
		
		void IntPropertyItem::setValue(shared_ptr<Type::VarWrapper> val){
			this->val = val->asInt();
			setText(1, getTextValue());
		}
		
		QString IntPropertyItem::getTextValue(){
			return QString::number(val);
		}
		
		void IntPropertyItem::setTextValue(QString val){
			this->val = val.toInt();
			setText(1, getTextValue());
		}

		QWidget* IntPropertyItem::createEditor(QWidget* parent, const QStyleOptionViewItem &option){
		    QSpinBox* spinBox = new QSpinBox(parent);
		    spinBox->setGeometry(option.rect);
		    spinBox->setFrame(false);
			spinBox->setMinimum(INT_MIN);
			spinBox->setMaximum(INT_MAX);
			
		    return spinBox;
		}

		void IntPropertyItem::setEditorData(QWidget* editor){
		    QSpinBox* spinBox = dynamic_cast<QSpinBox*>(editor);
			if(spinBox){
			    spinBox->setValue(val);
			}
		}

	    void IntPropertyItem::setModelData(QWidget* editor){
		    QSpinBox* spinBox = dynamic_cast<QSpinBox*>(editor);

			if(spinBox){
			    val = spinBox->value();
				setText(1, getTextValue());
				
				tree->setProp(propertyName, getValue());
			}
		}
	}
}
