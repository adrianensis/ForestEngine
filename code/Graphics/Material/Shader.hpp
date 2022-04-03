#pragma once

#include "Core/Module.hpp"

#ifdef CPP_INCLUDE
#include "Graphics/Material/Shader.hpp"
#include "Graphics/RenderContext.hpp"
#endif

class Shader: public ObjectBase
{
    GENERATE_METADATA(Shader)
private:
	u32 mVertexShader = 0;
	u32 mFragmentShader = 0;
	u32 mProgram = 0;

	inline static OwnerPtr<Shader> msShaderDefault;
	inline static Shader *msShaderDebug = nullptr;

	CPP void initInternal(const std::string& vertex, const std::string& fragment)
	{
		TRACE()

		mVertexShader = glCreateShader(GL_VERTEX_SHADER);

		std::ifstream vertex_ifs;
		vertex_ifs.open(vertex.c_str(), std::ifstream::in);
		std::string vertexShaderSource((std::istreambuf_iterator<char>(vertex_ifs)), (std::istreambuf_iterator<char>()));

		const char *c_str_vertex = vertexShaderSource.c_str();

		glShaderSource(mVertexShader, 1, &c_str_vertex, nullptr);
		glCompileShader(mVertexShader);

		int success;
		char infoLog[512];
		glGetShaderiv(mVertexShader, GL_COMPILE_STATUS, &success);

		if (!success)
		{
			glGetShaderInfoLog(mVertexShader, 512, nullptr, infoLog);
			std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
					<< infoLog << std::endl
					<< vertexShaderSource
					<< std::endl;
		}

		mFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

		std::ifstream fragment_ifs;
		fragment_ifs.open(fragment.c_str(), std::ifstream::in);
		std::string fragmentShaderSource((std::istreambuf_iterator<char>(fragment_ifs)),
										(std::istreambuf_iterator<char>()));
		const char *c_str_fragment = fragmentShaderSource.c_str();

		glShaderSource(mFragmentShader, 1, &c_str_fragment, nullptr);
		glCompileShader(mFragmentShader);

		glGetShaderiv(mFragmentShader, GL_COMPILE_STATUS, &success);

		if (!success)
		{
			glGetShaderInfoLog(mVertexShader, 512, nullptr, infoLog);
			std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
					<< infoLog << std::endl
					<< fragmentShaderSource
					<< std::endl;
		}

		mProgram = glCreateProgram();

		glAttachShader(mProgram, mVertexShader);
		glAttachShader(mProgram, mFragmentShader);
		glLinkProgram(mProgram);

		glGetProgramiv(mProgram, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(mProgram, 512, nullptr, infoLog);
			std::cout << "ERROR::SHADER::PROGRAM::LINK_FAILED\n"
					<< infoLog << std::endl;
		}

		glDeleteShader(mVertexShader);
		glDeleteShader(mFragmentShader);
	}

public:

	CPP Shader()
	{
		mVertexShader = -1;
		mFragmentShader = -1;
		mProgram = -1;
	};

	~Shader() override = default;

	CPP static Ptr<Shader> getDefaultShader()
	{
		if (!msShaderDefault)
		{
			msShaderDefault = OwnerPtr<Shader>(NEW(Shader));
			msShaderDefault.get().init();
		}

		return msShaderDefault;
	}

	CPP static Shader* getDebugShader()
	{
		if (!msShaderDebug)
		{
			msShaderDebug = NEW(Shader);
			msShaderDebug->initDebug();
		}
		return msShaderDebug;
	}

	CPP static void freeStaticShaders()
	{
		// if (msShaderDefault)
		// {
		// 	DELETE(msShaderDefault);
		// }

		if (msShaderDebug)
		{
			DELETE(msShaderDebug);
		}
	}

	CPP void init()
	{
		TRACE()

		initInternal("resources/shaders/vertex.shader", "resources/shaders/fragment.shader");
	}

	CPP void initDebug()
	{
		TRACE()

		initInternal("resources/shaders/vertexDebug.shader", "resources/shaders/fragmentDebug.shader");
	}

	CPP void enable()
	{
		glUseProgram(mProgram);
	}

	CPP void disable()
	{
		glUseProgram(0);
	}

	CPP void addMatrix(const Matrix4& matrix, const std::string& name)
	{
		u32 location = glGetUniformLocation(mProgram, name.c_str());
		//std::cout << "MATRIX LOCATION\n" << matrixLocation << std::endl;
		glUniformMatrix4fv(location, 1, GL_FALSE, matrix.getData());
	}

	CPP void addInt(i32 value, const std::string& name)
	{
		u32 location = glGetUniformLocation(mProgram, name.c_str());
		glUniform1i(location, value);
	}

	CPP void addUInt(u32 value, const std::string& name)
	{
		u32 location = glGetUniformLocation(mProgram, name.c_str());
		glUniform1ui(location, value);
	}

	CPP void addFloat(f32 value, const std::string& name)
	{
		u32 location = glGetUniformLocation(mProgram, name.c_str());
		glUniform1f(location, value);
	}

	CPP void addVector4(const Vector4& value, const std::string& name)
	{
		u32 location = glGetUniformLocation(mProgram, name.c_str());
		f32 vector[4] = {value.x, value.y, value.z, value.w};
		glUniform4fv(location, 1, vector);
	}

	CPP void addVector3(const Vector3& value, const std::string& name)
	{
		u32 location = glGetUniformLocation(mProgram, name.c_str());
		f32 vector[3] = {value.x, value.y, value.z};
		glUniform3fv(location, 1, vector);
	}

	CPP void addVector2(const Vector2& value, const std::string& name)
	{
		u32 location = glGetUniformLocation(mProgram, name.c_str());
		f32 vector[2] = {value.x, value.y};
		glUniform2fv(location, 1, vector);
	}

	CPP void addBool(bool value, const std::string& name)
	{
		u32 location = glGetUniformLocation(mProgram, name.c_str());
		glUniform1ui(location, value);
	}
};