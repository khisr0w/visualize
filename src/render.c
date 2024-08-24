/*  +======| File Info |===============================================================+
    |                                                                                  |
    |     Subdirectory:  /src                                                          |
    |    Creation date:  8/13/2024 10:31:07 PM                                         |
    |    Last Modified:                                                                |
    |                                                                                  |
    +======================================| Copyright © Sayed Abid Hashimi |==========+  */

#include "render.h"

typedef struct {
    Vector2 top_left;
    Vector2 bottom_right;
    Color border_color;
} draw_rect;

internal inline Vector2
vector_to_region_space(render_region *region, Vector2 vector) {
    // Matrix matrix = {
    //     1.f, 0.f, 0.f, position.x,
    //     0.f, 1.f, 0.f, position.y,
    //     0.f, 0.f, 0.f, 0.f,
    //     0.f, 0.f, 0.f, 0.f,
    // };

    Vector2 transformed_vec = Vector2Transform(vector, region->to_main_region);
    return transformed_vec;
}

internal inline void
push_line_to_render(render_region *render, render_line line) {
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
region_render_reset(render_region *group) {
    group->elements->used = 0;
    group->count = 0;
}


#if 0
internal inline Vector2
region_to_region(render_region source, render_region destination) {
    Matrix matrix = {
        1 - 2*group->padding.x, 0.f, 0.f, group->padding.x, // NOTE(Abid): m8 ignored
        0.f, 1 - 2*group->padding.y, 0.f, group->padding.y, // NOTE(Abid): m9 ignored
        0.f, 0.f, 0.f, 0.f,     // NOTE(Abid): ignored.
        0.f, 0.f, 0.f, 0.f      // NOTE(Abid): ignored.
    };

    Vector2 transformed_vec = Vector2Transform(vector, matrix);
    return (Vector2){transformed_vec.x * group->screen_dim.x, 
                     (1 - transformed_vec.y) * group->screen_dim.y};
}
#endif

/* TODO(Abid): Must implement a z-buffer for ordering of drawing elements. 
 *             Or maybe the code order is your z-buffer for now? 
 */


// internal inline Vector2
// vector_to_screen_space(Vector2 screen_dim, Vector2 vector) {
//     return (Vector2) {
//         vector.x * screen_dim.x, (1 - vector.y)*screen_dim.y
//     };
// }

internal void
region_render(render_region *region) {
    ClearBackground(RAYWHITE);

    for(u64 cnt = 0, idx = 0; cnt < region->count; ++cnt) {
        render_type *type = (render_type *)((u8 *)region->elements->ptr + idx);
        idx += sizeof(render_type);
        switch(*type) {
            case render_type_line: {
                idx += sizeof(render_line);

                render_line *line = (render_line *)(type + 1);
                Vector2 start_scrn = vector_to_region_space(region, line->start);
                Vector2 end_scrn = vector_to_region_space(region, line->end);
                DrawLineEx(start_scrn, end_scrn, line->thickness, line->color);
            } break;
            default: { assert(0, "Invalid render type. "); break; }
        }
    }
    region_render_reset(region);
}

internal inline render_region
region_create(Vector2 parent_dim, Vector2 region_dim, Vector2 position, Color border_color,
              Vector2 padding, mem_arena *Arena) {
    render_region result = {
        .elements = Arena,
        .count = 0,
        .border_color = border_color,
        .to_main_region = (Matrix){
            (1 - 2.f*padding.x)*parent_dim.x/region_dim.x, 0.f, 0.f, position.x - parent_dim.x*(0.5f - padding.x),
            0.f, (1 - 2.f*padding.y)*parent_dim.y/region_dim.y, 0.f, position.y - parent_dim.y*(0.5f - padding.y),
            0.f, 0.f, 0.f, 0.f,
            0.f, 0.f, 0.f, 0.f,
        }
    };

    return result;
}

internal inline render_region
screen_region_create(Vector2 parent_dim, Vector2 region_dim, Vector2 position, Color border_color, Vector2 padding, mem_arena *Arena) {
    f32 scale_x = (1 - 2.f*padding.x)*parent_dim.x/region_dim.x;
    f32 scale_y = -(1 - 2.f*padding.y)*parent_dim.y/region_dim.y;
    f32 translate_x = position.x - parent_dim.x*(0.5f - padding.x);
    f32 translate_y = position.y + parent_dim.y*(0.5f - padding.y);

    render_region result = {
        .elements = Arena,
        .count = 0,
        .border_color = border_color,
        .to_main_region = (Matrix){
            scale_x, 0.f, 0.f, translate_x,
            0.f, scale_y, 0.f, translate_y,
            0.f, 0.f, 0.f, 0.f,
            0.f, 0.f, 0.f, 0.f,
        }
    };

    return result;
}
