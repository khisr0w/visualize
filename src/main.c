/*  +======| File Info |===============================================================+
    |                                                                                  |
    |     Subdirectory:  /visualize                                                    |
    |    Creation date:  8/13/2024 5:19:23 PM                                          |
    |    Last Modified:                                                                |
    |                                                                                  |
    +======================================| Copyright © Sayed Abid Hashimi |==========+  */

#include <stdint.h>
#include "raylib.h"
#include "raymath.h"

#include "utils.c"
#include "memory.c"
#include "render.c"

internal void
draw_guides(render_group *render) {
    push_line_to_render(render, (render_line){
        .start = {.x = 0, .y = 0},
        .end = {.x = 1, .y = 0},
        .color = BLACK,
        .thickness = 2
    });

    push_line_to_render(render, (render_line){
        .start = {.x = 0, .y = 0},
        .end = {.x = 0, .y = 1},
        .color = BLACK,
        .thickness = 2
    });
}

internal void
draw_grids(render_group *render, range guide_limits, vector2i grid_count) {
    /* NOTE(Abid): How far can the grids be squeezed before they jump to next distance. */
    f32 threshold = 0.005;

    f32 xlim_dist = abs(guide_limits.a.y - guide_limits.a.x);
    f32 ylim_dist = abs(guide_limits.b.y - guide_limits.b.x);
    Vector2 grid_stride = { .x = 1.f/grid_count.x,
                            .y = 1.f/grid_count.y};

    /* NOTE(Abid): Draw y line grids */
    for(f32 x_pos = 0.f; x_pos <= 1.0f; x_pos += grid_stride.x) {
        push_line_to_render(render, (render_line){
            .start = {.x = x_pos, .y = 0},
            .end = {.x = x_pos, .y = 1},
            .color = LIGHTGRAY,
            .thickness = 1
        });
    }

    /* NOTE(Abid): Draw x line grids */
    for(f32 y_pos = 0.f; y_pos <= 1.f; y_pos += grid_stride.y) {
        push_line_to_render(render, (render_line){
            .start = {.x = 0, .y = y_pos},
            .end = {.x = 1, .y = y_pos},
            .color = LIGHTGRAY,
            .thickness = 1
        });
    }
}

i32 main() {
    /* NOTE(Abid): Define parameters. */
    vector2i screen_dim = { .x = 800.f, .y = 800.f };
    render_group main_render = {
        .screen_dim = screen_dim,
        .elements = mem_arena_allocate(megabyte(400)),
        .count = 0,
        .screen_padding = { .x = 0.06f, .y = 0.06f }
    };

    range guide_limits = {
        .a = (Vector2){0.f, 100.f},
        .b = (Vector2){0.f, 100.f}
    };
    /* NOTE(Abid): Initializing the window and drawing context. */
    SetTraceLogLevel(LOG_NONE);
    InitWindow(screen_dim.x, screen_dim.y, "Visualization");
    SetTargetFPS(60);

    while(!WindowShouldClose()) {
        BeginDrawing(); {
            draw_grids(&main_render, guide_limits, (vector2i){20, 20});
            draw_guides(&main_render);

            render(&main_render);
        }; EndDrawing();
    }
    
    CloseWindow();

    return 0;
}
