#pragma once

#include "Core/Module.hpp"

class IOcTreeElement: public ObjectBase
{
	GENERATE_METADATA(IOcTreeElement)

public:
	virtual Vector3 getOcTreeElementCenter() const = 0;
	virtual f32 getOcTreeElementRadius() const = 0;
};

// Parent class for QuadTree and OcTree
class OcTree: public ObjectBase
{
	GENERATE_METADATA(OcTree)

public:
	~OcTree() override {};

protected:
	class OcTreeNode: public ObjectBase
	{
		GENERATE_METADATA(OcTree::OcTreeNode)

	private:
		std::vector<IOcTreeElement*> mOcTreeElements;
		void updateChildren();
		bool childNodeTestPartial(u32 index, IOcTreeElement& element) const;

	public:
		Cube mCube;
        f32 mRadius = 0;
        Vector3 mHalfSize;
		bool mIsDivisible = false;
		const u32 mMaxChildNumber = 8;

		std::vector<OcTreeNode> mChildren;

        void init(const Cube& cube, const Vector3& minSize, OcTree& tree);
		void addOcTreeElement(IOcTreeElement& element);
		void update();
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
	void update();
};