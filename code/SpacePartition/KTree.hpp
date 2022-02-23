#pragma once

#include "Core/Module.hpp"

class IKTreeElement: public ObjectBase
{
	GENERATE_METADATA(IKTreeElement)

public:
	virtual Vector3 getKTreeElementCenter() const = 0;
	virtual f32 getKTreeElementRadius() const = 0;
};

// Parent class for QuadTree and OcTree
class KTree: public ObjectBase
{
	GENERATE_METADATA(KTree)

public:
	~KTree() override {};

protected:
	class KTreeNode: public ObjectBase
	{
		GENERATE_METADATA(KTree::KTreeNode)

		PRI std::vector<IKTreeElement*> mKTreeElements;
		PRI void updateChildren();
		PRI bool childNodeTestPartial(u32 index, IKTreeElement& element) const;

	public:
		Rectangle mRectangle;
        f32 mRadius = 0;
        Vector3 mHalfSize;
		bool mIsDivisible = false;

		std::vector<KTreeNode> mChildren;


        void init(const Rectangle& rectangle, const Vector3& minSize, KTree& tree);
		void addKTreeElement(IKTreeElement& element);
		void update();
		//void checkExit(Collider *collider) const;
		//void manageExits(List<Collider*> *exitingColliders);

		bool isLeaf() const;
    };

private:
	KTreeNode mRoot;

public:
	Vector3 mSize;

	//DE_M_GET_SET(Status, ColliderStatus)


	void init(f32 size);

	//void addCollider(Collider *collider);
	void update();
};

class QuadTree: public KTree
{
	GENERATE_METADATA(QuadTree)

	PRI class QuadTreeNode: public KTreeNode
	{
		GENERATE_METADATA(QuadTree::QuadTreeNode)
	//private:
		//void internalRemoveColliderFromList(const Iterator *it);
		//void internalFreeCollider(Collider *collider);

		//DE_M(Tree, QuadTree*)

		//DE_M(Colliders, List<Collider*>*)
		//DE_M(ExitingColliders, List<Collider*>*)
		//DE_M(Children, Array<Node*>*)
		//DE_M(LeftTopChildrenArray, Array<Vector2>*)

		//DE_M(DynamicCollidersCount, u32)
		//DE_M(StaticCollidersCount, u32)

		//DE_CLASS(Node)

		//public: u32 getCollidersCount() const;

		//public: void rayCastQuery(const Vector3& lineStart, const Vector3& lineSnd, List<GameObject*> *outList);
	};

	//public: void rayCastQuery(const Vector3& lineStart, const Vector3& lineSnd, List<GameObject*> *outList);

};