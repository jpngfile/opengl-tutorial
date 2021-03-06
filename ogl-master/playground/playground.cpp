// Include standard headers
#include <stdio.h>
#include <stdlib.h>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;
using namespace std;

#include <vector>
#include <iostream>

#include <common/shader.hpp>

size_t numSphereVertices(int resolution) {
    int n = resolution * 4;
    int numLayers = ((n-2)/2) + 2;
    //size_t numVertices = n * numLayers;
    size_t numTriangles = (n * (numLayers-2) * 2) + 2;
    return numTriangles * 3;
}

void getTrianglesFromVertices(vector<vector<vec4> > &levels, vec3 vertices[]) {
    int numLevels = levels.size();
    int n = levels[0].size();
    int index = 0;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < numLevels - 1; j++) {
            // Add top triangle
            if (j > 0) {
                vertices[index] = vec3(levels[j][i]);
                vertices[index + 1] = vec3(levels[j][(i + 1) % n]);
                vertices[index + 2] = vec3(levels[j + 1][(i + 1) % n]);
                index += 3;
            }
            // Add bottom triangle
            if (j < numLevels - 2) {
                vertices[index] = vec3(levels[j][i]);
                vertices[index + 1] = vec3(levels[j + 1][i]);
                vertices[index + 2] = vec3(levels[j + 1][(i + 1) % n]);
                index += 3;
            }
        }
    }
}

vec3* getSphereVertices(int resolution, float radius) {
    int n = resolution * 4;
    int numLayers = ((n-2)/2) + 2;
    size_t numVertices = n * numLayers;
    size_t numTriangles = (n * (numLayers-2) * 2) + 2;

    float yAngle = glm::radians(360 / ((float)n));
    float interiorAngle = ((float)(n - 2) * 180 / (float)n);
    float xAngle = glm::radians(180 - interiorAngle);
    float edgeLength = 2 * radius * glm::sin(M_PI/n);

    vector<vector<vec4> > levels;
    vec4 basePoint = vec4(0.0f, radius, 0.0f, 1.0f);
    vec4 edge = vec4(0.0f, 0.0f, edgeLength, 0.0f);
    cout <<  "edge: (" << edge.x << ", " << edge.y << ", " << edge.z << ")" << endl;
    vector<vec4> topLevel(n, basePoint);
    levels.push_back(topLevel);

    //size_t numVertices = n * numLayers;
    vec3* vertices = new vec3[numTriangles * 3];

    // Initialize beginning
    //vertices[0] = basePoint.x;
    //vertices[1] = basePoint.y;
    //vertices[2] = basePoint.z;
    //vertices[0] = vec3(basePoint.x, basePoint.y, basePoint.z);

    cout << "(" << vertices[0].x << ", " << vertices[0].y << ", " << vertices[0].z << ")" << endl;

    //int index = 1;
    mat4 xrotationMat = glm::rotate(mat4(), xAngle, vec3(1.0f, 0.0f, 0.0f));
    mat4 yrotationMat = glm::rotate(mat4(), yAngle, vec3(0.0f, 1.0f, 0.0f));
    //cout << "yangle: " << yAngle << endl;
    cout << "xangle: " << xAngle << endl;

    // Move initial edge a bit
    mat4 initialxRotation = glm::rotate(mat4(), -(xAngle / 2), vec3(1.0f, 0.0f, 0.0f));
    edge = initialxRotation * edge;

    cout <<  "edge: (" << edge.x << ", " << edge.y << ", " << edge.z << ")" << endl;
    cout << "numLayers: " << numLayers << endl;

    for (float i = 0; i < numLayers - 1; i++) {
        vector<vec4> curLevel = levels[i];
        edge = xrotationMat * edge;
        cout <<  "edge: (" << edge.x << ", " << edge.y << ", " << edge.z << ")" << endl;
        vector<vec4> newLevel;
        for (float j = 0; j < n; j++) {
            vec4 point = curLevel[j];
            cout << "point: (" << point.x << ", " << point.y << ", " << point.z << ")" << endl;
            //cout <<  "edge: (" << edge.x << ", " << edge.y << ", " << edge.z << ")" << endl;
            mat4 translateMat = glm::translate(mat4(), vec3(edge.x, edge.y, edge.z));
            vec4 newPoint = translateMat * point;
            newLevel.push_back(newPoint);
            //vertices[index] = newPoint.x;
            //vertices[index + 1] = newPoint.y;
            //vertices[index + 2] = newPoint.z;
            //vertices[index] = vec3(newPoint.x, newPoint.y, newPoint.z);
            //index += 1;
            //cout << "(" << vertices[index] << ", " << vertices[index + 1] << ", " << vertices[index + 2] << ")" << endl;
            //index += 3;
            edge = yrotationMat * edge;
        }
        levels.push_back(newLevel);
    }

    // Create the triangles
    getTrianglesFromVertices(levels, vertices);
    return vertices;
}

int main( void )
{
	// Initialise GLFW
	if( !glfwInit() )
	{
		fprintf( stderr, "Failed to initialize GLFW\n" );
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow( 1024, 768, "Tutorial 04 - Colored Cube", NULL, NULL);
	if( window == NULL ){
		fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Dark blue background
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS); 

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// Create and compile our GLSL program from the shaders
	GLuint programID = LoadShaders( "TransformVertexShader.vertexshader", "ColorFragmentShader.fragmentshader" );

	// Get a handle for our "MVP" uniform
	GLuint MatrixID = glGetUniformLocation(programID, "MVP");

	// Projection matrix : 45� Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	glm::mat4 Projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);
	// Camera matrix
	glm::mat4 View       = glm::lookAt(
								glm::vec3(4,3,-3), // Camera is at (4,3,-3), in World Space
								glm::vec3(0,0,0), // and looks at the origin
								glm::vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
						   );
	// Model matrix : an identity matrix (model will be at the origin)
	glm::mat4 Model      = glm::mat4(1.0f);
	// Our ModelViewProjection : multiplication of our 3 matrices
	glm::mat4 MVP        = Projection * View * Model; // Remember, matrix multiplication is the other way around

    size_t size = numSphereVertices(5);
    vec3* g_vertex_buffer_data = getSphereVertices(5, 1);
//	// One color for each vertex. They were generated randomly.
//	static const GLfloat g_color_buffer_data[] = { 
//		0.583f,  0.771f,  0.014f,
//		0.609f,  0.115f,  0.436f,
//		0.327f,  0.483f,  0.844f,
//		0.822f,  0.569f,  0.201f,
//		0.435f,  0.602f,  0.223f,
//		0.310f,  0.747f,  0.185f,
//		0.597f,  0.770f,  0.761f,
//		0.559f,  0.436f,  0.730f,
//		0.359f,  0.583f,  0.152f,
//		0.483f,  0.596f,  0.789f,
//		0.559f,  0.861f,  0.639f,
//		0.195f,  0.548f,  0.859f,
//		0.014f,  0.184f,  0.576f,
//		0.771f,  0.328f,  0.970f,
//		0.406f,  0.615f,  0.116f,
//		0.676f,  0.977f,  0.133f,
//		0.971f,  0.572f,  0.833f,
//		0.140f,  0.616f,  0.489f,
//		0.997f,  0.513f,  0.064f,
//		0.945f,  0.719f,  0.592f,
//		0.543f,  0.021f,  0.978f,
//		0.279f,  0.317f,  0.505f,
//		0.167f,  0.620f,  0.077f,
//		0.347f,  0.857f,  0.137f,
//		0.055f,  0.953f,  0.042f,
//		0.714f,  0.505f,  0.345f,
//		0.783f,  0.290f,  0.734f,
//		0.722f,  0.645f,  0.174f,
//		0.302f,  0.455f,  0.848f,
//		0.225f,  0.587f,  0.040f,
//		0.517f,  0.713f,  0.338f,
//		0.053f,  0.959f,  0.120f,
//		0.393f,  0.621f,  0.362f,
//		0.673f,  0.211f,  0.457f,
//		0.820f,  0.883f,  0.371f,
//		0.982f,  0.099f,  0.879f
//	};

	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
//	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);
	glBufferData(GL_ARRAY_BUFFER, size*sizeof(vec3), g_vertex_buffer_data, GL_STATIC_DRAW);
//
//	GLuint colorbuffer;
//	glGenBuffers(1, &colorbuffer);
//	glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
//	glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data), g_color_buffer_data, GL_STATIC_DRAW);

	do{

		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Use our shader
		glUseProgram(programID);

		// Send our transformation to the currently bound shader, 
		// in the "MVP" uniform
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(
			0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

//		// 2nd attribute buffer : colors
//		glEnableVertexAttribArray(1);
//		glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
//		glVertexAttribPointer(
//			1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
//			3,                                // size
//			GL_FLOAT,                         // type
//			GL_FALSE,                         // normalized?
//			0,                                // stride
//			(void*)0                          // array buffer offset
//		);

		// Draw the triangle !
        glPointSize(5);
		glDrawArrays(GL_LINES, 0, size); // 12*3 indices starting at 0 -> 12 triangles

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		   glfwWindowShouldClose(window) == 0 );

	// Cleanup VBO and shader
	glDeleteBuffers(1, &vertexbuffer);
//	glDeleteBuffers(1, &colorbuffer);
	glDeleteProgram(programID);
	glDeleteVertexArrays(1, &VertexArrayID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}

