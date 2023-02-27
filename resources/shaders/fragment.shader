// #version 420

// precision mediump float;

// out vec4 FragColor;

// // Passed in from the vertex shader.
// in vec2 vTexcoord;
// in vec4 vColor;
// // flat in ivec4 BoneIDs0;
// // in vec4 Weights0;

// uniform float time;

// uniform bool alphaEnabled;

// uniform sampler2D uSampler;

// uniform bool hasTexture;
// uniform bool hasBorder;
// uniform vec2 clipRegionLeftTop;
// uniform vec2 clipRegionSize;
// uniform vec2 windowSize;

// float rand(vec2 co){
//     return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
// }

// void main()
// {
//     float aspect = windowSize.x / windowSize.y;
//     vec2 t = vTexcoord;

//     if(hasTexture) {

//       FragColor = texture2D(uSampler, t);
      
//       if(alphaEnabled && (FragColor.r + FragColor.g + FragColor.b) == 0){
//         discard;
//       } else {
//         FragColor.r += vColor.r;
//         FragColor.g += vColor.g;
//         FragColor.b += vColor.b;
//         FragColor.a = vColor.a;
//       }

//     } else {
//         FragColor.r = vColor.r;
//         FragColor.g = vColor.g;
//         FragColor.b = vColor.b;
//         FragColor.a = vColor.a;
//     }

//     if(clipRegionSize.x > 0.0000001 && clipRegionSize.y > 0.0000001)
//     {
//       bool isInClipRectangle = gl_FragCoord.x > ((clipRegionLeftTop.x*(windowSize.x/2.0)) + (windowSize.x/2.0)) &&
//       gl_FragCoord.x < ((clipRegionLeftTop.x*(windowSize.x/2.0)) + (windowSize.x/2.0) + (clipRegionSize.x)*(windowSize.x/2.0)) &&
//       gl_FragCoord.y > clipRegionLeftTop.y*(windowSize.y/2.0) - clipRegionSize.y*(windowSize.y/2.0) + (windowSize.y/2.0) &&
//       gl_FragCoord.y < clipRegionLeftTop.y*(windowSize.y/2.0) - clipRegionSize.y*(windowSize.y/2.0) + (windowSize.y/2.0) + clipRegionSize.y*(windowSize.y/2.0);
      
//       if(!isInClipRectangle)
//       {
//         discard;
//       }
//     }
// }

#version 420
precision mediump float;
const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;
uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform bool isInstanced;
uniform bool isAnimated;
uniform bool hasTexture;
uniform bool alphaEnabled;
uniform bool hasBorder;
uniform float time;
uniform vec2 windowSize;
uniform mat4 bonesTransform[MAX_BONES];
uniform sampler2D uSampler;
in vec2 vTexcoord;
in vec4 vColor;
out vec4 FragColor;
void main()
{
    vec2 t = vTexcoord;
    if(hasTexture)
    {
        FragColor=texture2D(uSampler,t);
        if(alphaEnabled&&FragColor.r+FragColor.g+FragColor.b==0)
        {
            discard;
        }
        else
        {
            FragColor.r=FragColor.r+vColor.r;
            FragColor.g=FragColor.g+vColor.g;
            FragColor.b=FragColor.b+vColor.b;
            FragColor.a=vColor.a;
        }
    }
    else
    {
        FragColor.r=vColor.r;
        FragColor.g=vColor.g;
        FragColor.b=vColor.b;
        FragColor.a=vColor.a;
    }
}