/*  +======| File Info |===============================================================+
    |                                                                                  |
    |     Subdirectory:  /visualize                                                    |
    |    Creation date:  8/13/2024 5:19:23 PM                                          |
    |    Last Modified:                                                                |
    |                                                                                  |
    +======================================| Copyright © Sayed Abid Hashimi |==========+  */

#ifdef PLT_LINUX
#include <stdlib.h>
#include <stddef.h>
#include <sys/mman.h>
#endif

#include <stdint.h>
#include <stdio.h>
#include "raylib.h"
#include "raymath.h"

#include "utils.c"
#include "memory.c"
#include "render.c"

internal void
draw_guides(render_region *render) {
    push_line_to_render(render, (render_line){
        .start = {.x = 0, .y = 0},
        .end = {.x = 1, .y = 0},
        .color = ORANGE,
        .thickness = 2
    });

    push_line_to_render(render, (render_line){
        .start = {.x = 0, .y = 0},
        .end = {.x = 0, .y = 1},
        .color = ORANGE, 
        .thickness = 2
    });
    push_line_to_render(render, (render_line){
        .start = {.x = 0, .y = 1},
        .end = {.x = 1, .y = 1},
        .color = ORANGE,
        .thickness = 2
    });
    push_line_to_render(render, (render_line){
        .start = {.x = 1, .y = 1},
        .end = {.x = 1, .y = 0},
        .color = ORANGE,
        .thickness = 2
    });
}

internal void
draw_grids(render_region *render, range guide_limits, Vector2 grid_stride) {
    /* NOTE(Abid): How far can the grids be squeezed before they jump to next distance. */
    assert((grid_stride.x > 0) && (grid_stride.y > 0), "stride cannot be <= 0");
    f32 threshold = 0.05;

    f32 xlim_dist = abs_(guide_limits.a.y - guide_limits.a.x);
    f32 ylim_dist = abs_(guide_limits.b.y - guide_limits.b.x);

    if(grid_stride.x/xlim_dist < threshold) grid_stride.x = round_(xlim_dist*threshold);
    if(grid_stride.y/ylim_dist < threshold) grid_stride.y = round_(xlim_dist*threshold);

    // Vector2 grid_stride = { .x = 1.f/grid_count.x,
    //                         .y = 1.f/grid_count.y};

    /* NOTE(Abid): Draw y line grids */
    for(f32 x_pos = 0.f; x_pos <= 1.0f; x_pos += grid_stride.x/xlim_dist) {
        push_line_to_render(render, (render_line){
            .start = {.x = x_pos, .y = 0},
            .end = {.x = x_pos, .y = 1},
            .color = LIGHTGRAY,
            .thickness = 1
        });
    }

    /* NOTE(Abid): Draw x line grids */
    for(f32 y_pos = 0.f; y_pos <= 1.f; y_pos += grid_stride.y/ylim_dist) {
        push_line_to_render(render, (render_line){
            .start = {.x = 0, .y = y_pos},
            .end = {.x = 1, .y = y_pos},
            .color = LIGHTGRAY,
            .thickness = 1
        });
    }
}

internal void
draw_text_center(Font font, const char *text, Vector2 pos, Color color, float spacing) {
    Vector2 measure = MeasureTextEx(font, text, font.baseSize, spacing);
    Vector2 new_pos = Vector2Subtract(pos, Vector2Scale(measure, 0.5f));
    DrawTextEx(font, text, new_pos, font.baseSize, 2, color);
}

i32 main() {
    /* NOTE(Abid): Define parameters. */
    Vector2 screen_dim = { .x = 800.f, .y = 800.f };
    Vector2 padding_around_regions = {0.06f, 0.06f};
    mem_arena *main_arena = mem_arena_allocate(megabyte(400));
    render_region main_region = screen_region_create(
        screen_dim, (Vector2){1.f, 1.f}, Vector2Scale(screen_dim, 0.5f), RED, padding_around_regions,
        main_arena
    );
    range guide_limits = {
        .a = (Vector2){0.f, 100.f},
        .b = (Vector2){0.f, 100.f}
    };
    Vector2 grid_stride = { .x = 1.f, .y = 1.f};
    /* NOTE(Abid): Initializing the window and drawing context. */
    SetTraceLogLevel(LOG_NONE);
    InitWindow(screen_dim.x, screen_dim.y, "Visualization");
    SetTargetFPS(60);
    char *jet_font_dir = "resource/JetBrainsMono-Regular.ttf";
    Font font_20 = LoadFontEx(jet_font_dir, 20, NULL, 0); // Load font from file with extended parameters, use NULL for codepoints and 0 for codepointCount to load the default character set
    Font font_40 = LoadFontEx(jet_font_dir, 40, NULL, 0); // Load font from file with extended parameters, use NULL for codepoints and 0 for codepointCount to load the default character set

    while(!WindowShouldClose()) {
        //i32 fps = GetFPS(); char fps_str[64]; sprintf(fps_str, "%i", fps);
        //Vector2 font_measure = MeasureTextEx(font_40, fps_str, font_40.baseSize, 2);
        //DrawTextEx(font_40, fps_str, (Vector2){400.f-(font_measure.x/2), 400.f-((font_measure.y/2))}, font_40.baseSize, 2, DARKGRAY);

        BeginDrawing(); {
            draw_grids(&main_region, guide_limits, grid_stride);
            draw_guides(&main_region);
            draw_text_center(font_40, "Placeholder title", (Vector2){400, 30}, RED, 2);

            region_render(&main_region);
        }; EndDrawing();
    }
    
    CloseWindow();

    return 0;
}
