#include "Core/Log.h"

#include "Engine/Engine.h"

int main(int argc, char** argv)
{
	LOG("Engine", Trace, "Hello World");

	QE::Engine::Get().Initialize();
	QE::Engine::Get().Run();
	QE::Engine::Get().Shutdown();

	return 0;
}