#ifndef INPUTHANDLER_HPP
#define INPUTHANDLER_HPP
#pragma once

class GLFWwindow;

namespace Input
{

void KeyCallBack(GLFWwindow* window, int32_t key, int32_t scancode, int32_t action, int32_t mods);

} // namespace Input


#endif