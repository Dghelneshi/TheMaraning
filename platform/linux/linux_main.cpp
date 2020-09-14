#include "main\common.h"
#include "main\main.h"

int __cdecl main(int argc, const char* argv[])
{
	// Linux has no guarantees about argv encoding, YOLO
	std::unique_ptr<std::unique_ptr<char[]>[]> argv_utf8(new std::unique_ptr<char[]>[argc]);
	for (size_t i = 0; i < argc; ++i) {
		argv_utf8[i].reset(_strdup(argv[i]));
	}

	return Main::Main(argc, std::move(argv_utf8));
}
