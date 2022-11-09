#ifndef n20
#define n20

//#include <iostream>

#include <GL/glew.h>
#include <glm/glm.hpp>

class Icosaedre
{
public:

	Icosaedre();
	Icosaedre(float lenght, glm::vec3 origin, int sub_div);

	void set_param(float lenght, glm::vec3 origin, int sub_div);

	void sub_divide(int nb_subdivision = 1);
	size_t vertex_array_size();

	void calc();

	GLfloat* get_vertex_array();


private:
	void init();

	float* m_g_vertex_buffer_data = nullptr;
	float m_lenght;
	glm::vec3 m_origin;
	int m_sub_div;
};
GLuint LoadShaders(const char * vertex_file_path, const char * fragment_file_path);
GLuint loadDDS(const char * imagepath);
GLuint loadBMP_custom(const char * imagepath);
#endif 