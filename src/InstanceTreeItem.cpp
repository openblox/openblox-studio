#include "InstanceTreeItem.h"

namespace ob_studio{
	InstanceTreeItem::InstanceTreeItem(ob_instance::Instance* inst, QTreeWidget* parent) : QTreeWidgetItem(parent){
		this->inst = inst;
	}

	InstanceTreeItem::~InstanceTreeItem(){
	}

}
