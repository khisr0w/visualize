/*  +======| File Info |===============================================================+
    |                                                                                  |
    |     Subdirectory:  /src                                                          |
    |    Creation date:  8/14/2024 2:26:40 AM                                          |
    |    Last Modified:                                                                |
    |                                                                                  |
    +======================================| Copyright © Sayed Abid Hashimi |==========+  */

/* 
 * Win32 shared library used because raylib, in its infinite wisdom, doesn't respect namespaces.
 */

#include <windows.h>

void *win32_allocate(SIZE_T size) {
    void *result = NULL;
    result = VirtualAlloc(NULL, size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    if(result == NULL) {
        GetLastError();
        exit(EXIT_FAILURE);
    }

    return result;
}
