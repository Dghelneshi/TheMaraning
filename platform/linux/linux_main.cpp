#include "main\common.h"
#include "main\main.h"

int __cdecl main(int argc, const char* argv[])
{
	// Linux has no guarantees about argv encoding, YOLO
	const char** argv_utf8 = new const char* [argc];
	for (size_t i = 0; i < argc; ++i) {
		argv_utf8[i] = _strdup(argv[i]);
	}

	return Main::Main(MainArgs(argc, argv));
}
