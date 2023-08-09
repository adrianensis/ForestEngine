
#include "SpacePartition/OcTree.hpp"
#include "Graphics/Module.hpp"
#include "Scene/Module.hpp"

void OcTree::OcTreeNode::init(OcTreeNode* parent, const Cube& cube, f32 minSize)
{
    mParent = parent;
	mCube = cube;
    mMinSize = minSize;
	mHalfSize = mCube.getSize() / 2.0f;
	mIsDivisible = (mHalfSize.x >= mMinSize) and (mHalfSize.y >= mMinSize) and (mHalfSize.z >= mMinSize);

    FOR_RANGE(i, 0, smMaxChildNumber)
    {
        mActiveChildren[i] = -1;
    }

	if(mIsDivisible)
	{        
		// front
        mChildrenBoundingBoxes[0] = Cube(mCube.getLeftTopFront() + Vector3(0,0,-mHalfSize.z), mHalfSize);
        mChildrenBoundingBoxes[1] = Cube(mCube.getLeftTopFront() + Vector3(0,-mHalfSize.y,-mHalfSize.z), mHalfSize);
        mChildrenBoundingBoxes[2] = Cube(mCube.getLeftTopFront() + Vector3(mHalfSize.x,-mHalfSize.y,-mHalfSize.z), mHalfSize);
        mChildrenBoundingBoxes[3] = Cube(mCube.getLeftTopFront() + Vector3(mHalfSize.x,0,-mHalfSize.z), mHalfSize);
        
		// back
        mChildrenBoundingBoxes[4] = Cube(mCube.getLeftTopFront() + Vector3(0,0,0), mHalfSize);
        mChildrenBoundingBoxes[5] = Cube(mCube.getLeftTopFront() + Vector3(0,-mHalfSize.y,0), mHalfSize);
        mChildrenBoundingBoxes[6] = Cube(mCube.getLeftTopFront() + Vector3(mHalfSize.x,-mHalfSize.y,0), mHalfSize);
        mChildrenBoundingBoxes[7] = Cube(mCube.getLeftTopFront() + Vector3(mHalfSize.x,0,0), mHalfSize);
	}
}

void OcTree::OcTreeNode::addOcTreeElement(Ptr<IOcTreeElement> element)
{
    PROFILER_CPU()

    if (mIsDivisible)
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
        bool isPartiallyInChildren = Geometry::testCubeSphere(mChildrenBoundingBoxes[i],
			Sphere(element->getOcTreeBoundingBox().getCenter(), element->getOcTreeBoundingBox().getRadius()), 0);

        if (isPartiallyInChildren)
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

void OcTree::OcTreeNode::createChildren(u32 index)
{
    PROFILER_CPU()
    mChildren[index] = Memory::newObject<OcTreeNode>();
    mChildren[index]->init(this, mChildrenBoundingBoxes[index], mMinSize);
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
    auto it = std::find(octreeNodeElements->begin(), octreeNodeElements->end(), element);
    if(it == octreeNodeElements->end())
    {
        octreeNodeElements->push_back(element);
    }
}

void OcTree::OcTreeNode::addOcTreeElementToParent(Ptr<IOcTreeElement> element)
{
    PROFILER_CPU()

    bool test = Geometry::testSphereInsideCube(mCube,
        Sphere(element->getOcTreeBoundingBox().getCenter(), element->getOcTreeBoundingBox().getRadius()));

    if(test)
    {
        addOcTreeElement(element);
    }
    else
    {
        if(mParent)
        {
            mParent->addOcTreeElementToParent(element);
        }
    }
}

void OcTree::OcTreeNode::update(OcTree& tree/*contactManager*/)
{
    PROFILER_CPU()

    drawDebug();

	if (mIsDivisible)
	{
		updateChildren(tree/*contactManager*/);
	}
    else // If is leaf node
	{
        auto dynamicElementsCopyArray = mOcTreeElementsDynamic;
        mOcTreeElementsDynamic.clear();
        FOR_LIST(it, dynamicElementsCopyArray)
        {
            Ptr<IOcTreeElement> element = *it;
            bool test = Geometry::testSphereInsideCube(mCube,
            Sphere(element->getOcTreeBoundingBox().getCenter(), element->getOcTreeBoundingBox().getRadius()));

            if(test)
            {
                mOcTreeElementsDynamic.push_back(element);
            }
            else
            {
                mParent->addOcTreeElementToParent(*it);
            }
        }
	}
}

void OcTree::OcTreeNode::updateChildren(OcTree& tree/*contactManager*/)
{
    PROFILER_CPU()
    FOR_RANGE(i, 0, mActiveChildrenIndex)
    {
        i32 childrenIndex = mActiveChildren[i];
        OcTreeNode* node = mChildren[childrenIndex];
        if(node)
        {
            // if(node->mActiveChildren == 0)
            // {
            //     if(node->mIsDivisible)
            //     {
            //         Memory::deleteObject(node);
            //         mChildren[i] = nullptr;
            //         mActiveChildren--;
            //     }
            //     else
            //     {
            //         if(node->getElementsCount() == 0)
            //         {
            //             Memory::deleteObject(node);
            //             mChildren[i] = nullptr;
            //             mActiveChildren--;
            //         }
            //         else
            //         {
            //             node->update(tree/*contactManager*/);
            //         }
            //     }
            // }
            // else
            // {
                node->update(tree/*contactManager*/);
            // }
        }
	}
}

void OcTree::OcTreeNode::drawDebug()
{
    PROFILER_CPU()

    GET_SYSTEM(RenderEngine).drawLine(Line(mCube.getLeftTopFront(), mCube.getLeftTopFront() + Vector3(0,10,0)),1,true,Vector4(0,1,0,1));
    GET_SYSTEM(RenderEngine).drawCube(mCube,1,true,Vector4(1,1,1,0.5f));
    GET_SYSTEM(RenderEngine).drawCube(Cube(Vector3(0,0,-100), Vector3(100,100,100)),1,true,Vector4(1,0,1,1));
    GET_SYSTEM(RenderEngine).drawLine(Line(Vector3(0,0,-100), Vector3(0,0,-100) + Vector3(0,10,0)),1,true,Vector4(0,1,0,1));
    if (mIsDivisible)
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
	mMinSize = 400.0f; //size / 2.0f;
	mRoot.init(nullptr, Cube(Vector3(-mSize.x / 2.0f, mSize.y / 2.0f, mSize.z / 2.0f), mSize), mMinSize);
}

void OcTree::update()
{
    PROFILER_CPU()
	mRoot.update(*this);
}

void OcTree::addOcTreeElement(Ptr<IOcTreeElement> element)
{
    PROFILER_CPU()
    mRoot.addOcTreeElement(element);
}