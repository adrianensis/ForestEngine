#version 420

uniform float time;

uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform vec4 positionOffset;
uniform bool isInstanced;
uniform bool isAnimated;

const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;

uniform mat4 bonesTransform[MAX_BONES];

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texcoord;
layout (location = 2) in vec4 color;
layout (location = 3) in mat4 modelMatrix;
layout (location = 7) in ivec4 BoneIDs;
layout (location = 8) in vec4 Weights;

out vec2 vTexcoord;
out vec4 vColor;

vec4 calculateSkinnedPosition(in vec3 pos)
{
  vec4 finalPositon = vec4(pos,1.0f);
  if(isInstanced && isAnimated)
  {
    finalPositon = vec4(0.0f);
    for(int i = 0 ; i < MAX_BONE_INFLUENCE ; i++)
    {
        if(BoneIDs[i] == -1) 
            continue;
        if(BoneIDs[i] >= MAX_BONES) 
        {
            finalPositon = vec4(pos,1.0f);
            break;
        }
        vec4 localPosition = bonesTransform[BoneIDs[i]] * vec4(pos,1.0f);
        finalPositon += localPosition * Weights[i];
    }
  }

  return finalPositon;
}

void main()
{
  vec4 finalPositon = calculateSkinnedPosition(position);
  
  mat4 PV_Matrix = projectionMatrix * viewMatrix;

  if(isInstanced)
  {
    finalPositon = modelMatrix * finalPositon;
  }
  
  gl_Position = PV_Matrix * finalPositon;

  // Pass data to the fragment shader.
  vTexcoord = texcoord;
  vColor = color;
}
