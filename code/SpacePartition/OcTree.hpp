#pragma once

#include "SpacePartition/OcTreeElement.hpp"

// Parent class for QuadTree and OcTree
class OcTree: public ObjectBase
{
	GENERATE_METADATA(OcTree)

public:
	~OcTree() override {};

protected:
	class OcTreeNode: public ObjectBase
	{
		GENERATE_METADATA(OcTreeNode)

	private:
		std::vector<Ptr<IOcTreeElement>> mOcTreeElementsStatic;
		std::vector<Ptr<IOcTreeElement>> mOcTreeElementsDynamic;
		void updateChildren(OcTree& tree);
        void addOcTreeElementToChildren(Ptr<IOcTreeElement> element);
        void addOcTreeElementToLeaf(Ptr<IOcTreeElement> element);
        void addOcTreeElementToParent(Ptr<IOcTreeElement> element);
        void createChildren(u32 index);
        u32 getElementsCount() const { return mOcTreeElementsStatic.size() + mOcTreeElementsDynamic.size(); }

	public:
        OcTreeNode* mParent = nullptr;
		Cube mCube;
        Vector3 mHalfSize;
    	f32 mMinSize = 0;
    	f32 mMaxElements = 10;
		bool mIsDivisible = false;
		static const u32 smMaxChildNumber = 8;

		std::array<OcTreeNode*, smMaxChildNumber> mChildren;
		std::array<i32, smMaxChildNumber> mActiveChildren;
		u32 mActiveChildrenIndex = 0;
		std::array<Cube, smMaxChildNumber> mChildrenBoundingBoxes;

        void init(OcTreeNode* parent, const Cube& cube, f32 minSize);
		void addOcTreeElement(Ptr<IOcTreeElement> element);
		void update(OcTree& tree);
        void drawDebug();
		//void checkExit(Collider *collider) const;
		//void manageExits(List<Collider*> *exitingColliders);
    };

private:
	OcTreeNode mRoot;

public:
	Vector3 mSize;
	f32 mMinSize = 0;

	void init(f32 size);

	//void addCollider(Collider *collider);
    void addOcTreeElement(Ptr<IOcTreeElement> element);
	void update();
};
