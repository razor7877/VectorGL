#ifndef INPUT_HPP
#define INPUT_HPP

// This file contains functions responsible for handling and processing the various user inputs
// such as mouse movement, clicking, scrolling, and keyboard presses

extern const int WINDOW_WIDTH;
extern const int WINDOW_HEIGHT;

extern int windowWidth;
extern int windowHeight;

// Callback function to register mouse input
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
// Callback function to register scrolling
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
// Used to register only first key press of a given key
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
// Used to process various key presses like movement keys
void processInput(GLFWwindow* window, float& deltaTime);
// Callback function to handle window size change
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

#endif