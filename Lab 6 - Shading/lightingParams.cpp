//
//  lightingParams
//
//  Created by Joe Geigel on 1/23/13.
//
//  This code can be compiled as either C or C++.
//

#include "lightingParams.h"

// Add any global variables you need here.
// Material properties of the teapot and torus
float ambient_color[] = { 0.5f, 0.1f, 0.9f, 1.0f },
	ambient_reflection_coefficient = 0.5f,
	diffuse_color[] = { 0.89f, 0.0f, 0.0f, 1.0f },
	diffuse_reflection_coefficient = 0.7f,
	specular_color[] = { 1.0f, 1.0f, 1.0f, 1.0f },
	specular_exponent = 10.0f,
	specular_reflection_coefficient = 1.0f;

// Properties of the light source
float lightSourceColor[] = { 1.0f, 1.0f, 0.0f, 1.0f },
	lightSourcePosition[] = { 0.0f, 5.0f, 2.0f, 1.0f };

// Properties of the ambient light
float ambientLightColor[] = { 0.5f, 0.5f, 0.5f, 1.0 };

/**
 * This function sets up the lighting, material, and shading parameters
 * for the Phong shader.
 *
 * You will need to write this function, and maintain all of the values
 * needed to be sent to the vertex shader.
 *
 * @param program - The ID of an OpenGL (GLSL) shader program to which
 *    parameter values are to be sent
 *
 */
void setUpPhong( GLuint program )
{
    // Add your code here.
	// we basically send each information to the shaders
	glUniform4fv(
		glGetUniformLocation(program, "lightSourceColor"),
		1, lightSourceColor);
	glUniform4fv(
		glGetUniformLocation(program, "lightSourcePosition"),
		1, lightSourcePosition);
	glUniform4fv(
		glGetUniformLocation(program, "ambient_color"),
		1, ambient_color);
	glUniform4fv(
		glGetUniformLocation(program, "diffuse_color"),
		1, diffuse_color);
	glUniform4fv(
		glGetUniformLocation(program, "specular_color"),
		1, specular_color);

	glUniform1f(
		glGetUniformLocation(program, "ambient_reflection_coefficient"),
		ambient_reflection_coefficient);
	glUniform1f(
		glGetUniformLocation(program, "diffuse_reflection_coefficient"),
		diffuse_reflection_coefficient);
	glUniform1f(
		glGetUniformLocation(program, "specular_reflection_coefficient"),
		specular_reflection_coefficient);
	glUniform1f(
		glGetUniformLocation(program, "specular_exponent"),
		specular_exponent);
}
