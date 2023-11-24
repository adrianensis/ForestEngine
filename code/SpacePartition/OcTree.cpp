
#include "SpacePartition/OcTree.hpp"
#include "Graphics/Module.hpp"
#include "Scene/Module.hpp"

void OcTree::OcTreeNode::init(OcTree* tree, OcTreeNode* parent, u8 index, const Cube& cube, u8 depth)
{
    mTree = tree;
    mParent = parent;
	mCube = cube;
	mDepth = depth;
	mIndex = index;

    FOR_RANGE(i, 0, smMaxChildNumber)
    {
        mActiveChildren[i] = -1;
    }

	if(isDivisible())
	{
	    Vector3 halfSize = mCube.getSize() / 2.0f;
		// front +z
            // left -x
                // up +y
                mChildrenBoundingBoxes[0] = Cube(mCube.getLeftTopFront() + Vector3(0,0,0), halfSize);
                // down -y
                mChildrenBoundingBoxes[1] = Cube(mCube.getLeftTopFront() + Vector3(0,-halfSize.y,0), halfSize);

            // right +x
                // up +y
                mChildrenBoundingBoxes[2] = Cube(mCube.getLeftTopFront() + Vector3(halfSize.x,0,0), halfSize);
                // down -y
                mChildrenBoundingBoxes[3] = Cube(mCube.getLeftTopFront() + Vector3(halfSize.x,-halfSize.y,0), halfSize);
                
		// back -z
            // left -x

                // up +y
                mChildrenBoundingBoxes[4] = Cube(mCube.getLeftTopFront() + Vector3(0,0,-halfSize.z), halfSize);
                // down -y
                mChildrenBoundingBoxes[5] = Cube(mCube.getLeftTopFront() + Vector3(0,-halfSize.y,-halfSize.z), halfSize);
                
            // right +x
                // up +y
                mChildrenBoundingBoxes[6] = Cube(mCube.getLeftTopFront() + Vector3(halfSize.x,0,-halfSize.z), halfSize);
                // down -y
                mChildrenBoundingBoxes[7] = Cube(mCube.getLeftTopFront() + Vector3(halfSize.x,-halfSize.y,-halfSize.z), halfSize);
        
	}
}

bool OcTree::OcTreeNode::isDivisible() const
{
    return mDepth < (mTree->mMaxDepth - 1);
}

bool OcTree::OcTreeNode::isElementEnclosed(Ptr<IOcTreeElement> element) const
{
    PROFILER_CPU()
    bool test = Geometry::testSphereInsideCube(mCube,
    Sphere(element->getOcTreeBoundingBox().getCenter(), element->getOcTreeBoundingBox().getRadius()));
    return test;
}

bool OcTree::OcTreeNode::isElementOverlappingChild(Ptr<IOcTreeElement> element, u8 childIndex) const
{
    PROFILER_CPU()
    bool test = Geometry::testCubeSphere(mChildrenBoundingBoxes[childIndex],
    Sphere(element->getOcTreeBoundingBox().getCenter(), element->getOcTreeBoundingBox().getRadius()), 0);
    return test;
}

void OcTree::OcTreeNode::addOcTreeElement(Ptr<IOcTreeElement> element)
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

void OcTree::OcTreeNode::addOcTreeElementToChildren(Ptr<IOcTreeElement> element)
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

void OcTree::OcTreeNode::createChildren(u8 index)
{
    PROFILER_CPU()
    mChildren[index] = Memory::newObject<OcTreeNode>();
    mChildren[index]->init(mTree, this, index, mChildrenBoundingBoxes[index], mDepth + 1);
}

void OcTree::OcTreeNode::addOcTreeElementToLeaf(Ptr<IOcTreeElement> element)
{
    PROFILER_CPU()
    auto* octreeNodeElements = &mOcTreeElementsStatic;
    if(!element->isOcTreeElementStatic())
    {
        octreeNodeElements = &mOcTreeElementsDynamic;
    }

    // Add Element to leaf node
    octreeNodeElements->push_back(element);
}

void OcTree::OcTreeNode::addOcTreeElementToParent(Ptr<IOcTreeElement> element)
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
            Ptr<IOcTreeElement> element = *it;
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
            i8 childrenIndex = mActiveChildren[i];
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
            Ptr<IOcTreeElement> element = *it;
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
    u32 newActiveChildrenIndex = 0;
    std::array<i8, smMaxChildNumber> newActiveChildren;

    FOR_RANGE(i, 0, mActiveChildrenIndex)
    {
        i8 childrenIndex = mActiveChildren[i];
        OcTreeNode* node = mChildren[childrenIndex];
        if(node)
        {
            if(node->getElementsCount() == 0)
            {
                if(node->mActiveChildrenIndex == 0)
                {
                    Memory::deleteObject(node);
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
        i8 childrenIndex = mActiveChildren[i];
        OcTreeNode* node = mChildren[childrenIndex];
        if(node)
        {
            node->update(tree);
        }
	}
}

u32 OcTree::OcTreeNode::getHash() const
{
    u32 parentHash = 0;
    if(mParent)
    {
        parentHash = mParent->getHash();
    }

    return mIndex + parentHash + (mDepth > 0 ? std::pow(smMaxChildNumber, mDepth - 1) : 0);
}

void OcTree::OcTreeNode::drawDebug()
{
    PROFILER_CPU()

    GET_SYSTEM(RenderEngine).drawCube(mCube,1,true,Vector4(1,1,1,0.5f));
    // GET_SYSTEM(RenderEngine).drawLine(Line(mCube.getLeftTopFront(), mCube.getLeftTopFront() + Vector3(0,10,0)),1,true,Vector4(0,1,0,1));
    // GET_SYSTEM(RenderEngine).drawCube(Cube(Vector3(0,0,-100), Vector3(100,100,100)),1,true,Vector4(1,0,1,1));
    // GET_SYSTEM(RenderEngine).drawLine(Line(Vector3(0,0,-100), Vector3(0,0,-100) + Vector3(0,10,0)),1,true,Vector4(0,1,0,1));
    if (isDivisible())
	{
	}
    else // If is leaf node
	{
        // DEBUG DRAW
        if(mOcTreeElementsStatic.size() > 0 || mOcTreeElementsDynamic.size() > 0)
        {
		    GET_SYSTEM(RenderEngine).drawCube(mCube,1,true,Vector4(1,1,0,1));

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
    }
}

void OcTree::init(f32 size)
{
	mSize.set(size, size, size);
    mMaxDepth = 4;
	mRoot.init(this, nullptr, 0, Cube(Vector3(-mSize.x / 2.0f, mSize.y / 2.0f, mSize.z / 2.0f), mSize), 0);
}

void OcTree::update()
{
    PROFILER_CPU()
	mRoot.updateDynamicElements(*this);
	mRoot.update(*this);
}

void OcTree::addOcTreeElement(Ptr<IOcTreeElement> element)
{
    PROFILER_CPU()
    mRoot.addOcTreeElement(element);
}

void IOcTreeElement::init(const Matrix4& modelMatrix, const Vector3& AABBMin, const Vector3& AABBMax)
{
    PROFILER_CPU()

    Vector3 maxWorld = modelMatrix.mulVector(Vector4(AABBMax, 1));
    Vector3 minWorld = modelMatrix.mulVector(Vector4(AABBMin, 1));
    Vector3 diffWorld(maxWorld - minWorld);
    diffWorld.abs();
    Vector3 centerWorld = maxWorld - diffWorld/2.0f;
    Vector3 topLeft = centerWorld + (Vector3(-diffWorld.x,diffWorld.y,diffWorld.z)/2.0f);

    mOcTreeBoundingBox = Cube(topLeft, diffWorld);

    // FOR_RANGE(i, 0, OcTree::OcTreeNode::smMaxChildNumber)
    // {
    //     mNodes[i] = -1;
    // }
}

// void IOcTreeElement::addNode(u32 nodeHash)
// {
//     u8 freeSlot = 0;
//     bool found = false;
//     FOR_RANGE(i, 0, OcTree::OcTreeNode::smMaxChildNumber)
//     {
//         found = mNodes[i] == (i32)(nodeHash);
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

// void IOcTreeElement::removeNode(u32 nodeHash)
// {
//     FOR_RANGE(i, 0, OcTree::OcTreeNode::smMaxChildNumber)
//     {
//         if(mNodes[i] == (i32)(nodeHash))
//         {
//             mNodes[i] = -1;
//             break;
//         }
//     }
// }

// bool IOcTreeElement::isInNode(u32 nodeHash) const
// {
//     bool found = false;
//     FOR_RANGE(i, 0, OcTree::OcTreeNode::smMaxChildNumber)
//     {
//         found = mNodes[i] == (i32)(nodeHash);
//         if(found)
//         {
//             break;
//         }
//     }

//     return found;
// }
