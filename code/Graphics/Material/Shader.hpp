#ifndef SHADER_HPP
#define SHADER_HPP

#include "Core/Module.hpp"


class Shader: public ObjectBase
{
    GENERATE_METADATA(Shader)
public:

    Shader();
	~Shader() override = default;

    static Ptr<Shader> getDefaultShader();
    static Shader* getDebugShader();
    static void freeStaticShaders();
    void init();
    void initDebug();
    void enable();
    void disable();
    void addMatrix(const Matrix4& matrix, const std::string& name);
    void addInt(i32 value, const std::string& name);
    void addUInt(u32 value, const std::string& name);
    void addFloat(f32 value, const std::string& name);
    void addVector4(const Vector4& value, const std::string& name);
    void addVector3(const Vector3& value, const std::string& name);
    void addVector2(const Vector2& value, const std::string& name);
    void addBool(bool value, const std::string& name);

private:
    void initInternal(const std::string& vertex, const std::string& fragment);
	
private:
	u32 mVertexShader = 0;
	u32 mFragmentShader = 0;
	u32 mProgram = 0;

	inline static OwnerPtr<Shader> msShaderDefault;
	inline static Shader *msShaderDebug = nullptr;
};

#endif