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
    void addMatrix(CR(Matrix4) matrix, CR(std::string) name);
    void addMatrixArray(CR(std::vector<Matrix4>) matrixArray, CR(std::string) name);
    void addInt(i32 value, CR(std::string) name);
    void addUInt(u32 value, CR(std::string) name);
    void addFloat(f32 value, CR(std::string) name);
    void addVector4(CR(Vector4) value, CR(std::string) name);
    void addVector3(CR(Vector3) value, CR(std::string) name);
    void addVector2(CR(Vector2) value, CR(std::string) name);
    void addBool(bool value, CR(std::string) name);

private:
    void initInternal(CR(std::string) vertex, CR(std::string) fragment);
	
private:
	u32 mVertexShader = 0;
	u32 mFragmentShader = 0;
	u32 mProgram = 0;

	inline static OwnerPtr<Shader> msShaderDefault;
	inline static Shader *msShaderDebug = nullptr;
};

#endif