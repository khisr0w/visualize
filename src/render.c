/*  +======| File Info |===============================================================+
    |                                                                                  |
    |     Subdirectory:  /src                                                          |
    |    Creation date:  8/13/2024 10:31:07 PM                                         |
    |    Last Modified:                                                                |
    |                                                                                  |
    +======================================| Copyright © Sayed Abid Hashimi |==========+  */

#include "render.h"


internal inline Vector2
vector_to_screen_space(render_group *group, Vector2 vector) {
    Matrix matrix = {
        1 - 2*group->screen_padding.x, 0.f, 0.f, group->screen_padding.x,      // NOTE(Abid): m8 ignored
        0.f, 1 - 2*group->screen_padding.y, 0.f, group->screen_padding.y,      // NOTE(Abid): m9 ignored
        0.f, 0.f, 0.f, 0.f,     // NOTE(Abid): ignored.
        0.f, 0.f, 0.f, 0.f      // NOTE(Abid): ignored.
    };

    Vector2 transformed_vec = Vector2Transform(vector, matrix);
    return (Vector2){transformed_vec.x * group->screen_dim.x, 
                     (1 - transformed_vec.y) * group->screen_dim.y};
}

push_line_to_render(render_group *render, render_line line) {
    render_type *mem_type = (render_type *)mem_push_size(render->elements, sizeof(render_line) + sizeof(render_type));
    *mem_type = render_type_line;
    render_line *mem_line = (render_line *)(mem_type + 1);
    mem_line->start = line.start;
    mem_line->end = line.end;
    mem_line->color = line.color;
    mem_line->thickness = line.thickness;

    ++render->count;
}

internal inline void
render_reset(render_group *group) {
    group->elements->used = 0;
    group->count = 0;
}

/* TODO(Abid): Must implement a z buffer for ordering of drawing elements. */
internal void
render(render_group *group) {
    ClearBackground(RAYWHITE);

    for(u64 cnt = 0, idx = 0; cnt < group->count; ++cnt) {
        render_type *type = (render_type *)((u8 *)group->elements->ptr + idx);
        idx += sizeof(render_type);
        switch(*type) {
            case render_type_line: {
                idx += sizeof(render_line);

                render_line *line = (render_line *)(type + 1);
                Vector2 start_scrn = vector_to_screen_space(group, line->start);
                Vector2 end_scrn = vector_to_screen_space(group, line->end);
                DrawLineEx(start_scrn, end_scrn, line->thickness, line->color);
            } break;
            default: { assert(0, "Invalid render type. "); break; }
        }
    }
    render_reset(group);
}

