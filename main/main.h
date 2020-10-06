#pragma once
#include "thirdparty/eastl/string.h"
#include "thirdparty/eastl/unique_ptr.h"

struct MainArgs
{
	const char** argv;
	int argc;

	MainArgs(int argc, const char** argv) : argv(argv), argc(argc) {}
	MainArgs(const MainArgs& other) = delete;
	MainArgs(MainArgs&& other) noexcept {
		argv = other.argv;
		argc = other.argc;
		other.argc = 0;
		other.argv = nullptr;
	}
	void Destroy()
	{
		for (int i = 0; i < argc; i++)
		{
			delete[] argv[i];
		}
		delete[] argv;
		argc = 0;
		argv = nullptr;
	}
	~MainArgs()
	{
		Destroy();
	}
};

namespace Main
{
	int Main(MainArgs args);
	void Exit();
}