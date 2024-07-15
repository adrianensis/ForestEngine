#pragma once

#include "Core/Minimal.hpp"

class IOcTreeElement;

// Parent class for QuadTree and OcTree
class OcTree
{
public:
	class OcTreeNode
	{
    friend IOcTreeElement;
	private:
		std::vector<Ptr<IOcTreeElement>> mOcTreeElementsStatic;
		std::vector<Ptr<IOcTreeElement>> mOcTreeElementsDynamic;
		std::vector<Ptr<IOcTreeElement>> mOcTreeElementsDynamicReinsert;
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
        u8 mIndex = 0;
		inline static const u8 smMaxChildNumber = 8;

		std::array<OcTreeNode*, smMaxChildNumber> mChildren;
		std::array<i8, smMaxChildNumber> mActiveChildren;
		u8 mActiveChildrenIndex = 0;
		std::array<Cube, smMaxChildNumber> mChildrenBoundingBoxes;

        void init(OcTree* tree, OcTreeNode* parent, u8 index, const Cube& cube, u8 depth);
		void addOcTreeElement(Ptr<IOcTreeElement> element);
		void update(OcTree& tree);
		void updateDynamicElements(OcTree& tree);
        bool isDivisible() const;
        bool isElementEnclosed(Ptr<IOcTreeElement> element) const;
        bool isElementOverlappingChild(Ptr<IOcTreeElement> element, u8 childIndex) const;
        u32 getHash() const;
        void drawDebug();
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
    void init(const Matrix4& modelMatrix, const Vector3& AABBMin, const Vector3& AABBMax, bool isStatic);
    // void addNode(u32 nodeHash);
    // void removeNode(u32 nodeHash);
    // bool isInNode(u32 nodeHash) const;

private:
    Cube mOcTreeBoundingBox;
    bool mIsStatic = false;

    // TODO: CONTINUE HERE
    // track current octancts to avoid re-insertions
    // u8 mOctantHash ?
    // octanct Ptr?
    // std::array<i32, OcTree::OcTreeNode::smMaxChildNumber> mNodes;
    // u8 mNodesIndex = 0;
public:
    bool mPendingToReinsert = false;

public:
    CRGET(OcTreeBoundingBox)
    GET(IsStatic)
};