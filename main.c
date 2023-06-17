#include <stdio.h>
#include <dlfcn.h>

#define DEFAULT_ENTRY "entry"
#define DEBUG_PRINT(format, item) { if (argc >= 4) { printf("DBG: "format, item); } }
#define DEBUG_PRINT2(format, item, item2) { if (argc >= 4) { printf("DBG: "format, item, item2); } }

char * program_name;
char * entry_symbol = DEFAULT_ENTRY;

enum {
	ENUM_SUCCESS = 0,
	ENUM_RELOAD,
};

void print_help() {
	printf("%s [library file] (entry symbol) (debug?)\n", program_name);
}

int main(int argc, char ** argv) {
	program_name = argv[0];

	if (argc < 2) {
		print_help();
		return 0;
	}

	if (argc >= 3) {
		entry_symbol = argv[2];
	}

	void * library;
	int (* entry)();
	int ret = 0;

open:
	DEBUG_PRINT("lib %p\n", library);
	printf("nanoreload: opening library \"%s\"\n", argv[1]);
	library = dlopen(argv[1], RTLD_NOW);
	DEBUG_PRINT("lib %p\n", library);
	if (library == NULL) {
		fprintf(stderr, "nanoreload: %s\n", dlerror());
		return -1;
	}

	DEBUG_PRINT2("entry \"%s\" %p\n", entry_symbol, entry);
	printf("nanoreload: loading symbol \"%s\"\n", entry_symbol);
	*((void **) &entry) = dlsym(library, entry_symbol);
	DEBUG_PRINT2("entry \"%s\" %p\n", entry_symbol, entry);
	if (entry == NULL) {
		fprintf(stderr, "nanoreload: %s\n", dlerror());
		return -2;
	}

	printf("nanoreload: calling symbol \"%s\"\n", entry_symbol);
	ret = entry();
	switch (ret) {
		case ENUM_RELOAD:
			printf("nanoreload: reloading\n");
			dlclose(library);
			library = NULL;
			entry = NULL;
			goto open;
		case ENUM_SUCCESS:
		default:
			printf("nanoreload: closing library\n");
			dlclose(library);
			break;
	}

	return 0;
}
