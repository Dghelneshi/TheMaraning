#pragma once
#include <memory>

namespace Main
{
	int Main(int argc, std::unique_ptr<std::unique_ptr<char[]>[]> argv);
	void Exit();
}