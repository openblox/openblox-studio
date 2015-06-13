#ifndef INSTANCETREEITEM_H_
#define INSTANCETREEITEM_H_

#include <Instance.h>

#include <QTreeWidgetItem>

namespace ob_studio{
	class InstanceTreeItem: public QTreeWidgetItem{
		public:
			InstanceTreeItem(ob_instance::Instance* inst, QTreeWidget* parent = 0);
			virtual ~InstanceTreeItem();

			ob_instance::Instance* GetInstance();

		private:
			ob_instance::Instance* inst;
	};
}

#endif
