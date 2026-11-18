#pragma once

#include <algorithm>
#include <atomic>
#include <cstdint>
#include <utility>

// Helpers for resizing textures when the viewport changes.
// Callers must hold the lock that protects textures and viewport size.
namespace surface_resize
{
// Scale to the viewport, rounding down, with at least one pixel per side.
// Game sizes are relative to the game's 640x480 frame.
inline std::pair<uint32_t, uint32_t> UpscaledExtent(uint32_t guestWidth, uint32_t guestHeight,
	uint32_t viewportWidth, uint32_t viewportHeight, float scale)
{
	const float width = (float(guestWidth) / 640.0f) * viewportWidth * scale;
	const float height = (float(guestHeight) / 480.0f) * viewportHeight * scale;
	return { std::max(uint32_t(width), 1u), std::max(uint32_t(height), 1u) };
}

// Update the viewport size and request a resize when it changes.
// Keep any resize request already made by a settings change.
inline void SetViewportSize(uint32_t& width, uint32_t& height, uint32_t newWidth,
	uint32_t newHeight, std::atomic<bool>& pending)
{
	if (width != newWidth || height != newHeight)
	{
		width = newWidth;
		height = newHeight;
		pending = true;
	}
}

// Resize textures and surfaces without changing their color/depth type.
// Update surfaces that share a recreated texture.
//
// Wait for GPU work to finish and hold the resize lock before calling.
// recreate must update the texture, generation, size and layout.
template<typename Textures, typename Surfaces, typename Format, typename Recreate>
void RecreateTracked(const Textures& textures, const Surfaces& surfaces, Format depthFormat,
	const Recreate& recreate)
{
	for (auto* texture : textures)
	{
		texture->framebuffer.reset();
		for (auto* alias : texture->surfaceRTs)
		{
			alias->framebuffers.clear();
			alias->texture = nullptr;
		}

		recreate(texture, texture->isRenderTarget);

		for (auto* alias : texture->surfaceRTs)
		{
			alias->texture = texture->texture;
			alias->textureGeneration = texture->textureGeneration;
			alias->width = texture->width;
			alias->height = texture->height;
			alias->layout = texture->layout;
		}
	}

	for (auto* surface : surfaces)
	{
		surface->framebuffers.clear();
		recreate(surface, surface->format != depthFormat);
	}
}
}
