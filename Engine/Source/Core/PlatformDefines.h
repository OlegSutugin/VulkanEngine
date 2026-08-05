#pragma once

#define RENDER_API_VULKAN

#if defined(RENDER_API_VULKAN)
#define CLIP_SPACE_Y_FLIP(matrix) ((matrix).m[1][1] *= -1)
#elif defined(RENDER_API_OPENGL)
#define CLIP_SPACE_Y_FLIP(matrix)
#elif defined(RENDER_API_DIRECTX)
#define CLIP_SPACE_Y_FLIP(matrix)
#else
#define CLIP_SPACE_Y_FLIP(matrix)
#endif