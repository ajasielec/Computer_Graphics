#pragma once
#include <iostream>
#include <glad/glad.h>
#include <glfw3.h>
/*
	Window configuration class, 
		HOW TO USE IT:
	* Just create class object in main file
	* If you create first object, the glfw and glad configurations will proceed
	* If you create another object, you will only be able to create and manage another created window
	* to swap between window contexts just use openGL function

*/
class WindowSetup
{
public:
	static bool canInit;
	static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	GLFWwindow* window = NULL;
	WindowSetup(const unsigned window_width, const unsigned window_height, const char* window_title, GLFWmonitor* monitor, GLFWwindow* share);

private:
	void glfwConfiguration() const;
	void makeCurrentContext(GLFWwindow* window) const;
	void loadGlad() const;
	
	GLFWwindow* createWindow(unsigned w_w, unsigned w_h, const char* t, GLFWmonitor* mo, GLFWwindow* sh);

};

