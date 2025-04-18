#include "Core/Log.h"

#include "Engine/Engine.h"

int main(int argc, char** argv)
{
	QE::Log::Init();
	//LOG("Sandbox", Info, "Hello, Sandbox!");
	LOG_INFO("Hello, Sandbox!");

	QE::Engine::Get().Initialize();
	QE::Engine::Get().Run();
	QE::Engine::Get().Shutdown();

	return 0;
}