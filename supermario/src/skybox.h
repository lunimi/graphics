// global constants
static const char* skybox_image_path = "../bin/images/at-the-river.jpg";
uint				NUM_TESS = 40;

std::vector<vertex> skybox_vertices;

// OpenGL objects
GLuint	skyVertex = 0;

std::vector<vertex> create_sphere_vertices(uint N)
{
	std::vector<vertex> v;
	for (uint h = 0; h <= N; h++) {
		float t1 = PI * h / float(N), ch = cos(t1), sh = sin(t1);
		for (uint w = 0; w <= N; w++) {
			float t2 = 2.f * PI * w / float(N), cw = cos(t2), sw = sin(t2);
			v.push_back({ vec3(sh * cw,sh * sw,ch),vec3(-sh * cw,-sh * sw,-ch) ,vec2(t2 / (PI * 2.0f),1 - t1 / PI) });
		}
	}

	return v;
}

void update_vertex_buffer_sky(const std::vector<vertex>& vertices)
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
	for (uint k = 0; k < NUM_TESS * (NUM_TESS + 1); k++)
	{
		indices.push_back(k);
		indices.push_back(k + NUM_TESS + 1);
		indices.push_back(k + NUM_TESS);

		indices.push_back(k);
		indices.push_back(k + 1);
		indices.push_back(k + NUM_TESS + 1);
	}

	// generation of vertex buffer: use vertices as it is
	glGenBuffers(1, &vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

	// geneation of index buffer
	glGenBuffers(1, &index_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * indices.size(), &indices[0], GL_STATIC_DRAW);

	if (skyVertex) glDeleteVertexArrays(1, &skyVertex);
	skyVertex = cg_create_vertex_array(vertex_buffer, index_buffer);
	if (!skyVertex) { printf("%s(): failed to create vertex aray\n", __func__); return; }
}