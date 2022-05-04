
#include "SpacePartition/OcTree.hpp"
#include "Graphics/Module.hpp"
#include "Scene/Module.hpp"

void OcTree::OcTreeNode::init(const Cube& cube, const Vector3& minSize, OcTree& tree)
{
	mCube = cube;

	mRadius = mCube.getSize().len() / 2.0f;
	// mTree = tree;

	mHalfSize = mCube.getSize() / 2.0f;
	mIsDivisible = (mHalfSize.x >= minSize.x) && (mHalfSize.y >= minSize.y) && (mHalfSize.z >= minSize.z);

	// mColliders = Memory::allocate<List<Collider*>>();
	// mColliders->init();

	// mExitingColliders = Memory::allocate<List<Collider*>>();
	// mExitingColliders->init();

	//mChildren = Memory::allocate<Array<Node*>>();

	if(mIsDivisible)
	{
		mChildren.reserve(mMaxChildNumber);
		
		// front
		mChildren.emplace_back(OcTreeNode()).
		init(Cube(mCube.getLeftTopFront() + Vector3(0,0,0), mHalfSize), minSize, tree);
		
		mChildren.emplace_back(OcTreeNode()).
		init(Cube(mCube.getLeftTopFront() + Vector3(0,-mHalfSize.y,0), mHalfSize), minSize, tree);
		
		mChildren.emplace_back(OcTreeNode()).
		init(Cube(mCube.getLeftTopFront() + Vector3(mHalfSize.x,-mHalfSize.y,0), mHalfSize), minSize, tree);
		
		mChildren.emplace_back(OcTreeNode()).
		init(Cube(mCube.getLeftTopFront() + Vector3(mHalfSize.x,0,0), mHalfSize), minSize, tree);

		// back
		mChildren.emplace_back(OcTreeNode()).
		init(Cube(mCube.getLeftTopFront() + Vector3(0,0,-mHalfSize.z), mHalfSize), minSize, tree);
		
		mChildren.emplace_back(OcTreeNode()).
		init(Cube(mCube.getLeftTopFront() + Vector3(0,-mHalfSize.y,-mHalfSize.z), mHalfSize), minSize, tree);
		
		mChildren.emplace_back(OcTreeNode()).
		init(Cube(mCube.getLeftTopFront() + Vector3(mHalfSize.x,-mHalfSize.y,-mHalfSize.z), mHalfSize), minSize, tree);
		
		mChildren.emplace_back(OcTreeNode()).
		init(Cube(mCube.getLeftTopFront() + Vector3(mHalfSize.x,0,-mHalfSize.z), mHalfSize), minSize, tree);
	}

	// mLeftTopChildrenArray = Memory::allocate<Array<Vector2>>();
	// mLeftTopChildrenArray->init(4);

	// // This array testPartialCollider the LeftTop Vertices of the 4 children.
	// mLeftTopChildrenArray->set(0, );
	// mLeftTopChildrenArray->set(1, );
	// mLeftTopChildrenArray->set(2, );
	// mLeftTopChildrenArray->set(3, );

}

//----------------------------------------------------------------------

bool OcTree::OcTreeNode::childNodeTestPartial(u32 index, IOcTreeElement& element) const
{
	// TODO: uncomment this when ready
	// ------------------------------

	bool test = false; /*Geometry::testRectangleSphere(mChildren[index].mCube.getLeftTopFront(), mHalfSize.x, mHalfSize.y, mHalfSize.z,
				element.getOcTreeElementCenter(), element.getOcTreeElementRadius(), 0);*/

	return test;
};

//----------------------------------------------------------------------

void OcTree::OcTreeNode::addOcTreeElement(IOcTreeElement& element)
{
	if (mIsDivisible)
	{
		// For each "possible" child node
		FOR_ARRAY(i, mChildren)
		{
			OcTreeNode& node = mChildren[i];
			bool isPartiallyInChildren = childNodeTestPartial(i, element);
			if (isPartiallyInChildren)
			{
				node.addOcTreeElement(element);
			}
		}
	}
	else
	{
		// Add Element to node
		mOcTreeElements.emplace_back(&element);
	}
}

//----------------------------------------------------------------------

// void QuadTree::Node::internalRemoveColliderFromList(const Iterator *it)
// {
// 	auto castedIt = it->cast<Collider*>();
// 	mColliders->remove(*castedIt);

// }

// //----------------------------------------------------------------------

// void QuadTree::Node::internalFreeCollider(Collider *collider)
// {
// 	collider->setDestroyed();
// 	Memory::free<Collider>(collider);
// }

//----------------------------------------------------------------------

void OcTree::OcTreeNode::update(/*contactManager*/)
{
	// DEBUG DRAW
	//RenderEngine::getInstance().drawCube(mCube);

	// If is leaf node.
	if (isLeaf())
	{
		//RenderEngine::getInstance().drawCube(mCube,1,true,Vector4(1,1,1,0.1f));
		//ECHO("LEAF")
	}
	else
	{
		updateChildren(/*contactManager*/);
	}
};

//----------------------------------------------------------------------

void OcTree::OcTreeNode::updateChildren(/*contactManager*/)
{
	FOR_LIST(it, mChildren)
	{
		OcTreeNode& child = *it;
		child.update(/*contactManager*/);
	}
};

//----------------------------------------------------------------------

// void QuadTree::Node::checkExit(Collider *collider) const {
// 	// CHECK if collider is out of this node.
// 	// only dynamic objects can escape from their nodes !!!

// 	if (!collider->isStatic() || collider->getHasSizeChanged())
// {

// //		bool collision = MathUtils::testRectangleSphere(mLeftTop, mWidth, mHeight,
// //						Vector2(collider->getCenter()), collider->getRadius(), 0);

// 		bool collision = MathUtils::testCubePoint(mLeftTop, mWidth, mHeight,
// 								Vector2(collider->getCenter()), -collider->getRadius());

// 		if(!collision || collider->getHasSizeChanged()){
// 			mExitingColliders->pushBack(collider);
// 			mTree->addCollider(collider);
// 		}
// 	}
// };

// //----------------------------------------------------------------------

// void QuadTree::Node::manageExits(List<Collider*> *exitingColliders)
// {

// 	// If any collider has left the node
// 	if (exitingColliders->getLength() > 0)
// {

// 		// FOR EACH COLLIDER
// 		FOR_LIST (itExiting, exitingColliders)
// 		{
// 			Collider* collider = itExiting.get();
// 			mColliders->remove(mColliders->find(collider));

// 			// Note: Only dynamic colliders can leave the node!
// 			if (!collider->isStatic())
// {
// 				mDynamicCollidersCount--;
// 			}
// 		}
// 	}
// };

//----------------------------------------------------------------------

bool OcTree::OcTreeNode::isLeaf() const
{
	return mChildren.size() == 0;
};
// u32 QuadTree::Node::getCollidersCount() const {
// 	return mColliders->getLength();
// };

// void QuadTree::Node::rayCastQuery(const Vector3& lineStart, const Vector3& lineEnd, List<GameObject*> *outList)
// {

// 	bool rayIntersectsNode = MathUtils::testLineSphereSimple(Vector2(lineStart), Vector2(lineEnd),
// 			Vector2(mLeftTop + Vector3(mHalfWidth, -mHalfHeight, 0)), mRadius, 0.0f);

// 	if (rayIntersectsNode)
// {
// 		if (isLeaf())
// {

// 			FOR_LIST(it, mColliders)
// {
// 				Collider* collider = it.get();

// 				GameObject* gameObject = collider->getGameObject();

// 				// TODO : line vs cube
// 				bool rayIntersectsCollider = MathUtils::testLineSphereSimple(Vector2(lineStart), Vector2(lineEnd),
// 						Vector2(collider->getCenter()), collider->getRadius(), 0.0f);

// 				if (rayIntersectsCollider)
// {
// 					List<GameObject*>::ListIterator it = outList->find(gameObject);

// 					// If not found then add it
// 					if (it.isNull())
// {
// 						outList->pushBack(gameObject);
// 					}
// 				}
// 			}

// 		} else {
// 			FOR_ARRAY (i, mChildren)
// {
// 				Node* child = mChildren->get(i);

// 				if (child)
// {
// 					child->rayCastQuery(lineStart, lineEnd, outList);
// 				}
// 			}
// 		}
// 	}
// }

// QuadTree::QuadTree()
// {
// 	mWidth = 0.0f;
// 	mHeight = 0.0f;
// 	//mStatus = ColliderStatus::STATUS_NONE;
// }

// QuadTree::~QuadTree()
// {
// 	//DELETE(mRoot)
// }

void OcTree::init(f32 size)
{
	mSize.set(size, size, size);

	//f32 minSize = Settings::getInstance()->getF32("scene.quadTreeMinSize");

	f32 minSize = 200.0f; //size / 2.0f;
	mRoot.init(Cube(Vector3(-mSize.x / 2.0f, mSize.y / 2.0f, mSize.z / 2.0f), mSize), Vector3(minSize, minSize, minSize), *this);
}

void OcTree::update()
{
	mRoot.update();
}

// void QuadTree::addCollider(Collider *collider)
// {
// 	collider->resetHasSizeChaged();
// 	mRoot->addCollider(collider);
// }

// void QuadTree::rayCastQuery(const Vector3& lineStart, const Vector3& lineEnd, List<GameObject*> *outList)
// {
// 	mRoot->rayCastQuery(lineStart, lineEnd, outList);
// }