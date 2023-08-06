
#include "SpacePartition/OcTree.hpp"
#include "Graphics/Module.hpp"
#include "Scene/Module.hpp"

void OcTree::OcTreeNode::init(OcTreeNode* parent, const Cube& cube, f32 minSize, OcTree& tree)
{
    mParent = parent;
	mCube = cube;
    mMinSize.set(minSize,minSize,minSize);

	mRadius = mCube.getSize().len() / 2.0f;

	mHalfSize = mCube.getSize() / 2.0f;
	mIsDivisible = (mHalfSize.x >= mMinSize.x) and (mHalfSize.y >= mMinSize.y) and (mHalfSize.z >= mMinSize.z);

	if(mIsDivisible)
	{
		mChildren.resize(mMaxChildNumber);
		mChildrenBoundingBoxes.reserve(mMaxChildNumber);
        
		// front
        mChildrenBoundingBoxes.emplace_back(Cube(mCube.getLeftTopFront() + Vector3(0,0,0), mHalfSize));
        mChildrenBoundingBoxes.emplace_back(Cube(mCube.getLeftTopFront() + Vector3(0,-mHalfSize.y,0), mHalfSize));
        mChildrenBoundingBoxes.emplace_back(Cube(mCube.getLeftTopFront() + Vector3(mHalfSize.x,-mHalfSize.y,0), mHalfSize));
        mChildrenBoundingBoxes.emplace_back(Cube(mCube.getLeftTopFront() + Vector3(mHalfSize.x,0,0), mHalfSize));
        
		// back
        mChildrenBoundingBoxes.emplace_back(Cube(mCube.getLeftTopFront() + Vector3(0,0,-mHalfSize.z), mHalfSize));
        mChildrenBoundingBoxes.emplace_back(Cube(mCube.getLeftTopFront() + Vector3(0,-mHalfSize.y,-mHalfSize.z), mHalfSize));
        mChildrenBoundingBoxes.emplace_back(Cube(mCube.getLeftTopFront() + Vector3(mHalfSize.x,-mHalfSize.y,-mHalfSize.z), mHalfSize));
        mChildrenBoundingBoxes.emplace_back(Cube(mCube.getLeftTopFront() + Vector3(mHalfSize.x,0,-mHalfSize.z), mHalfSize));
	}
}

//----------------------------------------------------------------------

bool OcTree::OcTreeNode::childNodeTestPartial(u32 index, Ptr<IOcTreeElement> element) const
{
    PROFILER_CPU()
    bool test = Geometry::testCubeSphere(mChildrenBoundingBoxes[index],
			Sphere(element->getOcTreeElementCenter(), element->getOcTreeElementRadius()), 0);

	return test;
};

//----------------------------------------------------------------------

void OcTree::OcTreeNode::addOcTreeElement(Ptr<IOcTreeElement> element, f32 minSize, OcTree& tree)
{
    PROFILER_CPU()
    if (mIsDivisible)
    {
        addOcTreeElementToChildren(element, minSize, tree);
    }
    else
    {
        addOcTreeElementLeaf(element);
    }
}

void OcTree::OcTreeNode::addOcTreeElementToChildren(Ptr<IOcTreeElement> element, f32 minSize, OcTree& tree)
{
    PROFILER_CPU()
    // For each "possible" child node
    FOR_RANGE(i, 0, mMaxChildNumber)
    {
        bool isPartiallyInChildren = childNodeTestPartial(i, element);
        if (isPartiallyInChildren)
        {
            if(!mChildren[i])
            {
                createChildren(i, minSize, tree);
            }
            mChildren[i]->addOcTreeElement(element, minSize, tree);
        }
    }
}

void OcTree::OcTreeNode::createChildren(u32 index, f32 minSize, OcTree& tree)
{
    PROFILER_CPU()
    mChildren[index] = Memory::newObject<OcTreeNode>();
    mChildren[index]->init(this, mChildrenBoundingBoxes[index], minSize, tree);
    mActiveChildren.push_back(mChildren[index]);
}

void OcTree::OcTreeNode::addOcTreeElementLeaf(Ptr<IOcTreeElement> element)
{
    PROFILER_CPU()
    auto* octreeNodeElements = &mOcTreeElementsStatic;
    if(!element->isOcTreeElementStatic())
    {
        octreeNodeElements = &mOcTreeElementsDynamic;
    }

    // Add Element to leaf node
    auto it = std::find(octreeNodeElements->begin(), octreeNodeElements->end(), element);
    if(it == octreeNodeElements->end())
    {
        octreeNodeElements->push_back(element);
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

void OcTree::OcTreeNode::update(OcTree& tree/*contactManager*/)
{
    PROFILER_CPU()

    // GET_SYSTEM(RenderEngine).drawCube(mCube,1,true,Vector4(1,1,1,0.5f));

	// If is leaf node.
	if (isLeaf())
	{
        // DEBUG DRAW
        if(mOcTreeElementsStatic.size() > 0 || mOcTreeElementsDynamic.size() > 0)
        {
		    // GET_SYSTEM(RenderEngine).drawCube(mCube,1,true,Vector4(1,1,0,1));

            // FOR_RANGE(i,0,mOcTreeElementsStatic.size())
            // {
            //     Ptr<IOcTreeElement> element = mOcTreeElementsStatic[i];
            //     GET_SYSTEM(RenderEngine).drawCube(element->getOcTreeBoundingBox(),1,true,Vector4(0,0.8,0.8,1));
            // }
            // FOR_RANGE(i,0,mOcTreeElementsDynamic.size())
            // {
            //     Ptr<IOcTreeElement> element = mOcTreeElementsDynamic[i];
            //     GET_SYSTEM(RenderEngine).drawCube(element->getOcTreeBoundingBox(),1,true,Vector4(1,0,0,1));
            // }
        }

        // TODO: don't reinsert, check the parent/siblings instead

        // auto dynamicElementsCopyArray = mOcTreeElementsDynamic;
        // mOcTreeElementsDynamic.clear();
        // FOR_LIST(it, dynamicElementsCopyArray)
        // {
        //     tree.addOcTreeElement(*it);
        // }
	}
	else
	{
		updateChildren(tree/*contactManager*/);
	}
};

//----------------------------------------------------------------------

void OcTree::OcTreeNode::updateChildren(OcTree& tree/*contactManager*/)
{
    PROFILER_CPU()
	FOR_RANGE(i, 0, mActiveChildren.size())
	{
        mActiveChildren[i]->update(tree/*contactManager*/);
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
	return mActiveChildren.size() == 0;
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

// 				GameObject* gameObject = collider->mGameObject;

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
// 	//Memory::deleteObject(mRoot)
// }

void OcTree::init(f32 size)
{
	mSize.set(size, size, size);

	//f32 minSize = Settings::getInstance()->getF32("scene.quadTreeMinSize");

	mMinSize = 200.0f; //size / 2.0f;
	mRoot.init(nullptr, Cube(Vector3(-mSize.x / 2.0f, mSize.y / 2.0f, mSize.z / 2.0f), mSize), mMinSize, *this);
}

void OcTree::update()
{
    PROFILER_CPU()
	mRoot.update(*this);
}

void OcTree::addOcTreeElement(Ptr<IOcTreeElement> element)
{
    PROFILER_CPU()
    mRoot.addOcTreeElement(element, mMinSize, *this);
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