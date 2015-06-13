#include "InstanceTreeItem.h"

namespace ob_studio{
	InstanceTreeItem::InstanceTreeItem(ob_instance::Instance* inst, QTreeWidget* parent) : QTreeWidgetItem(parent){
		this->inst = inst;

		this->setText(0, inst->getName());
	}

	InstanceTreeItem::~InstanceTreeItem(){}

	ob_instance::Instance* InstanceTreeItem::GetInstance(){
		return inst;
	}
}
