
#include "SpacePartition/OcTree.hpp"
#include "Scene/Module.hpp"
#include "Graphics/Debug/DebugRenderer.hpp"

void OcTree::OcTreeNode::init(OcTree* tree, OcTreeNode* parent, Core::u8 index, const Maths::Cube& cube, Core::u8 depth)
{
    mTree = tree;
    mParent = parent;
	mCube = cube;
	mDepth = depth;
	mIndex = index;

    FOR_RANGE(i, 0, smMaxChildNumber)
    {
        mActiveChildren[i] = INVALID_INDEX;
    }

	if(isDivisible())
	{
	    Maths::Vector3 halfSize = mCube.getSize() / 2.0f;
		// front +z
            // left -x
                // up +y
                mChildrenBoundingBoxes[0] = Maths::Cube(mCube.getLeftTopFront() + Maths::Vector3::smZero, halfSize);
                // down -y
                mChildrenBoundingBoxes[1] = Maths::Cube(mCube.getLeftTopFront() + Maths::Vector3(0,-halfSize.y,0), halfSize);

            // right +x
                // up +y
                mChildrenBoundingBoxes[2] = Maths::Cube(mCube.getLeftTopFront() + Maths::Vector3(halfSize.x,0,0), halfSize);
                // down -y
                mChildrenBoundingBoxes[3] = Maths::Cube(mCube.getLeftTopFront() + Maths::Vector3(halfSize.x,-halfSize.y,0), halfSize);
                
		// back -z
            // left -x

                // up +y
                mChildrenBoundingBoxes[4] = Maths::Cube(mCube.getLeftTopFront() + Maths::Vector3(0,0,-halfSize.z), halfSize);
                // down -y
                mChildrenBoundingBoxes[5] = Maths::Cube(mCube.getLeftTopFront() + Maths::Vector3(0,-halfSize.y,-halfSize.z), halfSize);
                
            // right +x
                // up +y
                mChildrenBoundingBoxes[6] = Maths::Cube(mCube.getLeftTopFront() + Maths::Vector3(halfSize.x,0,-halfSize.z), halfSize);
                // down -y
                mChildrenBoundingBoxes[7] = Maths::Cube(mCube.getLeftTopFront() + Maths::Vector3(halfSize.x,-halfSize.y,-halfSize.z), halfSize);
        
	}
}

bool OcTree::OcTreeNode::isDivisible() const
{
    return mDepth < (mTree->mMaxDepth - 1);
}

bool OcTree::OcTreeNode::isElementEnclosed(Core::WeakPtr<IOcTreeElement> element) const
{
    PROFILER_CPU()
    bool test = Maths::Geometry::testSphereInsideCube(mCube,
    Maths::Sphere(element->getOcTreeBoundingBox().getCenter(), element->getOcTreeBoundingBox().getRadius()));
    return test;
}

bool OcTree::OcTreeNode::isElementOverlappingChild(Core::WeakPtr<IOcTreeElement> element, Core::u8 childIndex) const
{
    PROFILER_CPU()
    bool test = Maths::Geometry::testCubeSphere(mChildrenBoundingBoxes[childIndex],
    Maths::Sphere(element->getOcTreeBoundingBox().getCenter(), element->getOcTreeBoundingBox().getRadius()), 0);
    return test;
}

void OcTree::OcTreeNode::addOcTreeElement(Core::WeakPtr<IOcTreeElement> element)
{
    PROFILER_CPU()

    if (isDivisible())
    {
        addOcTreeElementToChildren(element);
    }
    else
    {
        addOcTreeElementToLeaf(element);
    }
}

void OcTree::OcTreeNode::addOcTreeElementToChildren(Core::WeakPtr<IOcTreeElement> element)
{
    PROFILER_CPU()

    // For each "possible" child node
    FOR_RANGE(i, 0, smMaxChildNumber)
    {
        if (isElementOverlappingChild(element, i))
        {
            if(!mChildren[i])
            {
                createChildren(i);
                mActiveChildren[mActiveChildrenIndex] = i;
                mActiveChildrenIndex++;
            }
            mChildren[i]->addOcTreeElement(element);
        }
    }
}

void OcTree::OcTreeNode::createChildren(Core::u8 index)
{
    PROFILER_CPU()
    mChildren[index] = Core::Memory::newObject<OcTreeNode>();
    mChildren[index]->init(mTree, this, index, mChildrenBoundingBoxes[index], mDepth + 1);
}

void OcTree::OcTreeNode::addOcTreeElementToLeaf(Core::WeakPtr<IOcTreeElement> element)
{
    PROFILER_CPU()
    auto* octreeNodeElements = &mOcTreeElementsStatic;
    if(!element->getIsStatic())
    {
        octreeNodeElements = &mOcTreeElementsDynamic;
    }

    // Add Element to leaf node
    octreeNodeElements->push_back(element);
}

void OcTree::OcTreeNode::addOcTreeElementToParent(Core::WeakPtr<IOcTreeElement> element)
{
    PROFILER_CPU()

    if(isElementEnclosed(element))
    {
        element->mPendingToReinsert = true;
        mOcTreeElementsDynamicReinsert.push_back(element);
    }
    else
    {
        if(mParent)
        {
            mParent->addOcTreeElementToParent(element);
        }
    }
}

void OcTree::OcTreeNode::update(OcTree& tree)
{
    PROFILER_CPU()

    // drawDebug();

	if (isDivisible())
	{
        FOR_LIST(it, mOcTreeElementsDynamicReinsert)
        {
            Core::WeakPtr<IOcTreeElement> element = *it;
            addOcTreeElementToChildren(element);
            element->mPendingToReinsert = false;
        }
        mOcTreeElementsDynamicReinsert.clear();

		updateChildren(tree);
	}
}

void OcTree::OcTreeNode::updateDynamicElements(OcTree& tree)
{
    PROFILER_CPU()

	if (isDivisible())
	{
        FOR_RANGE(i, 0, mActiveChildrenIndex)
        {
            Core::i8 childrenIndex = mActiveChildren[i];
            OcTreeNode* node = mChildren[childrenIndex];
            if(node)
            {
                node->updateDynamicElements(tree);
            }
        }
	}
    else // If is leaf node
	{
        auto dynamicElementsCopyArray = mOcTreeElementsDynamic;
        mOcTreeElementsDynamic.clear();
        FOR_LIST(it, dynamicElementsCopyArray)
        {
            Core::WeakPtr<IOcTreeElement> element = *it;
            if(isElementEnclosed(element))
            {
                mOcTreeElementsDynamic.push_back(element);
            }
            else
            {
                if(!element->mPendingToReinsert)
                {
                    if(mParent)
                    {
                        mParent->addOcTreeElementToParent(element);
                    }
                }
            }
        }
	}
}

void OcTree::OcTreeNode::updateChildren(OcTree& tree)
{
    Core::u32 newActiveChildrenIndex = 0;
    std::array<Core::i8, smMaxChildNumber> newActiveChildren;

    FOR_RANGE(i, 0, mActiveChildrenIndex)
    {
        Core::i8 childrenIndex = mActiveChildren[i];
        OcTreeNode* node = mChildren[childrenIndex];
        if(node)
        {
            if(node->getElementsCount() == 0)
            {
                if(node->mActiveChildrenIndex == 0)
                {
                    Core::Memory::deleteObject(node);
                    mChildren[childrenIndex] = nullptr;
                    continue;
                }
            }

            newActiveChildren[newActiveChildrenIndex] = childrenIndex;
            newActiveChildrenIndex++;
        }
	}
    mActiveChildren = newActiveChildren;
    mActiveChildrenIndex = newActiveChildrenIndex;

    FOR_RANGE(i, 0, mActiveChildrenIndex)
    {
        Core::i8 childrenIndex = mActiveChildren[i];
        OcTreeNode* node = mChildren[childrenIndex];
        if(node)
        {
            node->update(tree);
        }
	}
}

Core::u32 OcTree::OcTreeNode::getHash() const
{
    Core::u32 parentHash = 0;
    if(mParent)
    {
        parentHash = mParent->getHash();
    }

    return mIndex + parentHash + (mDepth > 0 ? std::pow(smMaxChildNumber, mDepth - 1) : 0);
}

void OcTree::OcTreeNode::drawDebug()
{
    PROFILER_CPU()

    // (DebugRenderer).drawCube(mCube,1,Maths::GeometricSpace::WORLD,Maths::Vector4(1,1,1,0.5f));
    // (DebugRenderer).drawLine(Maths::Cube(mCube.getLeftTopFront(), mCube.getLeftTopFront() + Maths::Vector3(0,10,0)),1,true,Maths::Vector4(0,1,0,1));
    // (DebugRenderer).drawCube(Maths::Cube(Maths::Vector3(0,0,-100), Maths::Vector3(100,100,100)),1,true,Maths::Vector4(1,0,1,1));
    // (DebugRenderer).drawLine(Maths::Cube(Maths::Vector3(0,0,-100), Maths::Vector3(0,0,-100) + Maths::Vector3(0,10,0)),1,true,Maths::Vector4(0,1,0,1));
    if (isDivisible())
	{
	}
    else // If is leaf node
	{
        // DEBUG DRAW
        if(mOcTreeElementsStatic.size() > 0 || mOcTreeElementsDynamic.size() > 0)
        {
		    // (DebugRenderer).drawCube(mCube,1,Maths::GeometricSpace::WORLD,Maths::Vector4(1,1,0,1));

            // FOR_RANGE(i,0,mOcTreeElementsStatic.size())
            // {
            //     Core::WeakPtr<IOcTreeElement> element = mOcTreeElementsStatic[i];
            //     (DebugRenderer).drawCube(element->getOcTreeBoundingBox(),1,true,Maths::Vector4(0,0.8,0.8,1));
            // }
            // FOR_RANGE(i,0,mOcTreeElementsDynamic.size())
            // {
            //     Core::WeakPtr<IOcTreeElement> element = mOcTreeElementsDynamic[i];
            //     (DebugRenderer).drawCube(element->getOcTreeBoundingBox(),1,true,Maths::Vector4(1,0,0,1));
            // }
        }
    }
}

void OcTree::init(Core::f32 size)
{
	mSize.set(size, size, size);
    mMaxDepth = 4;
	mRoot.init(this, nullptr, 0, Maths::Cube(Maths::Vector3(-mSize.x / 2.0f, mSize.y / 2.0f, mSize.z / 2.0f), mSize), 0);
}

void OcTree::update()
{
    PROFILER_CPU()
	mRoot.updateDynamicElements(*this);
	mRoot.update(*this);
}

void OcTree::addOcTreeElement(Core::WeakPtr<IOcTreeElement> element)
{
    PROFILER_CPU()
    mRoot.addOcTreeElement(element);
}

void IOcTreeElement::init(const Maths::Matrix4& modelMatrix, const Maths::Vector3& AABBMin, const Maths::Vector3& AABBMax, bool isStatic)
{
    PROFILER_CPU()

    mIsStatic = isStatic;
    Maths::Vector3 maxWorld = modelMatrix.mulVector(Maths::Vector4(AABBMax, 1));
    Maths::Vector3 minWorld = modelMatrix.mulVector(Maths::Vector4(AABBMin, 1));
    Maths::Vector3 diffWorld(maxWorld - minWorld);
    diffWorld.abs();
    Maths::Vector3 centerWorld = maxWorld - diffWorld/2.0f;
    Maths::Vector3 topLeft = centerWorld + (Maths::Vector3(-diffWorld.x,diffWorld.y,diffWorld.z)/2.0f);

    mOcTreeBoundingBox = Maths::Cube(topLeft, diffWorld);

    // FOR_RANGE(i, 0, OcTree::OcTreeNode::smMaxChildNumber)
    // {
    //     mNodes[i] = INVALID_INDEX;
    // }
}

// void IOcTreeElement::addNode(Core::u32 nodeHash)
// {
//     Core::u8 freeSlot = 0;
//     bool found = false;
//     FOR_RANGE(i, 0, OcTree::OcTreeNode::smMaxChildNumber)
//     {
//         found = mNodes[i] == (Core::i32)(nodeHash);
//         if(found)
//         {
//             break;
//         }

//         if(mNodes[i] == -1)
//         {
//             freeSlot = i;
//         }
//     }

//     if(!found)
//     {
//         mNodes[freeSlot] = nodeHash;
//     }
// }

// void IOcTreeElement::removeNode(Core::u32 nodeHash)
// {
//     FOR_RANGE(i, 0, OcTree::OcTreeNode::smMaxChildNumber)
//     {
//         if(mNodes[i] == (Core::i32)(nodeHash))
//         {
//             mNodes[i] = INVALID_INDEX;
//             break;
//         }
//     }
// }

// bool IOcTreeElement::isInNode(Core::u32 nodeHash) const
// {
//     bool found = false;
//     FOR_RANGE(i, 0, OcTree::OcTreeNode::smMaxChildNumber)
//     {
//         found = mNodes[i] == (Core::i32)(nodeHash);
//         if(found)
//         {
//             break;
//         }
//     }

//     return found;
// }
