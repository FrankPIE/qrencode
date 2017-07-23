////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2017 BoXiang.Pei
//
// Permission is hereby granted, free of charge,
// to any person obtaining a copy of this software and associated documentation
// files (the "Software"), to deal in the Software without restriction,
// including without limitation the rights to use, copy, modify, merge,
// publish, distribute, sublicense, and/or sell copies of the Software,
// and to permit persons to whom the Software is furnished to do so,
// subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.	
//
//////////////////////////////////////////////////////////////////////////

#ifndef GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_NONE
#endif

#include <stdio.h>

#include <cassert>

#include <qrencode.h>

#include <glfw/glfw3.h>

#include <glbinding/gl/gl.h>
#include <glbinding/Binding.h>

#include <glm/glm.hpp>
#include <glm/ext.hpp>

#if defined(_WIN32) || defined(_WIN64)
#pragma warning(disable: 4251)
#endif

using namespace gl;
using namespace glbinding;

#if defined(DEBUG) || defined(_DEBUG)
#define CHECKE_SHADER_COMPILER_ERROR(shader)								 \
std::string shader_compiler_error = ShaderCompileErrorCheck(shader);		 \
if ( shader_compiler_error != "success" ) {  printf("%s", shader_compiler_error.c_str()); } 
#define CHECKE_SHADER_LINK_ERROR(program)								 \
std::string shader_link_error = ShaderLinkErrorCheck(program);			 \
if ( shader_link_error != "success" ) { printf("%s", shader_link_error.c_str()); } 
#else
#define CHECKE_SHADER_COMPILER_ERROR(shader)
#define CHECKE_SHADER_LINK_ERROR(program)
#endif

namespace {

gl::GLuint kVBOHandle = 0;
gl::GLuint kVIOHandle = 0;
gl::GLuint kVAOHandle = 0;

gl::GLuint kTextureObj = 0;

gl::GLuint kVertShader = 0;
gl::GLuint kFragShader = 0;

gl::GLuint kShaderProgram = 0;

gl::GLint  kProjectionMat = 0;
gl::GLint  kViewMat = 0;
gl::GLint  kWorldMat = 0;
gl::GLint  kSampler = 0;

void LoadShaderFile(const std::string& shader_file, std::vector<char>& buffer)
{
	if (!buffer.empty())
		buffer.clear();

	FILE* file_handle = fopen(shader_file.c_str(), "rb");

	if (file_handle != nullptr)
	{
		fseek(file_handle, 0L, SEEK_END);
		auto text_length = ftell(file_handle);
		fseek(file_handle, 0L, SEEK_SET);

		buffer.resize(text_length);

		auto actual_read = fread(&buffer.front(), 1, text_length, file_handle);

		assert(actual_read == text_length);

		buffer.push_back('\0');
	
		fclose(file_handle);
	}
}

std::string ShaderCompileErrorCheck(gl::GLuint shader)
{
	GLint compiled;
	glGetShaderiv(shader, GL_LINK_STATUS, &compiled);

	if (!compiled)
	{
		std::string log;

		GLsizei log_size;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_size);

		log.resize(log_size + 1);
		glGetShaderInfoLog(shader, log_size, &log_size, &log[0]);

		gl::glDeleteShader(shader);

		return log;
	}

	return "success";
}

std::string ShaderLinkErrorCheck(gl::GLuint program)
{
	GLint linked;
	glGetProgramiv(program, GL_LINK_STATUS, &linked);

	if (!linked)
	{
		GLsizei log_size;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_size);

		std::string log;
		log.resize(log_size + 1);

		glGetProgramInfoLog(program, log_size, &log_size, &log[0]);

		gl::glDeleteProgram(program);

		return log;
	}

	return "success";
}

void CompileShder(gl::GLuint shader, const std::vector<char>& buffer)
{
	assert(gl::glIsShader(shader) == GLboolean::GL_TRUE);

	const auto buf = buffer.data();

	gl::glShaderSource(shader, 1, &buf, nullptr);

	gl::glCompileShader(shader);

	CHECKE_SHADER_COMPILER_ERROR(shader);
}

void GenQREncode(std::vector<float>& buffer)
{
	auto size = 256;

	if (!buffer.empty())
		buffer.clear();

	buffer.resize(size * size * 4, 1.0f);

	auto code = QRcode_encodeString8bit("http://madstrawberry.me", 0, QR_ECLEVEL_H);

	if (code)
	{
		auto mulriple = static_cast<float>(size - 16) / static_cast<float>(code->width);

		for (auto y = 0; y < code->width; ++y)
		{
			for (auto x = 0; x < code->width; ++x)
			{
				auto b = code->data[y * code->width + x];

				if (b & 0x01)
				{
					auto left = std::max(0, std::min(static_cast<int>(x * mulriple + 8), 256));
					auto top  = std::max(0, std::min(static_cast<int>(y * mulriple + 8), 256));

					auto right  = std::max(0, std::min(static_cast<int>((x + 1) * mulriple + 8), 256));
					auto bottom = std::max(0, std::min(static_cast<int>((y + 1) * mulriple + 8), 256));

					for (auto dy = top; dy < bottom; ++dy)
					{
						for (auto dx = left; dx < right; ++dx)
						{
							auto index = static_cast<size_t>((dy * 256 + dx) * 4);

							buffer[index] = 0.f;
							buffer[index + 1] = 0.f;
							buffer[index + 2] = 0.f;
							buffer[index + 3] = 1.f;
						}
					}
				}
			}
		}
		
		QRcode_free(code);
	}
}

void SceneInit()
{
	//Compile Shader
	std::vector<char> buffer;

	kVertShader = gl::glCreateShader(gl::GL_VERTEX_SHADER);
	LoadShaderFile("..\\..\\..\\media\\QREncode.vert", buffer);
	CompileShder(kVertShader, buffer);

	kFragShader = gl::glCreateShader(gl::GL_FRAGMENT_SHADER);
	LoadShaderFile("..\\..\\..\\media\\QREncode.frag", buffer);
	CompileShder(kFragShader, buffer);

	buffer.clear();

	kShaderProgram = gl::glCreateProgram();
	gl::glAttachShader(kShaderProgram, kVertShader);
	gl::glAttachShader(kShaderProgram, kFragShader);

	gl::glLinkProgram(kShaderProgram);

	CHECKE_SHADER_LINK_ERROR(kShaderProgram);

	gl::glDetachShader(kShaderProgram, kVertShader);
	gl::glDetachShader(kShaderProgram, kFragShader);

	gl::glDeleteShader(kVertShader);
	gl::glDeleteShader(kFragShader);

	gl::glUseProgram(kShaderProgram);

	kProjectionMat = glGetUniformLocation(kShaderProgram, "projection");
	kViewMat = glGetUniformLocation(kShaderProgram, "view");
	kWorldMat = glGetUniformLocation(kShaderProgram, "world");
	kSampler = glGetUniformLocation(kShaderProgram, "Sampler");

	glUniformMatrix4fv(kProjectionMat, 1, GL_FALSE, glm::value_ptr(glm::perspective(90.0f, 4.0f / 3.0f, 1.f, 1024.f)));
	glUniformMatrix4fv(kViewMat, 1, GL_FALSE, glm::value_ptr(glm::lookAt(glm::vec3(0.0f, 1.0f, -15.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f))));
	glUniformMatrix4fv(kWorldMat, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.f)));
	 
	const GLfloat vb[] = {
	   //position
	   -10.0f, -10.0f, 0.0f,	
	   -10.0f,  10.0f, 0.0f,	
		10.0f,  10.0f, 0.0f,	
	    10.0f, -10.0f, 0.0f,

	   //uv				  
		1.0f, 1.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f, 0.0f
	};

	gl::glGenBuffers(1, &kVBOHandle);

	gl::glBindBuffer(gl::GL_ARRAY_BUFFER, kVBOHandle);

	gl::glBufferData(gl::GL_ARRAY_BUFFER, sizeof(vb), vb, gl::GL_STATIC_DRAW);

	const GLuint ib[] = {
		0, 1, 2,
		2, 0, 3
	};

	gl::glGenBuffers(1, &kVIOHandle);
	gl::glBindBuffer(gl::GL_ELEMENT_ARRAY_BUFFER, kVIOHandle);
	gl::glBufferData(gl::GL_ELEMENT_ARRAY_BUFFER, sizeof(ib), ib, gl::GL_STATIC_DRAW);

	gl::glGenVertexArrays(1, &kVAOHandle);
	gl::glBindVertexArray(kVAOHandle);

	gl::glVertexAttribPointer(0, 3, gl::GL_FLOAT, gl::GL_FALSE, 0, reinterpret_cast<gl::GLvoid*>(0));
	gl::glVertexAttribPointer(1, 2, gl::GL_FLOAT, gl::GL_FALSE, 0, reinterpret_cast<gl::GLvoid*>(12 * sizeof(float)));
	gl::glEnableVertexAttribArray(0);
	gl::glEnableVertexAttribArray(1);

	std::vector<float> image;

	gl::glGenTextures(1, &kTextureObj);
	gl::glBindTexture(gl::GL_TEXTURE_2D, kTextureObj);

	GenQREncode(image);

	glTexParameteri(gl::GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GLint(GLenum::GL_LINEAR));
	glTexParameteri(gl::GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GLint(GLenum::GL_LINEAR));

	glTextureStorage2D(kTextureObj, 5, gl::GL_RGBA32F, 256, 256);

	glTexSubImage2D(gl::GL_TEXTURE_2D, 0, 0, 0, 256, 256, GL_RGBA, GLenum::GL_FLOAT, image.data());

	gl::glClearColor(0.235f, 0.235f, 0.235f, 1.0f);
}

void SceneRender()
{
	gl::glClear(gl::GL_COLOR_BUFFER_BIT);

	gl::glUseProgram(kShaderProgram);
	gl::glBindVertexArray(kVAOHandle);

	gl::glBindBuffer(gl::GL_ELEMENT_ARRAY_BUFFER, kVIOHandle);

	gl::glUniform1i(kSampler, 0);

	gl::glActiveTexture(GLenum::GL_TEXTURE0);
	gl::glBindTexture(GLenum::GL_TEXTURE_2D, kTextureObj);

	gl::glDrawElements(gl::GL_TRIANGLES, 6, gl::GL_UNSIGNED_INT, nullptr);
}

void SceneClear()
{
	gl::glDeleteProgram(kShaderProgram);

	gl::glDeleteVertexArrays(1, &kVAOHandle);
	gl::glDeleteBuffers(1, &kVBOHandle);
	gl::glDeleteBuffers(1, &kVIOHandle);

	gl::glDeleteTextures(1, &kTextureObj);
}

void KeyDown(GLFWwindow* window, int key, int scan_code, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, static_cast<int>(gl::GL_TRUE));
	}
}

void ReSizeFrameBuffer(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);

	auto ratio = 1.0f;

	if (height > 0)
	{
		ratio = static_cast<float>(width) / static_cast<float>(height);
	}

	glUniformMatrix4fv(kProjectionMat, 1, GL_FALSE, glm::value_ptr(glm::perspective(90.0f, ratio, 1.0f, 1024.0f)));
}

void ErrorCallback(int error, const char* description)
{
	printf("Error : %d, Desc : %s", error, description);
}
}

int main(int argc, char* argv[])
{
	if (glfwInit() == 0)
		return -1;

	glfwDefaultWindowHints();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, static_cast<int>(GL_TRUE));
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	glfwSetErrorCallback(ErrorCallback);

	auto window = glfwCreateWindow(800, 600, "RenderWithOpenGL", nullptr, nullptr);

	if (window == nullptr)
	{
		glfwTerminate();
		return -1;
	}

	glfwSetKeyCallback(window, KeyDown);
	glfwSetFramebufferSizeCallback(window, ReSizeFrameBuffer);

	glfwMakeContextCurrent(window);
	glfwSwapInterval(0);			//vertical synchronization off

	Binding::initialize();

	SceneInit();

	while (!glfwWindowShouldClose(window))
	{
		SceneRender();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	SceneClear();

	glfwDestroyWindow(window);

	glfwTerminate();

	return 0;
}