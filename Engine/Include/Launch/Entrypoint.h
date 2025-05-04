#pragma once

// ONLY INCLUDE THIS ONE TIME IN ONE .CPP FILE

#include "Core/Log.h"
#include "Engine/Engine.h"

int main(int argc, char** argv)
{
	using namespace QE;

	Log::Init();
	LOG_INFO("Hello from the Entrypoint for Quest Engine!");

	g_Engine.Initialize();
	g_Engine.Run();
	g_Engine.Shutdown();

	return 0;
}