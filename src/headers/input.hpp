#ifndef INPUT_HPP
#define INPUT_HPP

#include <glad/glad.h>

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

#endif