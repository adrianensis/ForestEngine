
#include "SpacePartition/OcTree.hpp"
#include "Graphics/Module.hpp"
#include "Scene/Module.hpp"

void OcTree::OcTreeNode::init(OcTreeNode* parent, u8 index, const Cube& cube, u8 maxDepth, u8 depth)
{
    mParent = parent;
    mIndex = index;
	mCube = cube;
	mMaxDepth = maxDepth;
	mDepth = depth;
	mHalfSize = mCube.getSize() / 2.0f;
	mIsDivisible = depth < maxDepth;

    FOR_RANGE(i, 0, smMaxChildNumber)
    {
        mActiveChildren[i] = -1;
    }

	if(mIsDivisible)
	{
        u8 index = 0;
		// front +z
            // left -x
                // up +y
                mChildrenBoundingBoxes[0] = Cube(mCube.getLeftTopFront() + Vector3(0,0,0), mHalfSize);
                // down -y
                mChildrenBoundingBoxes[1] = Cube(mCube.getLeftTopFront() + Vector3(0,-mHalfSize.y,0), mHalfSize);

            // right +x
                // up +y
                mChildrenBoundingBoxes[2] = Cube(mCube.getLeftTopFront() + Vector3(mHalfSize.x,0,0), mHalfSize);
                // down -y
                mChildrenBoundingBoxes[3] = Cube(mCube.getLeftTopFront() + Vector3(mHalfSize.x,-mHalfSize.y,0), mHalfSize);
                
		// back -z
            // left -x

                // up +y
                mChildrenBoundingBoxes[4] = Cube(mCube.getLeftTopFront() + Vector3(0,0,-mHalfSize.z), mHalfSize);
                // down -y
                mChildrenBoundingBoxes[5] = Cube(mCube.getLeftTopFront() + Vector3(0,-mHalfSize.y,-mHalfSize.z), mHalfSize);
                
            // right +x
                // up +y
                mChildrenBoundingBoxes[6] = Cube(mCube.getLeftTopFront() + Vector3(mHalfSize.x,0,-mHalfSize.z), mHalfSize);
                // down -y
                mChildrenBoundingBoxes[7] = Cube(mCube.getLeftTopFront() + Vector3(mHalfSize.x,-mHalfSize.y,-mHalfSize.z), mHalfSize);
        
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

void OcTree::OcTreeNode::createChildren(u8 index)
{
    PROFILER_CPU()
    mChildren[index] = Memory::newObject<OcTreeNode>();
    mChildren[index]->init(this, index, mChildrenBoundingBoxes[index], mMaxDepth, mDepth + 1);
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
                if(mParent)
                {
                    mParent->addOcTreeElementToParent(*it);
                }
            }
        }
	}
}

void OcTree::OcTreeNode::updateChildren(OcTree& tree/*contactManager*/)
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
            node->update(tree/*contactManager*/);
        }
	}
}

void OcTree::OcTreeNode::drawDebug()
{
    PROFILER_CPU()

    GET_SYSTEM(RenderEngine).drawCube(mCube,1,true,Vector4(1,1,1,0.5f));
    // GET_SYSTEM(RenderEngine).drawLine(Line(mCube.getLeftTopFront(), mCube.getLeftTopFront() + Vector3(0,10,0)),1,true,Vector4(0,1,0,1));
    // GET_SYSTEM(RenderEngine).drawCube(Cube(Vector3(0,0,-100), Vector3(100,100,100)),1,true,Vector4(1,0,1,1));
    // GET_SYSTEM(RenderEngine).drawLine(Line(Vector3(0,0,-100), Vector3(0,0,-100) + Vector3(0,10,0)),1,true,Vector4(0,1,0,1));
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
    mMaxDepth = 4;
	//f32 minSize = size / (mMaxDepth * 2);
	mRoot.init(nullptr, 0, Cube(Vector3(-mSize.x / 2.0f, mSize.y / 2.0f, mSize.z / 2.0f), mSize), mMaxDepth, 1);
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

void IOcTreeElement::init(const Matrix4& modelMatrix, const Vector3& meshMin, const Vector3& meshMax)
{
    Vector3 maxWorld = modelMatrix.mulVector(Vector4(meshMax, 1));
    Vector3 minWorld = modelMatrix.mulVector(Vector4(meshMin, 1));
    Vector3 diffWorld(maxWorld - minWorld);
    diffWorld.abs();
    Vector3 centerWorld = maxWorld - diffWorld/2.0f;
    Vector3 topLeft = centerWorld + (Vector3(-diffWorld.x,diffWorld.y,diffWorld.z)/2.0f);

    mOcTreeBoundingBox = Cube(topLeft, diffWorld);
}