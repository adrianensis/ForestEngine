#include "Graphics/Shaders/ShaderBuilder.hpp"
#include "Graphics/RenderContext.hpp"
#include "Graphics/Buffers/GPUBuffersLayout.hpp"

ShaderBuilder::ShaderBuilder()
{

}

void ShaderBuilder::createShader(const GPUBuffersLayout& gpuBuffersLayout, Ptr<const Material> material)
{
    const std::vector<GPUBuffer>& gpuBuffers = gpuBuffersLayout.getBuffers();

    using namespace ShaderBuilderNodes;
    using namespace ShaderBuilderNodes::Expressions;

    FOR_LIST(it, gpuBuffers)
    {
        const GPUBuffer& gpuBuffer = *it;
        get().attribute(GPUStorage::IN, gpuBuffer.getAttributeLocation(), {"vec3", gpuBuffer.mData.mAttributeName});
    }

    get().attribute(GPUStorage::UNIFORM, {"float", "time"});
    get().attribute(GPUStorage::UNIFORM, {"mat4", "viewMatrix"});
    get().attribute(GPUStorage::UNIFORM, {"mat4", "projectionMatrix"});
    get().attribute(GPUStorage::UNIFORM, {"vec4", "positionOffset"});
    auto& isInstanced = get().attribute(GPUStorage::UNIFORM, {"bool", "isInstanced"});
    auto& hasAnimations = get().attribute(GPUStorage::UNIFORM, {"bool", "hasAnimations"});

    auto& MAX_BONES = get().attribute(GPUStorage::CONST, {"int", "MAX_BONES", "100"});
    auto& MAX_BONE_INFLUENCE = get().attribute(GPUStorage::CONST, {"int", "MAX_BONE_INFLUENCE", "4"});

    auto& gBones = get().attribute(GPUStorage::UNIFORM, {"bool", "gBones", "", MAX_BONES});

    // auto& pos = get().attribute(GPUStorage::IN, 0, {"vec3", "position"});
    // get().attribute(GPUStorage::IN, 1, {"vec2", "texcoord"});
    // get().attribute(GPUStorage::IN, 2, {"vec4", "color"});
    // get().attribute(GPUStorage::IN, 3, {"mat4", "modelMatrix"});
    // auto& bonesIDs = get().attribute(GPUStorage::IN, 7, {"ivec4", "BoneIDs"});
    // auto& Weights = get().attribute(GPUStorage::IN, 8, {"vec4", "Weights"});

    get().attribute(GPUStorage::OUT, {"vec2", "vTexcoord"});
    get().attribute(GPUStorage::OUT, {"vec4", "vColor"});

    auto& mainFunc = get().function("void", "main");

    Variable totalPosition;
    Variable localPosition;
    
    mainFunc.body().
    variable(totalPosition, "vec4", "totalPosition", call("vec4", {get().getAttribute("position"), {"1.0f"}})).
    ifBlock(isInstanced, "&&", hasAnimations).
        set(totalPosition, call("vec4", {{"0.0f"}})).
        forBlock("i", "<", MAX_BONE_INFLUENCE, "++").
            ifBlock(get().getAttribute("BoneIDs").at("i"), "==", {"-1"}).
                line("continue").
            end().
            ifBlock(get().getAttribute("BoneIDs").at("i"), ">=", MAX_BONES).
                set(totalPosition, call("vec4", {get().getAttribute("position"), {"1.0f"}})).
                line("break").
            end().
            variable(localPosition, "vec4", "localPosition", gBones.at(get().getAttribute("BoneIDs").at("i")).mul(call("vec4", {get().getAttribute("position"), {"1.0f"}})).getNameOrValue()).
            set(totalPosition, totalPosition.add(localPosition.mul(get().getAttribute("Weights").at("i")))).
        end().
    end();
}

std::string ShaderBuilder::getCode() const
{
    std::string code = "";
    auto codeLines = mProgram.toLines(0);
    FOR_LIST(it, codeLines)
    {
        code.append((*it) + "\n");
    }

    return code;
}