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
		bool childNodeTestPartial(u32 index, Ptr<IOcTreeElement> element) const;

	public:
		Cube mCube;
        f32 mRadius = 0;
        Vector3 mHalfSize;
		bool mIsDivisible = false;
		const u32 mMaxChildNumber = 8;

		std::vector<OcTreeNode> mChildren;

        void init(const Cube& cube, const Vector3& minSize, OcTree& tree);
		void addOcTreeElement(Ptr<IOcTreeElement> element);
		void update(OcTree& tree);
		//void checkExit(Collider *collider) const;
		//void manageExits(List<Collider*> *exitingColliders);

		bool isLeaf() const;
    };

private:
	OcTreeNode mRoot;

public:
	Vector3 mSize;

	//DE_M_GET_SET(Status, ColliderStatus)


	void init(f32 size);

	//void addCollider(Collider *collider);
    void addOcTreeElement(Ptr<IOcTreeElement> element);
	void update();
};
