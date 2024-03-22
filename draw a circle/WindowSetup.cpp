#include "WindowSetup.h"
bool WindowSetup::canInit = true;
void WindowSetup::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
	std::cout << width << "," << height << std::endl;
}
WindowSetup::WindowSetup(const unsigned window_width, const unsigned window_height, const char* window_title, GLFWmonitor* monitor, GLFWwindow* share) {
	if (canInit) glfwConfiguration();

	window = createWindow(window_width, window_height, window_title, monitor, share);

	if (canInit) {
		makeCurrentContext(window);
		loadGlad();
		canInit = false;
	}
	glViewport(0, 0, window_width, window_height);
}
void WindowSetup::glfwConfiguration() const {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
}
void WindowSetup::makeCurrentContext(GLFWwindow* window) const {
	if (!window) {
		std::cout << "[Fatal Error]: Couldn't create the window\n";
		glfwTerminate();
		exit(-1);
	}
	glfwMakeContextCurrent(window);
}
void WindowSetup::loadGlad() const {
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "[Fatal Error]: Couldn't load glad\n";
		glfwTerminate();
		exit(-1);
	}
}
GLFWwindow* WindowSetup::createWindow(unsigned w_w, unsigned w_h, const char* t, GLFWmonitor* mo, GLFWwindow* sh) {
	return glfwCreateWindow(w_w, w_h, t, mo, sh);
}