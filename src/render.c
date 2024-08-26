/*  +======| File Info |===============================================================+
    |                                                                                  |
    |     Subdirectory:  /src                                                          |
    |    Creation date:  8/13/2024 10:31:07 PM                                         |
    |    Last Modified:                                                                |
    |                                                                                  |
    +======================================| Copyright © Sayed Abid Hashimi |==========+  */

#include "render.h"

internal inline Vector2
vector_to_region_space(axis *region, Vector2 vector) {
    Vector2 transformed_vec = Vector2Transform(vector, region->to_parent_axis);
    return transformed_vec;
}

internal inline void
axis_push_line(axis *region, Vector2 start, Vector2 end, u32 hex_color, u32 thickness) {
    render_type *mem_type = (render_type *)mem_push_size(region->elements, sizeof(render_line) + sizeof(render_type));
    *mem_type = render_type_line;
    render_line *mem_line = (render_line *)(mem_type + 1);
    mem_line->start = start;
    mem_line->end = end;
    mem_line->hex_color = hex_color;
    mem_line->thickness = thickness;

    ++region->count;
}

internal void
axis_push_text(axis *region, char *text, Vector2 position, u32 hex_color, Font *font, f32 spacing) {
    usize text_length = str_length(text);
    usize size_to_allocate = sizeof(render_text) + sizeof(render_type) + text_length + 1;
    render_type *type = (render_type *)mem_push_size(region->elements, size_to_allocate);
    *type = render_type_text;

    render_text *axis_text = (render_text *)(type + 1);
    axis_text->position = position;
    axis_text->font = font;
    axis_text->hex_color = hex_color;
    axis_text->text = (char *)(axis_text + 1);
    axis_text->spacing = spacing;

    char *text_cpy = (char *)(axis_text + 1);
    while(*text) *(text_cpy++) = *(text++); text_cpy[text_length] = '\0';

    ++region->count;
}

internal void
axis_title(axis *region, char *text, u32 hex_color, Font *font, f32 spacing) {
    usize text_length = str_length(text);
    usize size_to_allocate = sizeof(render_text) + sizeof(render_type) + text_length + 1;
    render_type *type = (render_type *)mem_push_size(region->elements, size_to_allocate);
    *type = render_type_title;

    render_text *axis_text = (render_text *)(type + 1);
    axis_text->position = (Vector2){.x = 0.5f, .y = 1.f};
    axis_text->font = font;
    axis_text->hex_color = hex_color;
    axis_text->text = (char *)(axis_text + 1);
    axis_text->spacing = spacing;
    axis_text->text_length = text_length;

    char *text_cpy = (char *)(axis_text + 1);
    while(*text) *(text_cpy++) = *(text++); text_cpy[text_length] = '\0';

    ++region->count;
}

internal void
axis_draw_guides(axis *region) {
    axis_push_line(region, (Vector2){.x = 0, .y = 0}, (Vector2){.x = 1, .y = 0}, region->hex_bc, 2);
    axis_push_line(region, (Vector2){.x = 0, .y = 0}, (Vector2){.x = 0, .y = 1}, region->hex_bc, 2);
    axis_push_line(region, (Vector2){.x = 0, .y = 1}, (Vector2){.x = 1, .y = 1}, region->hex_bc, 2);
    axis_push_line(region, (Vector2){.x = 1, .y = 1}, (Vector2){.x = 1, .y = 0}, region->hex_bc, 2);
}

internal void
axis_draw_grids(axis *region) {
    /* NOTE(Abid): How far can the grids be squeezed before they jump to next distance. */
    assert((region->grid_stride.x > 0) && (region->grid_stride.y > 0), "stride cannot be <= 0");
    f32 threshold = 0.05;

    f32 xlim_dist = abs_(region->guide_limits.a.y - region->guide_limits.a.x);
    f32 ylim_dist = abs_(region->guide_limits.b.y - region->guide_limits.b.x);

    if(region->grid_stride.x/xlim_dist < threshold) region->grid_stride.x = round_(xlim_dist*threshold);
    if(region->grid_stride.y/ylim_dist < threshold) region->grid_stride.y = round_(xlim_dist*threshold);

    // Vector2 region->grid_stride = { .x = 1.f/grid_count.x,
    //                         .y = 1.f/grid_count.y};

    /* NOTE(Abid): Draw y line grids */
    char temp_str[512];
    for(f32 x_pos = 0.f; x_pos <= 1.0f; x_pos += region->grid_stride.x/xlim_dist) {
        axis_push_line(region, (Vector2){.x = x_pos, .y = 0}, (Vector2){.x = x_pos, .y = 1},
                       0xC8C8C8FF, 1);
        // sprintf(temp_str, "%.4f", x_pos, );
        // axis_push_text(region, temp_str);
    }

    /* NOTE(Abid): Draw x line grids */
    for(f32 y_pos = 0.f; y_pos <= 1.f; y_pos += region->grid_stride.y/ylim_dist) {
        axis_push_line(region, (Vector2){.x = 0, .y = y_pos}, (Vector2){.x = 1, .y = y_pos},
                       0xC8C8C8FF, 1);
    }
}

internal inline void
region_render_reset(axis *group) {
    group->elements->used = 0;
    group->count = 0;
}

internal void
axis_plot(axis *region, f32 *x_data, f32 *y_data, usize data_size, u32 hex_color) {
    for(usize idx = 1; idx < data_size; ++idx) {
        f32 start_x = (x_data == NULL) ? idx - 1 : x_data[idx-1];
        f32 end_x = (x_data == NULL) ? idx : x_data[idx];
        f32 start_y = y_data[idx-1];
        f32 end_y = y_data[idx];
        if(end_y == 70.0f) {
            *(&idx) = idx;
        }

        if(region->adjust_lims) {
            f32 x_potential_min = min_(start_x, end_x);
            f32 x_potential_max = max_(start_x, end_x);
            f32 y_potential_min = min_(start_y, end_y);
            f32 y_potential_max = max_(start_y, end_y);
            if(x_potential_min < region->guide_limits.a.x) region->guide_limits.a.x = x_potential_min;
            if(x_potential_max > region->guide_limits.a.y) region->guide_limits.a.y = x_potential_max;
            if(y_potential_min < region->guide_limits.b.x) region->guide_limits.b.x = y_potential_min;
            if(y_potential_max > region->guide_limits.b.y) region->guide_limits.b.y = y_potential_max;
        }
        Vector2 start = {.x = abs_(region->guide_limits.a.x - start_x)/abs_(region->guide_limits.a.y),
                         .y = abs_(region->guide_limits.b.x - start_y)/abs_(region->guide_limits.b.y)};

        Vector2 end = {.x = abs_(region->guide_limits.a.x - end_x)/abs_(region->guide_limits.a.y),
                       .y = abs_(region->guide_limits.b.x - end_y)/abs_(region->guide_limits.b.y)};
        axis_push_line(region, start, end, hex_color, 2);
    }
}

#if 0
internal inline Vector2
region_to_region(axis source, axis destination) {
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

#if 0
internal void
draw_text_center(axis *region, const char *text, Vector2 pos, u32 hex_color, Font font, f32 spacing) {
    Vector2 measure = MeasureTextEx(font, text, font.baseSize, spacing);
    Vector2 new_pos = Vector2Subtract(pos, Vector2Scale(measure, 0.5f));
    push_text_to_axis(region, text, pos, hex_color, font, spacing);

    // DrawTextEx(font, text, new_pos, font.baseSize, spacing, color);
}
#endif

internal void
region_render(axis *region) {
    ClearBackground(RAYWHITE);

    axis_draw_guides(region);
    for(u64 cnt = 0, idx = 0; cnt < region->count; ++cnt) {
        render_type *type = (render_type *)((u8 *)region->elements->ptr + idx);
        idx += sizeof(render_type);
        switch(*type) {
            case render_type_line: {
                idx += sizeof(render_line);

                render_line *line = (render_line *)(type + 1);
                Vector2 start = vector2_clamp(line->start, (Vector2){0, 0}, (Vector2){1, 1});
                Vector2 end = vector2_clamp(line->end, (Vector2){0, 0}, (Vector2){1, 1});
                Vector2 start_scrn = vector_to_region_space(region, start);
                Vector2 end_scrn = vector_to_region_space(region, end);
                DrawLineEx(start_scrn, end_scrn, line->thickness, GetColor(line->hex_color));
            } break;
            case render_type_title: {

                render_text *axis_text = (render_text *)(type + 1);
                Vector2 position_scrn = vector_to_region_space(region, axis_text->position);
                Vector2 measure = MeasureTextEx(*axis_text->font, axis_text->text,
                                                axis_text->font->baseSize, axis_text->spacing);
                Vector2 half_measure = Vector2Scale(measure, 0.5f);
                position_scrn.x -= half_measure.x;
                DrawTextEx(*axis_text->font, axis_text->text, position_scrn, axis_text->font->baseSize,
                           axis_text->spacing, GetColor(axis_text->hex_color));
                idx += sizeof(render_text) + axis_text->text_length + 1;
            } break;
            case render_type_text: {
                idx += sizeof(render_text);

                render_text *axis_text = (render_text *)(type + 1);
                Vector2 position_scrn = vector_to_region_space(region, axis_text->position);
                DrawTextEx(*axis_text->font, axis_text->text, position_scrn, axis_text->font->baseSize,
                           axis_text->spacing, GetColor(axis_text->hex_color));
            } break;
            default: { assert(0, "Invalid render type. "); break; }
        }
    }

    region_render_reset(region);
}

internal inline axis
region_create(Vector2 parent_dim, Vector2 region_dim, Vector2 position, u32 hex_bc,
              Vector2 padding, range guide_limits, Vector2 grid_stride, bool adjust_lims,
              mem_arena *Arena) {
    f32 scale_x = (1 - 2.f*padding.x)*parent_dim.x/region_dim.x;
    f32 scale_y = (1 - 2.f*padding.y)*parent_dim.y/region_dim.y;
    f32 translate_x = position.x - parent_dim.x*(0.5f - padding.x);
    f32 translate_y = position.y - parent_dim.y*(0.5f - padding.y);

    axis result = {
        .elements       = Arena,
        .parent_dim     = parent_dim,
        .count          = 0,
        .hex_bc         = hex_bc,
        .guide_limits   = guide_limits,
        .grid_stride    = grid_stride,
        .to_parent_axis = (Matrix){
            scale_x, 0.f, 0.f, translate_x,
            0.f, scale_y, 0.f, translate_y,
            0.f, 0.f, 0.f, 0.f,
            0.f, 0.f, 0.f, 0.f,
        },
        .adjust_lims = adjust_lims
    };

    return result;
}

internal inline axis
screen_region_create(Vector2 parent_dim, Vector2 region_dim, Vector2 position, u32 hex_bc,
                     Vector2 padding, range guide_limits, Vector2 grid_stride, bool adjust_lims,
                     mem_arena *Arena) {
    f32 scale_x = (1 - 2.f*padding.x)*parent_dim.x/region_dim.x;
    f32 scale_y = -(1 - 2.f*padding.y)*parent_dim.y/region_dim.y;
    f32 translate_x = position.x - parent_dim.x*(0.5f - padding.x);
    f32 translate_y = position.y + parent_dim.y*(0.5f - padding.y);

    axis result = {
        .elements       = Arena,
        .parent_dim     = parent_dim,
        .count          = 0,
        .hex_bc         = hex_bc,
        .guide_limits   = guide_limits,
        .grid_stride    = grid_stride,
        .to_parent_axis = (Matrix){
            scale_x, 0.f, 0.f, translate_x,
            0.f, scale_y, 0.f, translate_y,
            0.f, 0.f, 0.f, 0.f,
            0.f, 0.f, 0.f, 0.f,
        },
        .adjust_lims = adjust_lims
    };

    return result;
}
