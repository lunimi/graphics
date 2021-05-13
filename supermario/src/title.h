// global constants
static const char* title_image_path = "../bin/images/title_test.png";

std::vector<vertex> title_vertices;

// OpenGL objects
GLuint	titleVertex = 0;
uint	title_onoff = 1;

std::vector<vertex> create_title_vertices()
{
	std::vector<vertex> v;

	v.push_back({ vec3(0,0,0),vec3(0,0,1),vec2(0,0) });
	v.push_back({ vec3(16,0,0),vec3(0,0,1),vec2(1,0) });
	v.push_back({ vec3(16,9,0),vec3(0,0,1),vec2(1,1) });
	v.push_back({ vec3(0,9,0),vec3(0,0,1),vec2(0,1) });

	return v;
}

void update_vertex_buffer_title(const std::vector<vertex>& vertices)
{
	static GLuint vertex_buffer = 0;	// ID holder for vertex buffer
	static GLuint index_buffer = 0;		// ID holder for index buffer

	// clear and create new buffers
	if (vertex_buffer)	glDeleteBuffers(1, &vertex_buffer);	vertex_buffer = 0;
	if (index_buffer)	glDeleteBuffers(1, &index_buffer);	index_buffer = 0;

	// check exceptions
	if (vertices.empty()) { printf("[error] vertices is empty.\n"); return; }

	// create buffers
	std::vector<uint> indices;

	indices.push_back(0);
	indices.push_back(1);
	indices.push_back(2);

	indices.push_back(2);
	indices.push_back(3);
	indices.push_back(0);

	// generation of vertex buffer: use vertices as it is
	glGenBuffers(1, &vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

	// geneation of index buffer
	glGenBuffers(1, &index_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * indices.size(), &indices[0], GL_STATIC_DRAW);

	if (titleVertex) glDeleteVertexArrays(1, &titleVertex);
	titleVertex = cg_create_vertex_array(vertex_buffer, index_buffer);
	if (!titleVertex) { printf("%s(): failed to create vertex aray\n", __func__); return; }
}