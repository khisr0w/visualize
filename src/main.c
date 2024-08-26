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

#if 0
internal inline void
draw_fps(Font font, Vector2 Pos) {
    i32 fps = GetFPS(); char fps_str[64]; sprintf(fps_str, "FPS: %i", fps);
    Vector2 font_measure = MeasureTextEx(font, fps_str, font.baseSize, 2);
    DrawTextEx(font, fps_str, (Vector2){400.f-(font_measure.x/2), 400.f-((font_measure.y/2))}, font.baseSize, 2, DARKGRAY);
}
#endif

i32 main() {
    /* NOTE(Abid): Define parameters. */
    Vector2 screen_dim = { .x = 800.f, .y = 800.f };
    Vector2 padding_around_regions = {.x = 0.06f, .y = 0.06f};
    mem_arena *main_arena = mem_arena_allocate(megabyte(400));
    range guide_limits = {
        .a = (Vector2){.x = 0.f, .y = 100.f},
        .b = (Vector2){.x = 0.f, .y = 100.f}
    };
    Vector2 grid_stride = {.x = 1.f, .y = 1.f};
    axis main_region = screen_region_create(
        screen_dim, (Vector2){.x = 1.f, .y = 1.f},
        Vector2Scale(screen_dim, 0.5f), 0xFFAF00FF,
        padding_around_regions, guide_limits,
        grid_stride, true, main_arena
    );
    /* NOTE(Abid): Initializing the window and drawing context. */
    SetTraceLogLevel(LOG_NONE);
    InitWindow(screen_dim.x, screen_dim.y, "Visualization");
    SetTargetFPS(60);
    char *jet_font_dir = "resource/JetBrainsMono-Regular.ttf";
    Font font_20 = LoadFontEx(jet_font_dir, 20, NULL, 0); // Load font from file with extended parameters, use NULL for codepoints and 0 for codepointCount to load the default character set
    Font font_40 = LoadFontEx(jet_font_dir, 40, NULL, 0); // Load font from file with extended parameters, use NULL for codepoints and 0 for codepointCount to load the default character set

#define data_points 4000 
    f32 y_data[data_points];
    f32 x_data[data_points];
    f32 y_pow = 0.f;
    for(u32 idx = 0; idx < data_points; ++idx) {
        y_data[idx] = y_pow*sinf(idx * 0.1);
        x_data[idx] = idx * 0.1;
        y_pow += 0.03;
    }

    usize data_until = 0;
    while(!WindowShouldClose()) {
        BeginDrawing(); {
            axis_draw_grids(&main_region);
            axis_plot(&main_region, x_data, y_data, data_until, ColorToInt(PURPLE));
            axis_title(&main_region, "Main map plot", ColorToInt(BLUE), &font_40, 2);

            region_render(&main_region);
        }; EndDrawing();
        data_until = min_(data_until + 10, data_points);
    }
    
    CloseWindow();
    return 0;
}
