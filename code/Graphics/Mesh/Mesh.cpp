#include "Graphics/Mesh/Mesh.hpp"
#include "Graphics/Model/Model.hpp"

void BoneVertexData::addBoneData(i32 id, f32 weight)
{
	FOR_RANGE(i, 0, smMaxBonesPerVertex)
	{
		if (mBoneIDs[i] == -1)
		{
			mBoneIDs[i] = id;
			mBoneWeights[i] = weight;
			//printf("bone %d weight %f index %i\n", id, weight, i);
			return;
		}
	}

	// should never get here - more bones than we have space for
	//ASSERT_MSG(false, "should never get here - more bones than we have space for");
}

Mesh::~Mesh()
{
}

void Mesh::init(u32 vertexCount, u32 facesCount)
{
	mVertexCount = vertexCount;
	mFacesCount = facesCount;

	clear();
}

void Mesh::addPosition(const Vector3& pos)
{
	mPositions.push_back(pos);
}

void Mesh::addNormal(const Vector3& vector)
{
	mNormals.push_back(vector.x);
	mNormals.push_back(vector.y);
	mNormals.push_back(vector.z);
}

void Mesh::addTexCoord(const Vector2& texCoord)
{
	mTextureCoordinates.push_back(texCoord);
}

void Mesh::addColor(const Vector4& color)
{
	mColors.push_back(color);
}

void Mesh::addFace(const Face& face)
{
	mFaces.push_back(face);
}

void Mesh::addBoneVertexData(const BoneVertexData& bone)
{
	mBonesVertexData.push_back(bone);
}

void Mesh::addPositions(const std::vector<Vector3>& vec)
{
	mPositions.insert(mPositions.end(), vec.begin(), vec.end());
}

void Mesh::addNormals(const std::vector<f32>& vec)
{
	mNormals.insert(mNormals.end(), vec.begin(), vec.end());
}

void Mesh::addTextureCoordinates(const std::vector<Vector2>& vec)
{
	mTextureCoordinates.insert(mTextureCoordinates.end(), vec.begin(), vec.end());
}

void Mesh::addColors(const std::vector<Vector4>& vec)
{
	mColors.insert(mColors.end(), vec.begin(), vec.end());
}

void Mesh::addFaces(const std::vector<Face>& vec)
{
	mFaces.insert(mFaces.end(), vec.begin(), vec.end());
}

void Mesh::addBonesVertexData(const std::vector<BoneVertexData>& vec)
{
	mBonesVertexData.insert(mBonesVertexData.end(), vec.begin(), vec.end());
}

void Mesh::addBoneWeight(u32 vertexId, i32 id, f32 weight)
{
	mBonesVertexData[vertexId].addBoneData(id, weight);
}

void Mesh::copyPositions(Ptr<const Mesh> other)
{
	std::copy(other.get().getPositions().begin(), other.get().getPositions().end(), back_inserter(mPositions));
}

void Mesh::copyNormals(Ptr<const Mesh> other)
{
	std::copy(other.get().getNormals().begin(), other.get().getNormals().end(), back_inserter(mNormals));
}

void Mesh::copyTextureCoordinates(Ptr<const Mesh> other)
{
	std::copy(other.get().getTextureCoordinates().begin(), other.get().getTextureCoordinates().end(), back_inserter(mTextureCoordinates));
}

void Mesh::copyColors(Ptr<const Mesh> other)
{
	std::copy(other.get().getColors().begin(), other.get().getColors().end(), back_inserter(mColors));
}

void Mesh::copyFaces(Ptr<const Mesh> other)
{
	std::copy(other.get().getFaces().begin(), other.get().getFaces().end(), back_inserter(mFaces));
}

void Mesh::copyBones(Ptr<const Mesh> other)
{
	std::copy(other.get().getBonesVertexData().begin(), other.get().getBonesVertexData().end(), back_inserter(mBonesVertexData));
}

void Mesh::clear()
{
	mPositions.clear();
	mNormals.clear();
	mTextureCoordinates.clear();
	mColors.clear();
	mFaces.clear();
	mBonesVertexData.clear();

	mPositions.reserve(mVertexCount);
	mTextureCoordinates.reserve(mVertexCount);
	mColors.reserve(mVertexCount);
	mNormals.reserve(mVertexCount * smVertexNormalSize);
	mFaces.reserve(mFacesCount);
	mBonesVertexData.reserve(mVertexCount);

	std::fill(mColors.begin(), mColors.end(), Vector4(0,0,0,0));

	mMaterialPath.clear();
}