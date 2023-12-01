#include "Graphics/GPU/GPUShader.hpp"
#include "Graphics/GPU/GPUVertexBuffersLayout.hpp"
#include "Graphics/GPU/GPUUniformBlock.hpp"

GPUShader::GPUShader()
{
};

void GPUShader::initDebug()
{
	TRACE()

	initFromFilePaths("resources/shaders/vertexDebug.shader", "resources/shaders/fragmentDebug.shader");
}

void GPUShader::enable() const
{
	glUseProgram(mProgram);
}

void GPUShader::disable() const
{
	glUseProgram(0);
}

void GPUShader::addMatrix(const Matrix4& matrix, const std::string& name)
{
	u32 location = glGetUniformLocation(mProgram, name.c_str());
	glUniformMatrix4fv(location, 1, GL_FALSE, matrix.getData());
}

void GPUShader::addMatrixArray(const std::vector<Matrix4>&  matrixArray, const std::string& name)
{
	u32 location = glGetUniformLocation(mProgram, name.c_str());
	glUniformMatrix4fv(location, matrixArray.size(), GL_FALSE, reinterpret_cast<const GLfloat*>(matrixArray.data()));
}

void GPUShader::addInt(i32 value, const std::string& name)
{
	u32 location = glGetUniformLocation(mProgram, name.c_str());
	glUniform1i(location, value);
}

void GPUShader::addUInt(u32 value, const std::string& name)
{
	u32 location = glGetUniformLocation(mProgram, name.c_str());
	glUniform1ui(location, value);
}

void GPUShader::addFloat(f32 value, const std::string& name)
{
	u32 location = glGetUniformLocation(mProgram, name.c_str());
	glUniform1f(location, value);
}

void GPUShader::addVector4(const Vector4& value, const std::string& name)
{
	u32 location = glGetUniformLocation(mProgram, name.c_str());
	f32 vector[4] = {value.x, value.y, value.z, value.w};
	glUniform4fv(location, 1, vector);
}

void GPUShader::addVector3(const Vector3& value, const std::string& name)
{
	u32 location = glGetUniformLocation(mProgram, name.c_str());
	f32 vector[3] = {value.x, value.y, value.z};
	glUniform3fv(location, 1, vector);
}

void GPUShader::addVector2(const Vector2& value, const std::string& name)
{
	u32 location = glGetUniformLocation(mProgram, name.c_str());
	f32 vector[2] = {value.x, value.y};
	glUniform2fv(location, 1, vector);
}

void GPUShader::addBool(bool value, const std::string& name)
{
	u32 location = glGetUniformLocation(mProgram, name.c_str());
	glUniform1ui(location, value);
}

void GPUShader::bindUniformBlock(const GPUUniformBlock& uniformBlock)
{
    u32 location = glGetUniformBlockIndex(mProgram, uniformBlock.getGPUUniformBlockData().mBlockName.c_str());   
    glUniformBlockBinding(mProgram, location, uniformBlock.getBindingPoint());
}

void GPUShader::initFromFileContents(const std::string& vertex, const std::string& fragment)
{
    mVertexShader = glCreateShader(GL_VERTEX_SHADER);

	const char *c_str_vertex = vertex.c_str();
	const char *c_str_fragment = fragment.c_str();

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
				<< vertex
				<< std::endl;
	}

	mFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(mFragmentShader, 1, &c_str_fragment, nullptr);
	glCompileShader(mFragmentShader);

	glGetShaderiv(mFragmentShader, GL_COMPILE_STATUS, &success);

	if (!success)
	{
		glGetShaderInfoLog(mVertexShader, 512, nullptr, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
				<< infoLog << std::endl
				<< fragment
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

void GPUShader::initFromFilePaths(const std::string& vertex, const std::string& fragment)
{
	TRACE()

    std::ifstream vertex_ifs;
	vertex_ifs.open(vertex.c_str(), std::ifstream::in);
	std::string vertexShaderSource((std::istreambuf_iterator<char>(vertex_ifs)), (std::istreambuf_iterator<char>()));


    std::ifstream fragment_ifs;
	fragment_ifs.open(fragment.c_str(), std::ifstream::in);
	std::string fragmentShaderSource((std::istreambuf_iterator<char>(fragment_ifs)),
									(std::istreambuf_iterator<char>()));

    initFromFileContents(vertexShaderSource, fragmentShaderSource);
}