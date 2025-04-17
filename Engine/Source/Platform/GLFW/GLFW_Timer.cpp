#include "Platform/PlatformUtility.h"

#include "GLFW/glfw3.h"

namespace QE
{
	double GetTime()
	{
		if (glfwInit())
			return glfwGetTime();
		else
			return 0.0f;
	}
}