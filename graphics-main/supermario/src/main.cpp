#include "cgmath.h"		// slee's simple math library
#include "cgut.h"		// slee's OpenGL utility
#include "trackball.h"	// virtual trackball
#include "map.h"
#include "character.h"
#include "light.h"

//*************************************
// global constants
static const char*	window_name = "cgbase - trackball";
static const char*	vert_shader_path = "../bin/shaders/trackball.vert";
static const char*	frag_shader_path = "../bin/shaders/trackball.frag";

//*************************************
// common structures
struct camera
{
	vec3	eye = vec3( 5, 5, 20 );
	vec3	at = vec3( 5, 5, 0 );
	vec3	up = vec3( 0, 1, 0 );
	mat4	view_matrix = mat4::look_at( eye, at, up );

	float	fovy = PI/6.0f; // must be in radian
	float	aspect;
	float	dnear = 1.0f;
	float	dfar = 1000.0f;
	mat4	projection_matrix;
};

struct material_t
{
	vec4	ambient = vec4(0.2f, 0.2f, 0.2f, 1.0f);
	vec4	diffuse = vec4(0.8f, 0.8f, 0.8f, 1.0f);
	vec4	specular = vec4(0.6f, 0.6f, 0.3f, 1.0f);
	float	shininess = 1000.0f;
};

//*************************************
// window objects
GLFWwindow*	window = nullptr;
ivec2		window_size = cg_default_window_size(); // initial window size

//*************************************
// OpenGL objects
GLuint	program	= 0;	// ID holder for GPU program
GLuint	vertex_array = 0;	// ID holder for vertex array object

//*************************************
// global variables
int		frame = 0;				// index of rendering frames

// holder of vertices and indices of a unit circle
std::vector<vertex>	unit_block_vertices;	// host-side vertices
Map			map;
Character	crt(&map,vec2(2,2));
bool	b_key_right;
bool	b_key_left;
//*************************************
// scene objects
camera		cam;
trackball	tb;
light_t		sunlight;
material_t	material;



//*************************************
void update(float t)
{
	// update projection matrix
	cam.aspect = window_size.x / float(window_size.y);
	cam.projection_matrix = mat4::perspective(cam.fovy, cam.aspect, cam.dnear, cam.dfar);

	if (b_key_right) {
		crt.move_right();
	}
	else if (b_key_left) {
		crt.move_left();
	}

	cam.eye = vec3(crt.position + vec2(3, 2), 20);
	cam.at = vec3(crt.position + vec2(3, 2), 0);
	cam.view_matrix = mat4::look_at(cam.eye, cam.at, cam.up);
	crt.update(float(t), b_key_left || b_key_right);

	// update uniform variables in vertex/fragment shaders
	GLint uloc;
	uloc = glGetUniformLocation( program, "view_matrix" );			if(uloc>-1) glUniformMatrix4fv( uloc, 1, GL_TRUE, cam.view_matrix );
	uloc = glGetUniformLocation( program, "projection_matrix" );	if(uloc>-1) glUniformMatrix4fv( uloc, 1, GL_TRUE, cam.projection_matrix );
}

void render()
{
	float theta = 0.0005f * frame;	//sunlight rotate
	sunlight.position.x = 25.0f * (float)sin(theta);
	sunlight.position.y = 25.0f * (float)abs(cos(theta));
	// setup sunlight properties
	glUniform4fv(glGetUniformLocation(program, "light_position"), 1, sunlight.position);
	glUniform4fv(glGetUniformLocation(program, "Ia"), 1, sunlight.ambient);
	glUniform4fv(glGetUniformLocation(program, "Id"), 1, sunlight.diffuse);
	glUniform4fv(glGetUniformLocation(program, "Is"), 1, sunlight.specular);

	// setup little lights properties
	
	// clear screen (with background color) and clear depth buffer
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	// notify GL that we use our own program
	glUseProgram( program );

	// build the model matrix for map
	for (int i = 0; i < MAP_WIDTH; i++) {
		for (int j = 0; j < MAP_HEIGHT; j++) {
			if (map.map[i][j] == 1) {
				mat4 model_matrix = mat4::translate(float(i), float(j), 0);

				GLint uloc;
				uloc = glGetUniformLocation(program, "model_matrix");			if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, model_matrix);

				// per-circle draw calls
				glDrawElements(GL_TRIANGLES, 36 , GL_UNSIGNED_INT, nullptr);
			}
		}
	}

	// build character model
	mat4 model_matrix = mat4::translate(crt.position.x, crt.position.y, 0);
	GLint uloc;
	uloc = glGetUniformLocation(program, "model_matrix");			if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, model_matrix);
	// per-circle draw calls
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);

	glUniform1i(glGetUniformLocation(program, "NUM_LIGHT"), NUM_LIGHT);
	int index = 0;
	char string[20] = "light_position2[0]";
	char string2[20] = "Ia2[0]";
	char string3[20] = "Id2[0]";
	char string4[20] = "Is2[0]";
	for (auto& l : lights) {

		glUniform4fv(glGetUniformLocation(program, string), 1, l.position);
		glUniform4fv(glGetUniformLocation(program, string2), 1, l.ambient);
		glUniform4fv(glGetUniformLocation(program, string3), 1, l.diffuse);
		glUniform4fv(glGetUniformLocation(program, string4), 1, l.specular);
		string[16] ++;
		string2[4]++;
		string3[4]++;
		string4[4]++;
	}

	// setup material properties
	glUniform4fv(glGetUniformLocation(program, "Ka"), 1, material.ambient);
	glUniform4fv(glGetUniformLocation(program, "Kd"), 1, material.diffuse);
	glUniform4fv(glGetUniformLocation(program, "Ks"), 1, material.specular);
	glUniform1f(glGetUniformLocation(program, "shininess"), material.shininess);

	// swap front and back buffers, and display to screen
	glfwSwapBuffers( window );
}

void reshape( GLFWwindow* window, int width, int height )
{
	// set current viewport in pixels (win_x, win_y, win_width, win_height)
	// viewport: the window area that are affected by rendering 
	window_size = ivec2(width,height);
	glViewport( 0, 0, width, height );
}

void print_help()
{
	printf( "[help]\n" );
	printf( "- press ESC or 'q' to terminate the program\n" );
	printf( "- press F1 or 'h' to see help\n" );
	printf( "- press Home to reset camera\n" );
	printf( "\n" );
}

void keyboard( GLFWwindow* window, int key, int scancode, int action, int mods )
{
	if(action==GLFW_PRESS)
	{
		if (key == GLFW_KEY_ESCAPE || key == GLFW_KEY_Q)	glfwSetWindowShouldClose(window, GL_TRUE);
		else if (key == GLFW_KEY_H || key == GLFW_KEY_F1)	print_help();
		else if (key == GLFW_KEY_HOME)					cam = camera();

		else if (key == GLFW_KEY_A) b_key_left = true;
		else if (key == GLFW_KEY_D) b_key_right = true;
		else if (key == GLFW_KEY_W || key == GLFW_KEY_SPACE) crt.jump();
	}
	if (action == GLFW_RELEASE) {
		if (key == GLFW_KEY_A) b_key_left = false;
		else if (key == GLFW_KEY_D) b_key_right = false;
	}
}

void mouse( GLFWwindow* window, int button, int action, int mods )
{
	if(button==GLFW_MOUSE_BUTTON_LEFT)
	{
		dvec2 pos; glfwGetCursorPos(window,&pos.x,&pos.y);
		vec2 npos = cursor_to_ndc( pos, window_size );
		if(action==GLFW_PRESS)			tb.begin( cam.view_matrix, npos );
		else if(action==GLFW_RELEASE)	tb.end();
	}
}

void motion( GLFWwindow* window, double x, double y )
{
	if(!tb.is_tracking()) return;
	vec2 npos = cursor_to_ndc( dvec2(x,y), window_size );
	cam.view_matrix = tb.update( npos );
}

void update_vertex_buffer(const std::vector<vertex>& vertices)
{
	static GLuint vertex_buffer = 0;	// ID holder for vertex buffer
	static GLuint index_buffer = 0;		// ID holder for index buffer

	if (vertex_buffer)	glDeleteBuffers(1, &vertex_buffer);	vertex_buffer = 0;
	if (index_buffer)	glDeleteBuffers(1, &index_buffer);	index_buffer = 0;

	// check exceptions
	if (vertices.empty()) { printf("[error] vertices is empty.\n"); return; }

	// create buffers
	std::vector<uint> indices = 
	{
		0,1,2,1,3,2,
		6,7,5,6,5,4,
		2,6,0,6,4,0,
		3,5,7,3,1,5,
		7,6,3,3,6,2,
		1,0,5,0,4,5
	};

	
	// generation of vertex buffer: use vertices as it is
	glGenBuffers(1, &vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

	// generation of index buffer
	glGenBuffers(1, &index_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * indices.size(), &indices[0], GL_STATIC_DRAW);

	// generate vertex array object, which is mandatory for OpenGL 3.3 and higher
	if (vertex_array) glDeleteVertexArrays(1, &vertex_array);
	vertex_array = cg_create_vertex_array(vertex_buffer, index_buffer);
	if (!vertex_array) { printf("%s(): failed to create vertex aray\n", __func__); return; }
}

bool user_init()
{
	// log hotkeys
	print_help();

	// init GL states
	glClearColor( 39/255.0f, 40/255.0f, 34/255.0f, 1.0f );	// set clear color
	glEnable( GL_CULL_FACE );								// turn on backface culling
	glEnable( GL_DEPTH_TEST );								// turn on depth tests

	map.create();

	// define the position of four corner vertices
	unit_block_vertices = std::move(create_block_vertices());

	// create vertex buffer
	update_vertex_buffer(unit_block_vertices);

	glBindVertexArray(vertex_array);


	return true;
}

void user_finalize()
{
}

int main( int argc, char* argv[] )
{
	// create window and initialize OpenGL extensions
	if(!(window = cg_create_window( window_name, window_size.x, window_size.y ))){ glfwTerminate(); return 1; }
	if(!cg_init_extensions( window )){ glfwTerminate(); return 1; }	// version and extensions

	// initializations and validations
	if(!(program=cg_create_program( vert_shader_path, frag_shader_path ))){ glfwTerminate(); return 1; }	// create and compile shaders/program
	if(!user_init()){ printf( "Failed to user_init()\n" ); glfwTerminate(); return 1; }					// user initialization

	// register event callbacks
	glfwSetWindowSizeCallback( window, reshape );	// callback for window resizing events
    glfwSetKeyCallback( window, keyboard );			// callback for keyboard events
	glfwSetMouseButtonCallback( window, mouse );	// callback for mouse click inputs
	glfwSetCursorPosCallback( window, motion );		// callback for mouse movement

	float tp, t = 0;
	// enters rendering/event loop
	for( frame=0; !glfwWindowShouldClose(window); frame++ )
	{
		tp = t;
		t = float(glfwGetTime());
		glfwPollEvents();	// polling and processing of events
		update(t-tp);			// per-frame update
		render();			// per-frame render
	}

	// normal termination
	user_finalize();
	cg_destroy_window(window);

	return 0;
}
