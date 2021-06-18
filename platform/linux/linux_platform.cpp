#include "main\common.h"
#include "main\main.h"
#include <locale.h>
#include <string.h>

int __cdecl main(int argc, const char* argv[])
{
	const char* locale = setlocale(LC_ALL, ".UTF8"); // enable UTF8 for the CRT
	TM_ASSERT(locale && (strstr(locale, ".UTF8") || strstr(locale, ".utf8") || strstr(locale, ".UTF-8") || strstr(locale, ".utf-8")));

	// Linux has no guarantees about argv encoding, YOLO
	const char** argv_utf8 = new const char* [argc];
	for (size_t i = 0; i < argc; ++i) {
		argv_utf8[i] = _strdup(argv[i]);
	}

	return Main::Main(MainArgs(argc, argv));
}
