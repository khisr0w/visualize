/*  +======| File Info |===============================================================+
    |                                                                                  |
    |     Subdirectory:  /src                                                          |
    |    Creation date:  8/14/2024 4:29:20 AM                                          |
    |    Last Modified:                                                                |
    |                                                                                  |
    +======================================| Copyright © Sayed Abid Hashimi |==========+  */

#if !defined(RENDER_H)

typedef enum {
    render_type_line,

    render_type_count,
} render_type;

typedef struct {
    Vector2 start;
    Vector2 end;
    Color color;
    i32 thickness;

} render_line;

typedef struct {
    Color border_color;
    Matrix to_main_region;
    Vector2 padding;

    mem_arena *elements;
    i32 count;
} render_region;

#define RENDER_H
#endif
