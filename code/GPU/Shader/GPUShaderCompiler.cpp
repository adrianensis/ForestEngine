#include "GPU/Shader/GPUShaderCompiler.hpp"

// INFO: from here https://github.com/KhronosGroup/glslang?tab=readme-ov-file#c-functional-interface-new
#include <glslang/Include/glslang_c_interface.h>
// Required for use of glslang_default_resource
#include <glslang/Public/resource_limits_c.h>

SPIRVBinary GPUShaderCompiler::compileGPUShaderToSPIRV(GPUPipelineStage stage, const char* shaderSource, const char* fileName)
{
    glslang_stage_t glsl_stage;
    switch (stage)
    {
        case GPUPipelineStage::VERTEX:
            glsl_stage = glslang_stage_t::GLSLANG_STAGE_VERTEX;
            break;
        case GPUPipelineStage::FRAGMENT:
            glsl_stage = glslang_stage_t::GLSLANG_STAGE_FRAGMENT;
            break;
        case GPUPipelineStage::GEOMETRY:
            glsl_stage = glslang_stage_t::GLSLANG_STAGE_GEOMETRY;
            break;
        case GPUPipelineStage::TESS_CONTROL:
            glsl_stage = glslang_stage_t::GLSLANG_STAGE_TESSCONTROL;
            break;
        case GPUPipelineStage::TESS_EVALUATION:
            glsl_stage = glslang_stage_t::GLSLANG_STAGE_TESSEVALUATION;
            break;
        case GPUPipelineStage::COMPUTE:
            glsl_stage = glslang_stage_t::GLSLANG_STAGE_COMPUTE;
            break;
        default:
            CHECK_MSG(false, "Unsupported shader stage.")
            break;
    };

    const glslang_input_t input = {
        .language = GLSLANG_SOURCE_GLSL,
        .stage = glsl_stage,
        .client = GLSLANG_CLIENT_VULKAN,
        .client_version = GLSLANG_TARGET_VULKAN_1_1,
        .target_language = GLSLANG_TARGET_SPV,
        .target_language_version = GLSLANG_TARGET_SPV_1_3,
        .code = shaderSource,
        .default_version = 100,
        .default_profile = GLSLANG_NO_PROFILE,
        .force_default_version_and_profile = false,
        .forward_compatible = false,
        .messages = GLSLANG_MSG_DEFAULT_BIT,
        .resource = glslang_default_resource(),
    };

    glslang_shader_t* shader = glslang_shader_create(&input);

    SPIRVBinary bin;

    if (!glslang_shader_preprocess(shader, &input))	{
        printf("GLSL preprocessing failed %s\n", fileName);
        printf("%s\n", glslang_shader_get_info_log(shader));
        printf("%s\n", glslang_shader_get_info_debug_log(shader));
        printf("%s\n", input.code);
        glslang_shader_delete(shader);
        return bin;
    }

    if (!glslang_shader_parse(shader, &input)) {
        printf("GLSL parsing failed %s\n", fileName);
        printf("%s\n", glslang_shader_get_info_log(shader));
        printf("%s\n", glslang_shader_get_info_debug_log(shader));
        printf("%s\n", glslang_shader_get_preprocessed_code(shader));
        glslang_shader_delete(shader);
        return bin;
    }

    glslang_program_t* program = glslang_program_create();
    glslang_program_add_shader(program, shader);

    if (!glslang_program_link(program, GLSLANG_MSG_SPV_RULES_BIT | GLSLANG_MSG_VULKAN_RULES_BIT)) {
        printf("GLSL linking failed %s\n", fileName);
        printf("%s\n", glslang_program_get_info_log(program));
        printf("%s\n", glslang_program_get_info_debug_log(program));
        glslang_program_delete(program);
        glslang_shader_delete(shader);
        return bin;
    }

    glslang_program_SPIRV_generate(program, glsl_stage);

    bin.mSize = glslang_program_SPIRV_get_size(program);
    bin.mWords = new u32[bin.mSize];
    glslang_program_SPIRV_get(program, bin.mWords);

    const char* spirv_messages = glslang_program_SPIRV_get_messages(program);
    if (spirv_messages)
        printf("(%s) %s\b", fileName, spirv_messages);

    glslang_program_delete(program);
    glslang_shader_delete(shader);

    return bin;
}

void GPUShaderCompiler::deleteSPIRVBinary(SPIRVBinary& spirvBinary)
{
    if(spirvBinary.mWords)
    {
        delete[] spirvBinary.mWords;
        spirvBinary.mWords = nullptr;
    }
}
