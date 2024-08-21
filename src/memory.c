/*  +======| File Info |===============================================================+
    |                                                                                  |
    |     Subdirectory:  /src                                                          |
    |    Creation date:  8/13/2024 10:50:54 PM                                         |
    |    Last Modified:                                                                |
    |                                                                                  |
    +======================================| Copyright © Sayed Abid Hashimi |==========+  */

typedef struct {
    usize used;
    usize size;
    void *ptr;
    
    u32 temp_count;
} mem_arena;

typedef struct {
    mem_arena *arena;
    usize used;
} temp_memory;

#ifdef PLT_WIN
void *win32_allocate(usize);
#endif

inline internal void *
platform_mem_allocate(usize size) {
    void *result = NULL;

#ifdef PLT_WIN
    result = win32_allocate(size);
#endif 

#ifdef PLT_LINUX
    result = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if(result == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }
#endif 

    return result;
}

/* TODO(Abid): Implement growable memory arena. */
internal mem_arena *
mem_arena_allocate(usize bytes_to_allocate) {
    mem_arena *arena = (mem_arena *)platform_mem_allocate(sizeof(mem_arena) + bytes_to_allocate);
    arena->size = bytes_to_allocate;
    arena->used = 0;
    arena->ptr = (arena + 1);

    return arena;
}

inline internal temp_memory
mem_temp_begin(mem_arena *arena) {
    temp_memory result = {0};

    result.arena = arena;
    result.used = arena->used;

    ++arena->temp_count;

    return result;
}

inline internal void
mem_temp_end(temp_memory temp_mem) {
    mem_arena *arena = temp_mem.arena;
    assert(arena->used >= temp_mem.used, "something was freed when it shouldn't have been");
    assert(arena->temp_count > 0, "no temp memory registered for it to end");
    arena->used = temp_mem.used;
    --arena->temp_count;
}

internal inline usize
mem_aligment_offset(mem_arena *arena, usize alignment) {
	usize alignment_offset = 0;
	usize result_pointer = (usize)arena->ptr + arena->used;

	usize alignment_mask = alignment - 1;
	if(result_pointer & alignment_mask) {
		alignment_offset = alignment - (result_pointer & alignment_mask);
	}

	return alignment_offset;
}

#define mem_push_struct(arena, type) (type *)mem_push_size(arena, sizeof(type))
#define mem_push_array(arena, type, count) (type *)mem_push_size(arena, (count)*sizeof(type))
internal void *
mem_push_size(mem_arena *arena, usize size) {
    assert(arena->used + size < arena->size, "not enough arena memory");
    void *result = (u8 *)arena->ptr + arena->used;
    arena->used += size;

    return result;
}
