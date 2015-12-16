//
//  shaderMain
//
//  Main program for lighting/shading assignment
//
//  This code can be compiled as either C or C++.
//
//  Students should not be modifying this file.
//

#ifdef __cplusplus
#include <cstdlib>
#include <iostream>
#else
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#endif

#ifdef __APPLE__
#include <GLUT/GLUT.h>
#include <OpenGL/gl.h>
#else
#ifdef WIN32
#include <windows.h>
#endif
#include <GL/glew.h>
#include <GL/glut.h>
#include <GL/gl.h>
#endif

#include "shaderSetup.h"
#include "simpleShape.h"
#include "shapes.h"
#include "viewParams.h"
#include "lightingParams.h"

#ifdef __cplusplus
using namespace std;
#endif

// How to calculate an offset into the vertex buffer
#define BUFFER_OFFSET(i) ((char *)NULL + (i))

// We need four vertex buffers and four element buffers:
// two for the torus (flat shading and non-flat shading) and
// two for the teapot (flat shading and non-flat shading).
//
// Array layout:
//         column 0      column 1
// row 0:  torus flat    torus non-flat
// row 1:  teapot flat   teapot non-flat
//
GLuint vbuffer[2][2];
GLuint ebuffer[2][2];
int    numVerts[2][2];

// Animation flag
bool animating = false;

// Initial animation rotation angles
float angles[2] = { 0.0f, 0.0f };

// Current shader type:  flat vs. non-flat
int currentShader = SHADE_FLAT;

// program IDs...for program and shaders
GLuint program, flat, phong, gouraud;

//
// createShape() - create vertex and element buffers for a shape
//
// @param obj - which shape to create
// @param flat   - flat vs. non-flat shading
//
void createShape(int obj, int flat)
{
	// clear any previous shape
	clearShape();

	// make the shape
	makeShape(obj, flat);

	// save the vertex count
	numVerts[obj][flat] = nVertices();

	// get the points for the shape
	float *points = getVertices();
	int dataSize = nVertices() * 4 * sizeof(float);

	// get the normals for the shape
	float *normals = getNormals();
	int ndataSize = nVertices() * 3 * sizeof(float);

	// get the element data for the shape
	GLushort *elements = getElements();
	int edataSize = nVertices() * sizeof(GLushort);

	// generate the vertex buffer
	glGenBuffers(1, &vbuffer[obj][flat]);
	glBindBuffer(GL_ARRAY_BUFFER, vbuffer[obj][flat]);
	glBufferData(GL_ARRAY_BUFFER, dataSize + ndataSize, 0, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, dataSize, points);
	glBufferSubData(GL_ARRAY_BUFFER, dataSize, ndataSize, normals);

	// generate the element buffer
	glGenBuffers(1, &ebuffer[obj][flat]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebuffer[obj][flat]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, edataSize,
		elements, GL_STATIC_DRAW);
}

//
// selectBuffers() - bind the correct vertex and element buffers
//
// @param obj - which object to select
// @param flat   - flat vs. non-flat shading
//
// Assumes the correct shader program has already been enabled
//
void selectBuffers(GLuint program, int obj, int flat) {

	// bind the buffers
	glBindBuffer(GL_ARRAY_BUFFER, vbuffer[obj][flat]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebuffer[obj][flat]);

	// calculate the number of bytes of vertex data
	int dataSize = numVerts[obj][flat] * 4 * sizeof(float);

	// set up the vertex attribute variables
	GLuint vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(0));

	GLuint vNormal = glGetAttribLocation(program, "vNormal");
	glEnableVertexAttribArray(vNormal);
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(dataSize));
}


// Verify shader program creation
static void checkShaderError(GLuint program, const char *which)
{
	if (!program) {
#ifdef __cplusplus
		cerr << "Error setting " << which << " shader - "
			<< errorString(shaderErrorCode) << endl;
#else
		fprintf(stderr, "Error setting up %s shader - %s\n",
			which, errorString(shaderErrorCode));
#endif
		exit(1);
	}
}


// OpenGL initialization
void init()
{
	// Load shaders, verifying each
	flat = shaderSetup("flat.vert", "flat.frag");
	checkShaderError(flat, "flat");

	gouraud = shaderSetup("gouraud.vert", "gouraud.frag");
	checkShaderError(gouraud, "gouraud");

	phong = shaderSetup("phong.vert", "phong.frag");
	checkShaderError(phong, "phong");

	// Default shader program
	program = flat;

	// Create all four objects
	createShape(OBJ_TORUS, SHADE_FLAT);
	createShape(OBJ_TORUS, SHADE_NOT_FLAT);
	createShape(OBJ_TEAPOT, SHADE_FLAT);
	createShape(OBJ_TEAPOT, SHADE_NOT_FLAT);

	// Some openGL initialization...probably best to keep
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glDepthFunc(GL_LEQUAL);
	glClearDepth(1.0f);
}


#ifdef __cplusplus
extern "C" {
#endif

	void display(void)
	{
		// clear and draw params..
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// use the correct program
		glUseProgram(program);

		// set up Phong illumination
		setUpPhong(program, 1); // fence


		// set up viewing and projection parameters
		setUpFrustum(program);

		// set up the camera
		setUpCamera(program,
			0.2f, 3.0f, 6.5f,
			0.0f, 1.0f, 0.0f,
			0.0f, 1.0f, 0.0f
			);

		// set up transformations for the torus
		setUpTransforms(program,
			1.0f, 1.0f, 1.0f,
			-90.0f, 0.0f, 3.0f,
			0.0f, -2.0f, -3.0f);

		// draw it
		selectBuffers(program, OBJ_TORUS, currentShader);
		glDrawElements(GL_TRIANGLES, numVerts[OBJ_TORUS][currentShader],
			GL_UNSIGNED_SHORT, (void *)0);

		setUpPhong(program, 3); // fence

		// set up transformations for the teapot
		setUpTransforms(program,
			1.2f, 1.2f, 1.2f,
			-110.0f, 0.0f, -40.0f,
			1.5f, -0.5f, -1.5f);

		// draw it
		selectBuffers(program, OBJ_TEAPOT, currentShader);
		glDrawElements(GL_TRIANGLES, numVerts[OBJ_TEAPOT][currentShader],
			GL_UNSIGNED_SHORT, (void *)0);

		// swap the buffers
		glutSwapBuffers();
	}


	void keyboard(unsigned char key, int x, int y)
	{
		switch (key) {

		case '1':    // flat shading
			program = flat;
			currentShader = SHADE_FLAT;
			break;

		case '2':    // Gouraud shading
			program = gouraud;
			currentShader = SHADE_NOT_FLAT;
			break;

		case '3':    // Phong shading
			program = phong;
			currentShader = SHADE_NOT_FLAT;
			break;

		case 'a':    // animate
			animating = true;
			break;

		case 's':    // stop animating
			animating = false;
			break;

		case 033:   // terminate the program
		case 'q': case 'Q':
			exit(0);
			break;
		}

		glutPostRedisplay();
	}

	// Animate the objects (maybe)
	void animate(void) {
		if (animating) {
			angles[OBJ_TORUS] += 2;
			angles[OBJ_TEAPOT] += 1;
			glutPostRedisplay();
		}
	}

#ifdef __cplusplus
}
#endif

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(720, 720);
	glutCreateWindow("CG - Shading Assignment");

#ifndef __APPLE__
	glewInit();
#endif

	init();

	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutIdleFunc(animate);

	glutMainLoop();
	return 0;
}
