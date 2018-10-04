/*
 * shader.cpp
 *
 *  Created on: Oct 2, 2018
 *      Author: vrobot
 */


#include "shader.h"

// TODO: There are numerous ways to get GL
#define GL_GLEXT_PROTOTYPES 1
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_opengl_glext.h>

/* SHADER Text
 *
 * This shader is for rendering 2D text textures straight from a texture
 * Color and alpha blended.
 *
 */
const char* vs_Text = R"(
	uniform mat4 uMVPMatrix;
	attribute vec4 vPosition;
	attribute vec4 a_Color;
	attribute vec2 a_texCoord;
	varying vec4 v_Color;
	varying vec2 v_texCoord;
	void main() {
		gl_Position = uMVPMatrix * vPosition;
		v_texCoord = a_texCoord;
		v_Color = a_Color;
	})";

const char* fs_Text = R"(
	precision mediump float;
	varying vec4 v_Color;
	varying vec2 v_texCoord;
	uniform sampler2D s_texture;
	void main() {
		gl_FragColor = texture2D( s_texture, v_texCoord ) * v_Color;
		gl_FragColor.rgb *= v_Color.a;
	})";

namespace Fx {

Shader::Shader()
{

}

Shader::~Shader()
{
	reset();
}

int Shader::getAttrib(const char* name)
{
	auto it = attribs.find(name);
	if (it != attribs.end())
		return it->second;
	return 0;
}

void Shader::inspect()
{
	if (!program)
		return;

	GLint num_uniforms = 0;
	glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &num_uniforms);

	for (int i = 0; i < num_uniforms; i++)
	{
		GLchar uniform_name[256];
		GLsizei length = 0;
		GLint size = 0;
		UniformData data;
		glGetActiveUniform(program, i, sizeof(uniform_name),
				&length, &size, &data.type, uniform_name);
		// ... save this uniform data so it can be used during rendering
	}
}

std::string glTypeToString(GLenum type)
{
	#define CASE(TYPE) case TYPE: return #TYPE;
	switch(type)
	{
	CASE(GL_FLOAT);
	CASE(GL_FLOAT_VEC2);
	CASE(GL_FLOAT_VEC3);
	CASE(GL_FLOAT_VEC4);
	CASE(GL_DOUBLE);
	CASE(GL_BYTE);
	CASE(GL_UNSIGNED_BYTE);
	CASE(GL_2_BYTES);
	CASE(GL_3_BYTES);
	CASE(GL_4_BYTES);
	CASE(GL_SHORT);
	CASE(GL_UNSIGNED_SHORT);
	CASE(GL_INT);
	CASE(GL_UNSIGNED_INT);
	CASE(GL_INT_VEC2);
	CASE(GL_INT_VEC3);
	CASE(GL_INT_VEC4);
	CASE(GL_BOOL);
	CASE(GL_BOOL_VEC2);
	CASE(GL_BOOL_VEC3);
	CASE(GL_BOOL_VEC4);
	CASE(GL_FLOAT_MAT2);
	CASE(GL_FLOAT_MAT3);
	CASE(GL_FLOAT_MAT4);
	CASE(GL_SAMPLER_2D);
	CASE(GL_SAMPLER_CUBE);
	default:
		return "unknown";
	}
}

void Shader::activate()
{
	if (program)
		glUseProgram(program);
}

void Shader::reset()
{
	if (program)
	{
		glUseProgram(0);
		glDeleteProgram(program);

		program = 0;
	}

	// TODO: Delete vertex and fragment shaders
	srcVertex = "";
	srcFragment = "";
	error = "";
}

int Shader::init(
		const char* vertex_shader_source,
		const char* fragment_shader_source,
		const std::initializer_list<std::string>& uniforms,
		const std::initializer_list<std::string>& attributes)
{
	reset();

	GLsizei log_length = 0;
	GLchar message[1024];

	// Create and compile the vertex shader
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertex_shader_source, NULL); // vertex_shader_source is a GLchar* containing glsl shader source code
	glCompileShader(vertexShader);

	GLint status = 0;
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &status);
	if (status != GL_TRUE)
	{
		glGetShaderInfoLog(vertexShader, sizeof(message), &log_length, message);
		error = message;
		reset();
		return -1;
	}

	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragment_shader_source, NULL); // fragment_shader_source is a GLchar* containing glsl shader source code
	glCompileShader(fragmentShader);

	status = 0;
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &status);
	if (status != GL_TRUE)
	{
		glGetShaderInfoLog(fragmentShader, sizeof(message), &log_length, message);
		// Write the error to a log
		error = message;
		reset();
		return -1;
	}

	program = glCreateProgram();

	int val = glGetAttribLocation(program, "aTheta");
	// Uniforms
	int val2 = glGetUniformLocation(program, "uProjectionMatrix");

	/*
	// This step is unnecessary if you use the location specifier in your shader
	// e.g. layout (location = 0) in vec3 position;
	glBindAttribLocation(program, 0, "position"); // The index passed into glBindAttribLocation is
	glBindAttribLocation(program, 1, "texcoord"); // used by glEnableVertexAttribArray. "position"
	glBindAttribLocation(program, 2, "normal");   // "texcoord" "normal" and "color" are the names of the
	glBindAttribLocation(program, 3, "color");    // respective inputs in your fragment shader.
	*/

	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);
	glLinkProgram(program);

	status = 0;
	glGetProgramiv(program, GL_LINK_STATUS, &status);
	if (status != GL_TRUE)
	{
		glGetProgramInfoLog(program, sizeof(message), &log_length, message);
		// Write the error to a log
		error = message;
		return -1;
	}

	srcVertex = vertex_shader_source;
	srcFragment = fragment_shader_source;

	inspect();
	// Now we can detach and clean all the data
	glDetachShader(program, vertexShader);
	glDeleteShader(vertexShader);
	vertexShader = 0;

	glDetachShader(program, fragmentShader);
	glDeleteShader(fragmentShader);
	fragmentShader = 0;

	return 0;
}

const char* Shader::getError() const
{
	return error.c_str();
}

int Shader::getProgram() const
{
	return program;
}

}
