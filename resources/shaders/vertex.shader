#version 420

uniform float time;

uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform vec4 positionOffset;
uniform bool isInstanced;
uniform bool hasAnimations;

const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;

uniform mat4 gBones[MAX_BONES];

//const int MAX_BONES = 100;
//uniform mat4 gBones[MAX_BONES];

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texcoord;
layout (location = 2) in vec4 color;
layout (location = 3) in mat4 modelMatrix;
layout (location = 7) in ivec4 BoneIDs;
layout (location = 8) in vec4 Weights;

out vec2 vTexcoord;
out vec4 vColor;
// flat out ivec4 BoneIDs0;
// out vec4 Weights0;

void main()
{
  vec4 totalPosition = vec4(position,1.0f);
  if(isInstanced && hasAnimations)
  {
  //   // mat4 BoneTransform = gBones[BoneIDs[0]] * Weights[0];
  //   // BoneTransform += gBones[BoneIDs[1]] * Weights[1];
  //   // BoneTransform += gBones[BoneIDs[2]] * Weights[2];
  //   // BoneTransform += gBones[BoneIDs[3]] * Weights[3];

    totalPosition = vec4(0.0f);
    for(int i = 0 ; i < MAX_BONE_INFLUENCE ; i++)
    {
        if(BoneIDs[i] == -1) 
            continue;
        if(BoneIDs[i] >= MAX_BONES) 
        {
            totalPosition = vec4(position,1.0f);
            break;
        }
        vec4 localPosition = gBones[BoneIDs[i]] * vec4(position,1.0f);
        totalPosition += localPosition * Weights[i];
    }
  }
  
  mat4 PV_Matrix = projectionMatrix * viewMatrix;

  vec4 finalPositon = totalPosition;
  //vec4 finalPositon = BoneTransform * vec4(position, 1.0);
  //vec4 finalPositon = vec4(position, 1.0);

  if(isInstanced)
  {
    finalPositon = modelMatrix * finalPositon;
  }
  
  gl_Position = PV_Matrix * finalPositon;

  // Pass data to the fragment shader.
  vTexcoord = texcoord;
  vColor = color;
}
