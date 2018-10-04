#include <exception>
#include <iostream>

#include "fx/fxmanager.h"
#include "fx/rendercontext.h"
#include "fx/texturemanager.h"
#include "fx/shader.h"

#include <SDL2/SDL.h>
#define GL_GLEXT_PROTOTYPES 1

#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_opengl_glext.h>

// Shader sources
const GLchar* vertexSource = R"(
attribute vec4 position;
void main()
{
	 gl_Position = vec4(position.xyz, 1.0);
})";

const GLchar* fragmentSource = R"(
precision mediump float;
void main()
{
  gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0 );
})";

int main(int argc, char** argv)
{
	Fx::FxManager manager;
	manager.init();

	// TODO: Test texture loading and disposal
	// TODO: Test batch drawing
	// TODO: Fix view uniform
	// TODO: Test fx objects

	Fx::TextureManager* tm = manager.getTextureManager();

	int screenWidth = 640;
	int screenHeight = 480;
	int flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;

	SDL_Window* wnd = SDL_CreateWindow("test", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
			screenWidth, screenHeight, flags);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
	SDL_GL_SetSwapInterval(0);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	auto glc = SDL_GL_CreateContext(wnd);
	auto rdr = SDL_CreateRenderer(wnd, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);

	Fx::Shader shader;
	if (shader.init(vertexSource, fragmentSource, {"position"}) != 0)
	{
		std::cerr << "Failed to initialize shader: " << shader.getError();
		return -1;
	}
	shader.activate();

	// TODO: No need for VBO right now.
	// Create Vertex Array Object
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Create a Vertex Buffer Object and copy the vertex data to it
	GLuint vbo;
	glGenBuffers(1, &vbo);

	GLfloat vertices[] = {0.0f, 0.5f, 0.5f, -0.5f, -0.5f, -0.5f};

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	// Specify the layout of the vertex data
	GLint posAttrib = shader.getAttrib("position");

	glEnableVertexAttribArray(posAttrib);
	// Set attribute feed from vbo
	glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);

	Fx::RenderContext rc(&manager);
	// Setting up scene view
	Fx::FxView2 view;
	view.scale = 1.0;
	// This is center position of the view
	view.pose = Fx::Pose(0,0,0);

	// This one does literally nothing
	rc.setView(view, screenWidth/2, screenHeight/2);

	bool done = false;

	while(!done)
	{
		SDL_Event e;
		while(SDL_PollEvent(&e))
		{
			if(e.type == SDL_QUIT)
			{
				done = true;
				break;
			}
		}

		// Clear the screen to black
		rc.clearFrame(Fx::makeRGB(0,0,0));

		Fx::VertexBatch batch(Fx::VertexBatch::PRIM_TRIPLES);
		batch += {
			Fx::Vertex::make2c(0.0f, 0.5f, Fx::makeRGB(255, 0, 255)),
			Fx::Vertex::make2c(0.5f,-0.5f, Fx::makeRGB(0, 255, 255)),
			Fx::Vertex::make2c(-0.5f,-0.5f, Fx::makeRGB(255, 255, 0)),
		};
		rc.renderBatch(batch);

		SDL_GL_SwapWindow(wnd);
		// NOTE: On Mac OS X make sure you bind 0 to the draw framebuffer
		// before swapping the window, otherwise nothing will happen.
	}

	return 0;
}
