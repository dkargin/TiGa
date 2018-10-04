/*
 * shader.h
 *
 *  Created on: Oct 2, 2018
 *      Author: vrobot
 */

#ifndef SIMENGINE_FX_SHADER_H_
#define SIMENGINE_FX_SHADER_H_

#include <map>
#include <vector>
#include <string>

namespace Fx {

// Wrapper around GLSL ES 2.0 shader
class Shader
{
public:
	Shader();
	~Shader();

	// Creates vertex program
	int init(const char* vertex, const char* fragment,
			const std::initializer_list<std::string>& uniforms = {},
			const std::initializer_list<std::string>& attributes = {});
	void reset();

	// Enables the shader
	void activate();
	void inspect();

	int getAttrib(const char* name);

	const char* getError() const;

	struct UniformData
	{
		std::string name;
		unsigned int type;
		int id;
	};

	// Get ID of shader program
	int getProgram() const;

protected:
	// Map of uniform parameters
	std::map<std::string, int> uniforms;
	// Map of vertex attributes
	std::map<std::string, int> attribs;
	// Map of attached samplers
	std::map<std::string, int> samplers;

	std::vector<UniformData> allUniforms;
	unsigned int program = 0;
	unsigned int vertexShader = 0;
	unsigned int fragmentShader = 0;
	// Source code of vertex program
	std::string srcVertex;
	// Source code of the fragment program
	std::string srcFragment;

	// Error description
	mutable std::string error;
};

} // namespace Fx

#endif /* SIMENGINE_FX_SHADER_H_ */
