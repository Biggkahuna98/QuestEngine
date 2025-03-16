#include "Core/QuestEngineTest.h"
#include "Core/Log.h"

int main(int argc, char** argv)
{
	QE::QuestHelloWorld();
	LOG(Engine, Trace, "Hello World");
}