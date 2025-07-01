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
		std::vector<Core::WeakPtr<IOcTreeElement>> mOcTreeElementsStatic;
		std::vector<Core::WeakPtr<IOcTreeElement>> mOcTreeElementsDynamic;
		std::vector<Core::WeakPtr<IOcTreeElement>> mOcTreeElementsDynamicReinsert;
		void updateChildren(OcTree& tree);
        void addOcTreeElementToChildren(Core::WeakPtr<IOcTreeElement> element);
        void addOcTreeElementToLeaf(Core::WeakPtr<IOcTreeElement> element);
        void addOcTreeElementToParent(Core::WeakPtr<IOcTreeElement> element);
        void createChildren(Core::u8 index);
        Core::u32 getElementsCount() const { return mOcTreeElementsStatic.size() + mOcTreeElementsDynamic.size(); }

	public:
        OcTree* mTree = nullptr;
        OcTreeNode* mParent = nullptr;
		Cube mCube;
    	Core::u8 mDepth = 0;
        Core::u8 mIndex = 0;
		inline static const Core::u8 smMaxChildNumber = 8;

		std::array<OcTreeNode*, smMaxChildNumber> mChildren;
		std::array<Core::i8, smMaxChildNumber> mActiveChildren;
		Core::u8 mActiveChildrenIndex = 0;
		std::array<Cube, smMaxChildNumber> mChildrenBoundingBoxes;

        void init(OcTree* tree, OcTreeNode* parent, Core::u8 index, const Cube& cube, Core::u8 depth);
		void addOcTreeElement(Core::WeakPtr<IOcTreeElement> element);
		void update(OcTree& tree);
		void updateDynamicElements(OcTree& tree);
        bool isDivisible() const;
        bool isElementEnclosed(Core::WeakPtr<IOcTreeElement> element) const;
        bool isElementOverlappingChild(Core::WeakPtr<IOcTreeElement> element, Core::u8 childIndex) const;
        Core::u32 getHash() const;
        void drawDebug();
    };

private:
	OcTreeNode mRoot;
    Core::u8 mMaxDepth = 0;
	Vector3 mSize;

public:
	void init(Core::f32 size);

	//void addCollider(Collider *collider);
    void addOcTreeElement(Core::WeakPtr<IOcTreeElement> element);
	void update();
};

class IOcTreeElement
{
public:
    void init(const Matrix4& modelMatrix, const Vector3& AABBMin, const Vector3& AABBMax, bool isStatic);
    // void addNode(Core::u32 nodeHash);
    // void removeNode(Core::u32 nodeHash);
    // bool isInNode(Core::u32 nodeHash) const;

private:
    Cube mOcTreeBoundingBox;
    bool mIsStatic = false;

    // NEXT: CONTINUE HERE
    // track current octancts to avoid re-insertions
    // Core::u8 mOctantHash ?
    // octanct Core::WeakPtr?
    // std::array<Core::i32, OcTree::OcTreeNode::smMaxChildNumber> mNodes;
    // Core::u8 mNodesIndex = 0;
public:
    bool mPendingToReinsert = false;

public:
    CRGET(OcTreeBoundingBox)
    GET(IsStatic)
};