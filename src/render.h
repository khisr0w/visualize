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
    render_type_text,
    render_type_title,

    render_type_count,
} render_type;

typedef struct {
    Vector2 start;
    Vector2 end;
    u32 hex_color;
    i32 thickness;
} render_line;

typedef struct {
    Vector2 position;
    u32 hex_color;
    f32 spacing;
    Font *font;
    usize text_length;
    char *text;
} render_text;

typedef struct {
    Matrix to_parent_axis;
    mem_arena *elements;
    i32 count;
    u32 hex_bc;
    Vector2 parent_dim;
    char *title;
    bool adjust_lims;

    range guide_limits;
    Vector2 grid_stride;
} axis;

#define RENDER_H
#endif
