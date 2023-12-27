#include "Graphics/Mesh/Mesh.hpp"
#include "Graphics/Model/Model.hpp"
#include "Graphics/Model/Animation/AnimationManager.hpp"

void Mesh::addBoneWeight(u32 vertexId, i32 id, f32 weight)
{
	BoneVertexIDsData& boneVertexIDsData = mBonesVertexIDsData[vertexId]; //.setBoneWeight(id, weight);
	BoneVertexWeightsData& boneVertexWeightsData = mBonesVertexWeightsData[vertexId]; //.setBoneWeight(id, weight);

    FOR_RANGE(i, 0, smMaxBonesPerVertex)
	{
		if (boneVertexIDsData.mBonesIDs[i] == -1)
		{
			boneVertexIDsData.mBonesIDs[i] = id;
			boneVertexWeightsData.mBonesWeights[i] = weight;
			//printf("bone %d weight %f index %i\n", id, weight, i);
			return;
		}
	}

	// should never get here - more bones than we have space for
	CHECK_MSG(false, "should never get here - more bones than we have space for");
}

std::vector<Vector3> Mesh::calculateSkinnedVertices() const
{
    std::vector<Vector3> skinnedVertices;
    skinnedVertices.resize(mVertexCount);

    FOR_RANGE(i, 0, mVertexCount)
    {
        skinnedVertices[i] = calculateSkinnedVertex(i);
    }

    return skinnedVertices;
}

Vector3 Mesh::calculateSkinnedVertex(u32 i) const
{
    Vector3 vertexPosition = mPositions[i];

    const std::vector<Matrix4>& boneTransforms = GET_SYSTEM(AnimationManager).getBoneTransforms(mModel);

    Vector4 skinnedVertexPosition = Vector4(0,0,0,0);
    for(u32 boneIt = 0 ; boneIt < (i32)MAX_BONE_INFLUENCE ; boneIt++)
    {
        const BoneVertexIDsData& IDsData = mBonesVertexIDsData[i];
        const BoneVertexWeightsData& wightsData = mBonesVertexWeightsData[i];
        if(IDsData.mBonesIDs[boneIt] > -1)
        {
            if(IDsData.mBonesIDs[boneIt] >= (i32)MAX_BONES) 
            {
                skinnedVertexPosition = vertexPosition;
                CHECK_MSG(false, "MAX_BONES!");
                break;
            }

            Vector4 localPosition = boneTransforms[IDsData.mBonesIDs[boneIt]].mulVector(Vector4(vertexPosition,1.0f));
            skinnedVertexPosition += localPosition * wightsData.mBonesWeights[boneIt];
        }
    }

    // NOTE: normalize by w! this is bc we cannot assume w == 1 after bone transforms multiplications
    if(skinnedVertexPosition.w > 0)
    {
        skinnedVertexPosition = skinnedVertexPosition / skinnedVertexPosition.w;
    }

    return skinnedVertexPosition;
}