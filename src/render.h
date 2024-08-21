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
    vector2i screen_dim;
    Vector2 screen_padding; // Essentially it defines the padding around the plot.

    mem_arena *elements;
    i32 count;
} render_group;

#define RENDER_H
#endif
