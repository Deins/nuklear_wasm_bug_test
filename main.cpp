#include <iostream>
#include <cassert>
#include <cstdio>
#include <cmath>
#include <cinttypes>
using namespace std;

// Include nuklear
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_ZERO_COMMAND_MEMORY          
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_ASSERT(x) assert((x))
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_MEMCPY memcpy
#define NK_MEMSET memset
#define NK_BYTE uint8_t
#define NK_INT16 int16_t
#define NK_UINT16 uint16_t
#define NK_INT32 int32_t
#define NK_UINT32 uint32_t
#define NK_SIZE_TYPE uint32_t
//#define NK_VSNPRINTF

#define NK_SQRT sqrt
#define NK_SIN sin
#define NK_COS cos
#define NK_STATIC_ASSERT(x) static_assert((x), "NUKLEAR ASSERT!")
#define NK_ZERO_COMMAND_MEMORY

#define NK_IMPLEMENTATION
#include "nuklear/nuklear.h"

nk_context ctx;
struct NkDevice {
	struct Vert
	{
		float pos[2];
		float uv[2];
		uint8_t col[4];
	};
	nk_font_atlas font_atlas;
	nk_draw_null_texture null_texture;
	nk_buffer cmds;
	alignas(64) Vert verts[1024 * 32];
	alignas(64) uint16_t indices[1024 * 32];
	alignas(64) uint8_t memory_nk[1024 * 8];
	alignas(64) uint8_t memory_cmds[1024];
} gNkDevice;

void Init()
{	// GUI Init
    cout << "Init!" << endl;
    nk_init_default(&ctx, nullptr);
    nk_buffer_init_default(&gNkDevice.cmds);

    // font
    nk_font_atlas font_atlas = gNkDevice.font_atlas;
    nk_font_atlas_init_default(&font_atlas); // todo: fix allocation buffers
    int gPreMadeTex_White = 0;
    gNkDevice.null_texture = { nk_handle_id((int)gPreMadeTex_White), nk_vec2(0.0f, 0.0f) }; // texture handle to a 1x1 white texture

    nk_font_atlas_begin(&font_atlas);
    const void *image; int w, h;
    image = nk_font_atlas_bake(&font_atlas, &w, &h, NK_FONT_ATLAS_RGBA32);
    unsigned tex = 1; //CreateAndUploadTexture(UploadTexture(tex, (RGBA*)image, Vec2i(w, h), false)
    nk_font_atlas_end(&font_atlas, nk_handle_id((int)tex), &gNkDevice.null_texture);

    nk_style_set_font(&ctx, &font_atlas.default_font->handle); // default font		
}

void Render()
{
    cout << "Render!" << endl;
    // Just generate vertex & indice buffers from nuklear, without actual gpu rendering
    {
        // convert from command queue into draw list
        nk_convert_config config;
        constexpr nk_draw_vertex_layout_element vertex_layout[] = {
            { NK_VERTEX_POSITION, NK_FORMAT_FLOAT, offsetof(NkDevice::Vert, pos) },
            { NK_VERTEX_TEXCOORD, NK_FORMAT_FLOAT, offsetof(NkDevice::Vert, uv) },
            { NK_VERTEX_COLOR, NK_FORMAT_R8G8B8A8, offsetof(NkDevice::Vert, col) },
            { NK_VERTEX_LAYOUT_END }
        };
        memset(&config, 0, sizeof(config));
        config.vertex_layout = vertex_layout;
        config.vertex_size = sizeof(NkDevice::Vert);
        config.vertex_alignment = alignof(NkDevice::Vert);
        config.null = gNkDevice.null_texture;
        config.circle_segment_count = 22;
        config.curve_segment_count = 22;
        config.arc_segment_count = 22;
        config.global_alpha = 1.0f;
        config.shape_AA = nk_anti_aliasing::NK_ANTI_ALIASING_OFF;
        config.line_AA = nk_anti_aliasing::NK_ANTI_ALIASING_OFF;

        nk_buffer vbuf, ebuf;
        nk_buffer_init_fixed(&vbuf, gNkDevice.verts, sizeof(gNkDevice.verts));
        nk_buffer_init_fixed(&ebuf, gNkDevice.indices, sizeof(gNkDevice.indices));
        auto convert_result = nk_convert(&ctx, &gNkDevice.cmds, &vbuf, &ebuf, &config);
        assert(convert_result == NK_CONVERT_SUCCESS);
    }

    {	// Draw
        const struct nk_draw_command *cmd;
        const nk_draw_index *offset = NULL;
        nk_draw_foreach(cmd, &ctx, &gNkDevice.cmds)
        {
            if (!cmd->elem_count)
                continue;
            // rendering should happen here
            offset += cmd->elem_count;
        }
    }
	// Cleanup & finishing steps
    nk_clear(&ctx);
}

int main()
{
    Init();

    //while (true)
    {   // New frame
        nk_input_begin(&ctx);
        nk_input_end(&ctx);

        // GUI
		if (nk_begin(&ctx, "Test", nk_rect(350, 0, 220, 220), NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_CLOSABLE))
		{
			nk_layout_row_static(&ctx, 30, 80, 1);
			nk_label(&ctx, "Hello World!", NK_TEXT_ALIGN_LEFT); // this works
			nk_label_colored(&ctx, "Hello Color!", NK_TEXT_ALIGN_LEFT, nk_color{ 255,222,0,255 }); // this crashes in wasm
		}
		nk_end(&ctx);

        // Put everything on screen
        Render();
    }
    cout << "Success!" << endl;
}