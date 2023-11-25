#pragma once

#include "Core/Module.hpp"

class GPUVertexBuffersLayout;
class GPUUniformBlock;

class GPUShader: public ObjectBase
{
    GENERATE_METADATA(GPUShader)
public:

    GPUShader();
	~GPUShader() override = default;

    void initFromFilePaths(const std::string& vertex, const std::string& fragment);
    void initFromFileContents(const std::string& vertex, const std::string& fragment);
    void initDebug();
    void enable() const;
    void disable() const;
    void addMatrix(const Matrix4& matrix, const std::string& name);
    void addMatrixArray(const std::vector<Matrix4>&  matrixArray, const std::string& name);
    void addInt(i32 value, const std::string& name);
    void addUInt(u32 value, const std::string& name);
    void addFloat(f32 value, const std::string& name);
    void addVector4(const Vector4& value, const std::string& name);
    void addVector3(const Vector3& value, const std::string& name);
    void addVector2(const Vector2& value, const std::string& name);
    void addBool(bool value, const std::string& name);
    void bindUniformBlock(const GPUUniformBlock& uniformBlock);

private:
	u32 mVertexShader = 0;
	u32 mFragmentShader = 0;
	u32 mProgram = 0;
};
