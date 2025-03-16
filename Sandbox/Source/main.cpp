#include "Core/Log.h"

#include "Engine/Engine.h"

int main(int argc, char** argv)
{
	QE::Log::Get().AddLogger("Sandbox");
	LOG("Sandbox", Info, "Hello, Sandbox!");

	QE::Engine::Get().Initialize();
	QE::Engine::Get().Run();
	QE::Engine::Get().Shutdown();

	return 0;
}