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
    void addMatrix(const Matrix4& matrix, const String& name);
    void addInt(i32 value, const String& name);
    void addUInt(u32 value, const String& name);
    void addFloat(f32 value, const String& name);
    void addVector4(const Vector4& value, const String& name);
    void addVector3(const Vector3& value, const String& name);
    void addVector2(const Vector2& value, const String& name);
    void addBool(bool value, const String& name);

private:
    void initInternal(const String& vertex, const String& fragment);
	
private:
	u32 mVertexShader = 0;
	u32 mFragmentShader = 0;
	u32 mProgram = 0;

	inline static OwnerPtr<Shader> msShaderDefault;
	inline static Shader *msShaderDebug = nullptr;
};

#endif