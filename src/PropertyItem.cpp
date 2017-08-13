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
#include <QDoubleSpinBox>

#include "StudioWindow.h"
#include "ColorDialog.h"

#include <cfloat>

namespace OB{
	namespace Studio{
		PropertyItem::PropertyItem(PropertyTreeWidget* tree, QString name){
			this->tree = tree;
		    propertyName = name.toStdString();
		    propertyType = "unknown";

			setText(0, name);
			setFlags(Qt::ItemIsEnabled | Qt::ItemIsEditable);
			setChildIndicatorPolicy(QTreeWidgetItem::DontShowIndicatorWhenChildless);
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

		void PropertyItem::childPropertyUpdated(){}

		// StringPropertyItem

	    StringPropertyItem::StringPropertyItem(PropertyTreeWidget* tree, QString name) : PropertyItem(tree, name){
			setPropertyType("string");
			val = "";
			setText(1, "");
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
			if(flags() & Qt::ItemIsEnabled){
				QLineEdit* lineEdit = new QLineEdit(parent);
				lineEdit->setGeometry(option.rect);
				lineEdit->setFrame(false);
			
				return lineEdit;
			}else{
				return NULL;
			}
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

		// InstancePropertyItem

	    InstancePropertyItem::InstancePropertyItem(PropertyTreeWidget* tree, QString name) : PropertyItem(tree, name){
			setPropertyType("Instance");
			val = NULL;
			setText(1, "");
			
			setFlags(flags() & ~Qt::ItemIsEnabled);
		}

		shared_ptr<Type::VarWrapper> InstancePropertyItem::getValue(){
			return make_shared<Type::VarWrapper>(val);
		}
		
		void InstancePropertyItem::setValue(shared_ptr<Type::VarWrapper> val){
			this->val = val->asInstance();
			
			setText(1, getTextValue());
		}
		
		QString InstancePropertyItem::getTextValue(){
			if(val){
				return QString(val->getName().c_str());
			}else{
				return "";
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
				this->val = true;
			}else{
				this->val = false;
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

		// DoublePropertyItem

	    DoublePropertyItem::DoublePropertyItem(PropertyTreeWidget* tree, QString name) : PropertyItem(tree, name){
			setPropertyType("double");
			val = 0;
			setText(1, getTextValue());
		}

		shared_ptr<Type::VarWrapper> DoublePropertyItem::getValue(){
			return make_shared<Type::VarWrapper>(val);
		}
		
		void DoublePropertyItem::setValue(shared_ptr<Type::VarWrapper> val){
			this->val = val->asDouble();
			setText(1, getTextValue());
		}
		
		QString DoublePropertyItem::getTextValue(){
			return QString::number(val);
		}
		
		void DoublePropertyItem::setTextValue(QString val){
			this->val = val.toDouble();
			setText(1, getTextValue());
		}

		QWidget* DoublePropertyItem::createEditor(QWidget* parent, const QStyleOptionViewItem &option){
		    QDoubleSpinBox* spinBox = new QDoubleSpinBox(parent);
		    spinBox->setGeometry(option.rect);
		    spinBox->setFrame(false);
			spinBox->setMinimum(-DBL_MAX);
			spinBox->setMaximum(DBL_MAX);
			
		    return spinBox;
		}

		void DoublePropertyItem::setEditorData(QWidget* editor){
		    QDoubleSpinBox* spinBox = dynamic_cast<QDoubleSpinBox*>(editor);
			if(spinBox){
			    spinBox->setValue(val);
			}
		}

	    void DoublePropertyItem::setModelData(QWidget* editor){
		    QDoubleSpinBox* spinBox = dynamic_cast<QDoubleSpinBox*>(editor);

			if(spinBox){
			    val = spinBox->value();
				setText(1, getTextValue());
				
				tree->setProp(propertyName, getValue());
			}
		}

		// FloatPropertyItem

	    FloatPropertyItem::FloatPropertyItem(PropertyTreeWidget* tree, QString name) : PropertyItem(tree, name){
			setPropertyType("float");
			val = 0;
			setText(1, getTextValue());
		}

		shared_ptr<Type::VarWrapper> FloatPropertyItem::getValue(){
			return make_shared<Type::VarWrapper>(val);
		}
		
		void FloatPropertyItem::setValue(shared_ptr<Type::VarWrapper> val){
			this->val = val->asFloat();
			setText(1, getTextValue());
		}
		
		QString FloatPropertyItem::getTextValue(){
			return QString::number(val);
		}
		
		void FloatPropertyItem::setTextValue(QString val){
			this->val = val.toFloat();
			setText(1, getTextValue());
		}

		QWidget* FloatPropertyItem::createEditor(QWidget* parent, const QStyleOptionViewItem &option){
		    QDoubleSpinBox* spinBox = new QDoubleSpinBox(parent);
		    spinBox->setGeometry(option.rect);
		    spinBox->setFrame(false);
			spinBox->setMinimum(-FLT_MAX);
			spinBox->setMaximum(FLT_MAX);
			
		    return spinBox;
		}

		void FloatPropertyItem::setEditorData(QWidget* editor){
		    QDoubleSpinBox* spinBox = dynamic_cast<QDoubleSpinBox*>(editor);
			if(spinBox){
			    spinBox->setValue((double)val);
			}
		}

	    void FloatPropertyItem::setModelData(QWidget* editor){
		    QDoubleSpinBox* spinBox = dynamic_cast<QDoubleSpinBox*>(editor);

			if(spinBox){
			    val = (float)spinBox->value();
				setText(1, getTextValue());
				
				tree->setProp(propertyName, getValue());
			}
		}

		// Color3PropertyItem
		
		QIcon getColorAsIcon(const QColor &color){
			QImage img(12, 12, QImage::Format_ARGB32_Premultiplied);
			img.fill(0);

			QPainter painter(&img);
			painter.setCompositionMode(QPainter::CompositionMode_Source);
			painter.setPen(QPen(Qt::black, 1));
			painter.drawRect(0, 0, img.width()-1, img.height()-1);
			painter.fillRect(1, 1, img.width()-2, img.height()-2, color);
			painter.end();

			return QPixmap::fromImage(img);
		}

	    Color3PropertyItem::Color3PropertyItem(PropertyTreeWidget* tree, QString name) : PropertyItem(tree, name){
			setPropertyType("Color3");
			val = make_shared<Type::Color3>();
			setIcon(1, getColorAsIcon(QColor(0, 0, 0)));
			setText(1, getTextValue());
		}

		shared_ptr<Type::VarWrapper> Color3PropertyItem::getValue(){
			return make_shared<Type::VarWrapper>(val);
		}
		
		void Color3PropertyItem::setValue(shared_ptr<Type::VarWrapper> val){
			this->val = val->asColor3();
			if(!this->val){
				this->val = make_shared<Type::Color3>();
			}
			
			setIcon(1, getColorAsIcon(QColor(this->val->getRi(), this->val->getGi(), this->val->getBi())));
			setText(1, getTextValue());
		}
		
		QString Color3PropertyItem::getTextValue(){
			return QString("[%1, %2, %3]").arg(this->val->getRi()).arg(this->val->getGi()).arg(this->val->getBi());
		}

		QWidget* Color3PropertyItem::createEditor(QWidget* parent, const QStyleOptionViewItem &option){
		    ColorDialog* colorDialog = new ColorDialog(StudioWindow::static_win);
			colorDialog->setModal(true);
			
		    return colorDialog;
		}

		void Color3PropertyItem::setEditorData(QWidget* editor){
		    ColorDialog* colorDialog = dynamic_cast<ColorDialog*>(editor);
			if(colorDialog){
			    colorDialog->setCurrentColor(QColor(this->val->getRi(), this->val->getGi(), this->val->getBi()));
			}
		}

	    void Color3PropertyItem::setModelData(QWidget* editor){
		    ColorDialog* colorDialog = dynamic_cast<ColorDialog*>(editor);

			if(colorDialog){
			    QColor col = colorDialog->selectedColor();
				if(col.isValid()){
					val = make_shared<Type::Color3>(col.red(), col.green(), col.blue());
					setText(1, getTextValue());
				
					tree->setProp(propertyName, getValue());
				}
			}
		}

		// ChildDoublePropertyItem

	    ChildDoublePropertyItem::ChildDoublePropertyItem(PropertyTreeWidget* tree, QString name) : PropertyItem(tree, name){
			setPropertyType("double");
			val = 0;
			setText(1, getTextValue());
		}

	    double ChildDoublePropertyItem::getDValue(){
			return val;
		}

		void ChildDoublePropertyItem::setDValue(double val){
			this->val = val;
			setText(1, getTextValue());
		}
		
		QString ChildDoublePropertyItem::getTextValue(){
			return QString::number(val);
		}

		QWidget* ChildDoublePropertyItem::createEditor(QWidget* parent, const QStyleOptionViewItem &option){
		    QDoubleSpinBox* spinBox = new QDoubleSpinBox(parent);
		    spinBox->setGeometry(option.rect);
		    spinBox->setFrame(false);
			spinBox->setMinimum(-DBL_MAX);
			spinBox->setMaximum(DBL_MAX);
			
		    return spinBox;
		}

		void ChildDoublePropertyItem::setEditorData(QWidget* editor){
		    QDoubleSpinBox* spinBox = dynamic_cast<QDoubleSpinBox*>(editor);
			if(spinBox){
			    spinBox->setValue(val);
			}
		}

	    void ChildDoublePropertyItem::setModelData(QWidget* editor){
		    QDoubleSpinBox* spinBox = dynamic_cast<QDoubleSpinBox*>(editor);

			if(spinBox){
			    val = spinBox->value();
				setText(1, getTextValue());

			    QTreeWidgetItem* par = parent();
				if(par){
					((PropertyItem*)par)->childPropertyUpdated();
				}
			}
		}

		// Vector3PropertyItem

	    Vector3PropertyItem::Vector3PropertyItem(PropertyTreeWidget* tree, QString name) : PropertyItem(tree, name){
			setPropertyType("Vector3");
			val = make_shared<Type::Vector3>();
			setText(1, getTextValue());

			xVal = new ChildDoublePropertyItem(tree, "X");
			yVal = new ChildDoublePropertyItem(tree, "Y");
			zVal = new ChildDoublePropertyItem(tree, "Z");

			addChild(xVal);
			addChild(yVal);
			addChild(zVal);
		}

		Vector3PropertyItem::~Vector3PropertyItem(){
			delete xVal;
			delete yVal;
			delete zVal;
		}

		shared_ptr<Type::VarWrapper> Vector3PropertyItem::getValue(){
			return make_shared<Type::VarWrapper>(val);
		}
		
		void Vector3PropertyItem::setValue(shared_ptr<Type::VarWrapper> val){
			this->val = val->asVector3();
			if(!this->val){
				this->val = make_shared<Type::Vector3>();
			}
		    
			setText(1, getTextValue());
			
			xVal->setDValue(this->val->getX());
			yVal->setDValue(this->val->getY());
			zVal->setDValue(this->val->getZ());
		}
		
		QString Vector3PropertyItem::getTextValue(){
			return QString("%1, %2, %3").arg(this->val->getX()).arg(this->val->getY()).arg(this->val->getZ());
		}

		void Vector3PropertyItem::setTextValue(QString val){
			QStringList valSplit = val.split(',');
			if(valSplit.size() == 3){
				QString xStr = valSplit[0].trimmed();
				QString yStr = valSplit[1].trimmed();
				QString zStr = valSplit[2].trimmed();

			    double nX = this->val->getX();
				double nY = this->val->getY();
				double nZ = this->val->getZ();

				bool validDouble = true;

				if(xStr.length() > 0){
					nX = xStr.toDouble(&validDouble);
				}

				if(!validDouble){
					return;
				}

				if(yStr.length() > 0){
					nY = yStr.toDouble(&validDouble);
				}

				if(!validDouble){
					return;
				}

				if(zStr.length() > 0){
					nZ = zStr.toDouble(&validDouble);
				}

				if(!validDouble){
					return;
				}

				this->val = make_shared<Type::Vector3>(nX, nY, nZ);

				setText(1, getTextValue());
				
				xVal->setDValue(nX);
				yVal->setDValue(nY);
				zVal->setDValue(nZ);
			}
		}

		QWidget* Vector3PropertyItem::createEditor(QWidget* parent, const QStyleOptionViewItem &option){
			if(flags() & Qt::ItemIsEnabled){
				QLineEdit* lineEdit = new QLineEdit(parent);
				lineEdit->setGeometry(option.rect);
				lineEdit->setFrame(false);
			
				return lineEdit;
			}else{
				return NULL;
			}
		}

		void Vector3PropertyItem::setEditorData(QWidget* editor){
			QLineEdit* lineEdit = dynamic_cast<QLineEdit*>(editor);
			if(lineEdit){
				lineEdit->setText(getTextValue());
			}
		}

	    void Vector3PropertyItem::setModelData(QWidget* editor){
			QLineEdit* lineEdit = dynamic_cast<QLineEdit*>(editor);

			if(lineEdit){
				setTextValue(lineEdit->text());

				tree->setProp(propertyName, getValue());
			}
		}

		void Vector3PropertyItem::childPropertyUpdated(){
			double xval = xVal->getDValue();
			double yval = yVal->getDValue();
			double zval = zVal->getDValue();

			val = make_shared<Type::Vector3>(xval, yval, zval);
			setText(1, getTextValue());

			tree->setProp(propertyName, getValue());
		}

		// Vector2PropertyItem

	    Vector2PropertyItem::Vector2PropertyItem(PropertyTreeWidget* tree, QString name) : PropertyItem(tree, name){
			setPropertyType("Vector2");
			val = make_shared<Type::Vector2>();
			setText(1, getTextValue());

			xVal = new ChildDoublePropertyItem(tree, "X");
			yVal = new ChildDoublePropertyItem(tree, "Y");

			addChild(xVal);
			addChild(yVal);
		}

		Vector2PropertyItem::~Vector2PropertyItem(){
			delete xVal;
			delete yVal;
		}

		shared_ptr<Type::VarWrapper> Vector2PropertyItem::getValue(){
			return make_shared<Type::VarWrapper>(val);
		}
		
		void Vector2PropertyItem::setValue(shared_ptr<Type::VarWrapper> val){
			this->val = val->asVector2();
			if(!this->val){
				this->val = make_shared<Type::Vector2>();
			}
		    
			setText(1, getTextValue());
			
			xVal->setDValue(this->val->getX());
			yVal->setDValue(this->val->getY());
		}
		
		QString Vector2PropertyItem::getTextValue(){
			return QString("%1, %2").arg(this->val->getX()).arg(this->val->getY());
		}

		void Vector2PropertyItem::setTextValue(QString val){
			QStringList valSplit = val.split(',');
			if(valSplit.size() == 2){
				QString xStr = valSplit[0].trimmed();
				QString yStr = valSplit[1].trimmed();

			    double nX = this->val->getX();
				double nY = this->val->getY();

				bool validDouble = true;

				if(xStr.length() > 0){
					nX = xStr.toDouble(&validDouble);
				}

				if(!validDouble){
					return;
				}

				if(yStr.length() > 0){
					nY = yStr.toDouble(&validDouble);
				}

				if(!validDouble){
					return;
				}

				this->val = make_shared<Type::Vector2>(nX, nY);

				setText(1, getTextValue());
				
				xVal->setDValue(nX);
				yVal->setDValue(nY);
			}
		}

		QWidget* Vector2PropertyItem::createEditor(QWidget* parent, const QStyleOptionViewItem &option){
			if(flags() & Qt::ItemIsEnabled){
				QLineEdit* lineEdit = new QLineEdit(parent);
				lineEdit->setGeometry(option.rect);
				lineEdit->setFrame(false);
			
				return lineEdit;
			}else{
				return NULL;
			}
		}

		void Vector2PropertyItem::setEditorData(QWidget* editor){
			QLineEdit* lineEdit = dynamic_cast<QLineEdit*>(editor);
			if(lineEdit){
				lineEdit->setText(getTextValue());
			}
		}

	    void Vector2PropertyItem::setModelData(QWidget* editor){
			QLineEdit* lineEdit = dynamic_cast<QLineEdit*>(editor);

			if(lineEdit){
				setTextValue(lineEdit->text());

				tree->setProp(propertyName, getValue());
			}
		}

		void Vector2PropertyItem::childPropertyUpdated(){
			double xval = xVal->getDValue();
			double yval = yVal->getDValue();

			val = make_shared<Type::Vector2>(xval, yval);
			setText(1, getTextValue());

			tree->setProp(propertyName, getValue());
		}

		// UDimPropertyItem

	    UDimPropertyItem::UDimPropertyItem(PropertyTreeWidget* tree, QString name) : PropertyItem(tree, name){
			setPropertyType("UDim");
			val = make_shared<Type::UDim>();
			setText(1, getTextValue());

			scale = new ChildDoublePropertyItem(tree, "Scale");
		    offset = new ChildDoublePropertyItem(tree, "Offset");

			addChild(scale);
			addChild(offset);
		}

	    UDimPropertyItem::~UDimPropertyItem(){
			delete scale;
			delete offset;
		}

		shared_ptr<Type::VarWrapper> UDimPropertyItem::getValue(){
			return make_shared<Type::VarWrapper>(val);
		}
		
		void UDimPropertyItem::setValue(shared_ptr<Type::VarWrapper> val){
			this->val = val->asUDim();
			if(!this->val){
				this->val = make_shared<Type::UDim>();
			}
		    
			setText(1, getTextValue());
			
			scale->setDValue(this->val->getScale());
		    offset->setDValue(this->val->getOffset());
		}
		
		QString UDimPropertyItem::getTextValue(){
			return QString("%1, %2").arg(this->val->getScale()).arg(this->val->getOffset());
		}

		void UDimPropertyItem::setTextValue(QString val){
			QStringList valSplit = val.split(',');
			if(valSplit.size() == 2){
				QString scaleStr = valSplit[0].trimmed();
				QString offsetStr = valSplit[1].trimmed();

			    double nScale = this->val->getScale();
				double nOffset = this->val->getOffset();

				bool validDouble = true;

				if(scaleStr.length() > 0){
				    nScale = scaleStr.toDouble(&validDouble);
				}

				if(!validDouble){
					return;
				}

				if(offsetStr.length() > 0){
					nOffset = offsetStr.toDouble(&validDouble);
				}

				if(!validDouble){
					return;
				}

				this->val = make_shared<Type::UDim>(nScale, nOffset);

				setText(1, getTextValue());
				
			    scale->setDValue(nScale);
			    offset->setDValue(nOffset);
			}
		}

		QWidget* UDimPropertyItem::createEditor(QWidget* parent, const QStyleOptionViewItem &option){
			if(flags() & Qt::ItemIsEnabled){
				QLineEdit* lineEdit = new QLineEdit(parent);
				lineEdit->setGeometry(option.rect);
				lineEdit->setFrame(false);
			
				return lineEdit;
			}else{
				return NULL;
			}
		}

		void UDimPropertyItem::setEditorData(QWidget* editor){
			QLineEdit* lineEdit = dynamic_cast<QLineEdit*>(editor);
			if(lineEdit){
				lineEdit->setText(getTextValue());
			}
		}

	    void UDimPropertyItem::setModelData(QWidget* editor){
			QLineEdit* lineEdit = dynamic_cast<QLineEdit*>(editor);

			if(lineEdit){
				setTextValue(lineEdit->text());

				tree->setProp(propertyName, getValue());
			}
		}

		void UDimPropertyItem::childPropertyUpdated(){
			double scaleval = scale->getDValue();
			double offsetval = offset->getDValue();

			val = make_shared<Type::UDim>(scaleval, offsetval);
			setText(1, getTextValue());

			tree->setProp(propertyName, getValue());
		}

		// UDim2PropertyItem

	    UDim2PropertyItem::UDim2PropertyItem(PropertyTreeWidget* tree, QString name) : PropertyItem(tree, name){
			setPropertyType("UDim2");
			val = make_shared<Type::UDim2>();
			setText(1, getTextValue());

			xscale = new ChildDoublePropertyItem(tree, "X Scale");
		    xoffset = new ChildDoublePropertyItem(tree, "X Offset");
			yscale = new ChildDoublePropertyItem(tree, "Y Scale");
		    yoffset = new ChildDoublePropertyItem(tree, "Y Offset");

			addChild(xscale);
			addChild(xoffset);
			addChild(yscale);
			addChild(yoffset);
		}

	    UDim2PropertyItem::~UDim2PropertyItem(){
			delete xscale;
			delete xoffset;
			delete yscale;
			delete yoffset;
		}

		shared_ptr<Type::VarWrapper> UDim2PropertyItem::getValue(){
			return make_shared<Type::VarWrapper>(val);
		}
		
		void UDim2PropertyItem::setValue(shared_ptr<Type::VarWrapper> val){
			this->val = val->asUDim2();
			if(!this->val){
				this->val = make_shared<Type::UDim2>();
			}
		    
			setText(1, getTextValue());

			shared_ptr<Type::UDim> uX = this->val->getX();
			shared_ptr<Type::UDim> uY = this->val->getY();
			
			xscale->setDValue(uX->getScale());
		    xoffset->setDValue(uX->getOffset());
			yscale->setDValue(uY->getScale());
		    yoffset->setDValue(uY->getOffset());
		}
		
		QString UDim2PropertyItem::getTextValue(){
			shared_ptr<Type::UDim> uX = this->val->getX();
			shared_ptr<Type::UDim> uY = this->val->getY();
			return QString("%1, %2, %3, %4").arg(uX->getScale()).arg(uX->getOffset()).arg(uY->getScale()).arg(uY->getOffset());
		}

		void UDim2PropertyItem::setTextValue(QString val){
			QStringList valSplit = val.split(',');
			if(valSplit.size() == 4){
				QString xscaleStr = valSplit[0].trimmed();
				QString xoffsetStr = valSplit[1].trimmed();
				QString yscaleStr = valSplit[2].trimmed();
				QString yoffsetStr = valSplit[3].trimmed();

				shared_ptr<Type::UDim> uX = this->val->getX();
				shared_ptr<Type::UDim> uY = this->val->getY();
				
			    double nxScale = uX->getScale();
				double nxOffset = uX->getOffset();
				double nyScale = uY->getScale();
				double nyOffset = uY->getOffset();

				bool validDouble = true;

				if(xscaleStr.length() > 0){
					nxScale = xscaleStr.toDouble(&validDouble);
				}

				if(!validDouble){
					return;
				}

				if(xoffsetStr.length() > 0){
					nxOffset = xoffsetStr.toDouble(&validDouble);
				}

				if(!validDouble){
					return;
				}

				if(yscaleStr.length() > 0){
					nyScale = yscaleStr.toDouble(&validDouble);
				}

				if(!validDouble){
					return;
				}

				if(yoffsetStr.length() > 0){
					nyOffset = yoffsetStr.toDouble(&validDouble);
				}

				if(!validDouble){
					return;
				}

				this->val = make_shared<Type::UDim2>(nxScale, nxOffset, nyScale, nyOffset);

				setText(1, getTextValue());
				
			    xscale->setDValue(nxScale);
			    xoffset->setDValue(nxOffset);
				yscale->setDValue(nyScale);
			    yoffset->setDValue(nyOffset);
			}
		}

		QWidget* UDim2PropertyItem::createEditor(QWidget* parent, const QStyleOptionViewItem &option){
			if(flags() & Qt::ItemIsEnabled){
				QLineEdit* lineEdit = new QLineEdit(parent);
				lineEdit->setGeometry(option.rect);
				lineEdit->setFrame(false);
			
				return lineEdit;
			}else{
				return NULL;
			}
		}

		void UDim2PropertyItem::setEditorData(QWidget* editor){
			QLineEdit* lineEdit = dynamic_cast<QLineEdit*>(editor);
			if(lineEdit){
				lineEdit->setText(getTextValue());
			}
		}

	    void UDim2PropertyItem::setModelData(QWidget* editor){
			QLineEdit* lineEdit = dynamic_cast<QLineEdit*>(editor);

			if(lineEdit){
				setTextValue(lineEdit->text());

				tree->setProp(propertyName, getValue());
			}
		}

		void UDim2PropertyItem::childPropertyUpdated(){
			double xscaleval = xscale->getDValue();
			double xoffsetval = xoffset->getDValue();
			double yscaleval = yscale->getDValue();
			double yoffsetval = yoffset->getDValue();

			val = make_shared<Type::UDim2>(xscaleval, xoffsetval, yscaleval, yoffsetval);
			setText(1, getTextValue());

			tree->setProp(propertyName, getValue());
		}
	}
}
