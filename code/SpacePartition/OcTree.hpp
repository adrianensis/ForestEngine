#pragma once

#include "Core/Module.hpp"

class IOcTreeElement;

// Parent class for QuadTree and OcTree
class OcTree: public ObjectBase
{
	GENERATE_METADATA(OcTree)

public:
	~OcTree() override {};

public:
	class OcTreeNode: public ObjectBase
	{
		GENERATE_METADATA(OcTreeNode)
    friend IOcTreeElement;
	private:
		std::vector<Ptr<IOcTreeElement>> mOcTreeElementsStatic;
		std::vector<Ptr<IOcTreeElement>> mOcTreeElementsDynamic;
		void updateChildren(OcTree& tree);
        void addOcTreeElementToChildren(Ptr<IOcTreeElement> element);
        void addOcTreeElementToLeaf(Ptr<IOcTreeElement> element);
        void addOcTreeElementToParent(Ptr<IOcTreeElement> element);
        void createChildren(u8 index);
        u32 getElementsCount() const { return mOcTreeElementsStatic.size() + mOcTreeElementsDynamic.size(); }

	public:
        OcTree* mTree = nullptr;
        OcTreeNode* mParent = nullptr;
		Cube mCube;
    	u8 mDepth = 0;
		inline static const u8 smMaxChildNumber = 8;

		std::array<OcTreeNode*, smMaxChildNumber> mChildren;
		std::array<i8, smMaxChildNumber> mActiveChildren;
		u8 mActiveChildrenIndex = 0;
		std::array<Cube, smMaxChildNumber> mChildrenBoundingBoxes;

        void init(OcTree* tree, OcTreeNode* parent, const Cube& cube, u8 depth);
		void addOcTreeElement(Ptr<IOcTreeElement> element);
		void update(OcTree& tree);
        bool isDivisible() const;
        void drawDebug();
		//void checkExit(Collider *collider) const;
		//void manageExits(List<Collider*> *exitingColliders);
    };

private:
	OcTreeNode mRoot;
    u8 mMaxDepth = 0;
	Vector3 mSize;

public:
	void init(f32 size);

	//void addCollider(Collider *collider);
    void addOcTreeElement(Ptr<IOcTreeElement> element);
	void update();
};

class IOcTreeElement
{
public:
    void init(const Matrix4& modelMatrix, const Vector3& AABBMin, const Vector3& AABBMax);
	virtual bool isOcTreeElementStatic() const { return false;}
	virtual bool isOcTreeTransformChanged() const { return false;}

private:
    Cube mOcTreeBoundingBox;

    // TODO: CONTINUE HERE
    // track current octancts to avoid re-insertions
    // u8 mOctantHash ?
    // octanct Ptr?


public:
    CRGET(OcTreeBoundingBox)
};