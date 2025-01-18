#ifndef INPUT_INPUTHANDLER_HPP
#define INPUT_INPUTHANDLER_HPP
#pragma once

// Forward declarations
class GLFWwindow;

namespace Input
{

void KeyCallBack(GLFWwindow* window, int32_t key, int32_t scancode, int32_t action, int32_t mods);

} // namespace Input


#endif