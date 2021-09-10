#include "WSceneDataModel.h"
#include <SceneGraph.h>


WNodeDataModel::WNodeDataModel()
{
	
}

void WNodeDataModel::setScene(dyno::SceneGraph* scene)
{
	mSceneGraph = scene;

	layoutAboutToBeChanged().emit();

	for (NodeItem* item : mNodeList)
		delete item;
	mNodeList.clear();

	if (scene)
	{
		recursiveBuild(scene->getRootNode());
	}

	layoutChanged().emit();
}

WNodeDataModel::NodeItem* WNodeDataModel::recursiveBuild(std::shared_ptr<dyno::Node> node)
{
	if (!node)
		return 0;

	NodeItem* item = new NodeItem;
	item->id = mNodeList.size();
	item->ref = node;
	item->parent = 0;
	mNodeList.push_back(item);

	for (auto child : node->getAncestors())
	{
		NodeItem* n = recursiveBuild(child);
		n->parent = item;
		n->offset = item->children.size();
		item->children.push_back(n);
	}

	return item;
}


Wt::WModelIndex WNodeDataModel::parent(const Wt::WModelIndex & index) const
{
	if (index.isValid() && index.internalId() > 0)
	{
		unsigned int idx = index.internalId();
		NodeItem* item = mNodeList[idx];
		NodeItem* parent = item->parent;
		if (item->parent != 0)
		{
			return createIndex(item->parent->offset, 0, item->parent->id);
		}
	}
	return Wt::WModelIndex(); 
}

Wt::WModelIndex WNodeDataModel::index(int row, int column, const Wt::WModelIndex & parent) const
{
	unsigned int id = 0;
	if (parent.isValid())
	{
		unsigned int idx = parent.internalId();
		NodeItem* item = mNodeList[idx];
		NodeItem* curr = item->children[row];
		id = curr->id;
	}
	else
	{
		NodeItem* item = mNodeList[0];
		NodeItem* curr = item->children[row];
		id = curr->id;
	}
	return createIndex(row, column, id);
}

int WNodeDataModel::columnCount(const Wt::WModelIndex & parent) const
{
	return 2;
}

int WNodeDataModel::rowCount(const Wt::WModelIndex & parent) const
{
	unsigned int idx = 0;
	if (parent.isValid())
	{
		idx = parent.internalId();
	}
	if (idx < mNodeList.size())
	{
		NodeItem* item = mNodeList[idx];
		return item->children.size();
	}
	return 0;
}

Wt::cpp17::any WNodeDataModel::data(const Wt::WModelIndex & index, Wt::ItemDataRole role) const
{
	if (index.isValid()) 
	{
		auto node = mNodeList[index.internalId()]->ref;

		if (role == Wt::ItemDataRole::Display || role == Wt::ItemDataRole::ToolTip)
		{
			if (index.column() == 0)
			{
				return node->getName();
			}
			if (index.column() == 1)
			{
				return node->getClassInfo()->getClassName();
			}
		}
		else if (role == Wt::ItemDataRole::Decoration)
		{
			if (index.column() == 0)
			{
				if (node->getName() == "cube")
				{
					return "icons/cube.png";
				}
				else if (node->getName() == "Mesh")
				{
					return "icons/mesh.png";
				}
				return std::string("icons/node.png");
			}
		}
	}
	return Wt::cpp17::any();
}

Wt::cpp17::any WNodeDataModel::headerData(int section, Wt::Orientation orientation, Wt::ItemDataRole role) const
{
	if (orientation == Wt::Orientation::Horizontal && role == Wt::ItemDataRole::Display) {
		switch (section) {
		case 0:
			return std::string("Node");
		case 1:
			return std::string("Type");
		default:
			return Wt::cpp17::any();
		}
	}
	else
		return Wt::cpp17::any();
}

std::shared_ptr<dyno::Node> WNodeDataModel::getNode(const Wt::WModelIndex& index)
{
	return mNodeList[index.internalId()]->ref;
}

void WModuleDataModel::setNode(std::shared_ptr<dyno::Node> node)
{
	mNode = node;
	layoutAboutToBeChanged().emit();
	layoutChanged().emit();
}

int WModuleDataModel::columnCount(const Wt::WModelIndex& parent) const
{
	return 2;
}

int WModuleDataModel::rowCount(const Wt::WModelIndex& parent) const
{
	if (mNode != 0)
	{
		return mNode->getModuleList().size();
	}
	return 0;
}

Wt::cpp17::any WModuleDataModel::data(const Wt::WModelIndex& index, Wt::ItemDataRole role) const
{
	if (mNode != 0 && index.isValid())
	{
		auto mod = mNode->getModuleList();
		auto iter = mod.begin();
		std::advance(iter, index.row());

		if (role == Wt::ItemDataRole::Display || role == Wt::ItemDataRole::ToolTip)
		{
			if (index.column() == 0)
			{
				return (*iter)->getName();
			}
			if (index.column() == 1)
			{
				return (*iter)->getModuleType();
			}			
		}
		else if (role == Wt::ItemDataRole::Decoration)
		{
			if (index.column() == 0)
			{
				return std::string("icons/module.png");
			}
		}
	}
	return Wt::cpp17::any();
}


Wt::cpp17::any WModuleDataModel::headerData(int section, Wt::Orientation orientation, Wt::ItemDataRole role) const
{
	if (orientation == Wt::Orientation::Horizontal && role == Wt::ItemDataRole::Display) {
		switch (section) {
		case 0:
			return std::string("Module");
		case 1:
			return std::string("Type");
		default:
			return Wt::cpp17::any();
		}
	}
	else
		return Wt::cpp17::any();
}

std::shared_ptr<dyno::Module> WModuleDataModel::getModule(const Wt::WModelIndex& index)
{
	if (mNode != 0 && index.isValid())
	{
		auto mod = mNode->getModuleList();
		auto iter = mod.begin();
		std::advance(iter, index.row());
		return *iter;
	}
	return std::shared_ptr<dyno::Module>();
}
