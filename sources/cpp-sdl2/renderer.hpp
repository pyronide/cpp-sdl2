#pragma once

#include <SDL.h>
#include <SDL_render.h>
#include <utility>
#include <vector>

#include "color.hpp"
#include "exception.hpp"
#include "rect.hpp"
#include "surface.hpp"
#include "texture.hpp"

namespace sdl
{
///Class that represent a SDL2 2D renderer
class Renderer
{
public:
	///Construct a renderer from the SDL_Renderer C object
	explicit Renderer(SDL_Renderer* renderer) : renderer_{renderer} {}
	Renderer(Renderer&& other) = default;

	///Move a renderer to this one
	Renderer& operator=(Renderer&& other)
	{
		SDL_DestroyRenderer(renderer_);
		renderer_		= other.renderer_;
		other.renderer_ = nullptr;
		return *this;
	}

	///Destroy the renderer automaticlally when object goes out of scope
	~Renderer() { SDL_DestroyRenderer(renderer_); }

	///This is a managed RAII resource. this object is not copyable
	Renderer(Renderer const&) = delete;

	///This is a managed RAII resource. this object is not copyable
	Renderer& operator=(Renderer const&) = delete;

	///Return a pointer to the wrapped C SDL_Renderer
	SDL_Renderer* ptr() { return renderer_; }

	////////////////
	// Attributes //
	////////////////

	///Populate renderinfo structure
	void get_info(SDL_RendererInfo& info)
	{
		if (SDL_GetRendererInfo(renderer_, &info) != 0)
			throw Exception{"SDL_GetRendererInfo"};
	}

	///Get render infos
	SDL_RendererInfo info()
	{
		SDL_RendererInfo info;
		get_info(info);
		return info;
	}

	// Get the current draw color
	Color drawcolor()
	{
		Color c;
		if (SDL_GetRenderDrawColor(renderer_, &c.r, &c.g, &c.b, &c.a) != 0)
			throw Exception{"SDL_GetRenderDrawColor"};
		return c;
	}

	///Set the drawcolor form colors values as bytes
	void set_drawcolor(Uint8 r, Uint8 g, Uint8 b, Uint8 a = SDL_ALPHA_OPAQUE)
	{
		if (SDL_SetRenderDrawColor(renderer_, r, g, b, a) != 0)
			throw Exception{"SDL_SetRenderDrawColor"};
	}

	///Set the drawcolor from color struct
	void set_drawcolor(Color const& c) { set_drawcolor(c.r, c.g, c.b, c.a); }

	///Get the clipping rectangle
	Rect cliprect()
	{
		Rect r;
		SDL_RenderGetClipRect(renderer_, &r);
		return r;
	}

	///Set the clipping rectangle
	void set_cliprect(Rect const& r)
	{
		if (SDL_RenderSetClipRect(renderer_, &r) != 0)
			throw Exception{"SDL_RenderSetClipRect"};
	}

	///Renturn true if clipping is enable
	bool clip_enabled() { return SDL_RenderIsClipEnabled(renderer_); }

	///Disable the clipping rectangle
	void disable_clip()
	{
		if (SDL_RenderSetClipRect(renderer_, nullptr) != 0)
			throw Exception{"SDL_RenderSetClipRect"};
	}

	///Get the current integer scale
	bool intscale() { return SDL_RenderGetIntegerScale(renderer_); }

	///Set the interger scale
	void set_intscale(bool intscale)
	{
		if (SDL_RenderSetIntegerScale(renderer_, SDL_bool(intscale)) != 0)
			throw Exception{"SDL_RenderSetIntegerScale"};
	}

	////////////////////
	// TEXTURE MAKING //
	////////////////////

	///Make a new texture
	Texture make_texture(
		Uint32 format, SDL_TextureAccess access, int w, int h) const
	{
		return Texture{renderer_, format, access, w, h};
	}

	///Make a new texture
	Texture make_texture(
		Uint32 format, SDL_TextureAccess access, Vec2i size) const
	{
		return Texture{renderer_, format, access, size.x, size.y};
	}

	///Make a texture from a surface
	Texture make_texture(Surface const& surface) const
	{
		return Texture{renderer_, surface};
	}

	///Make a texture from a file
	///\param filename file path
	Texture make_texture(std::string const& filename) const
	{
		return Texture{renderer_, filename};
	}

	/////////////
	// DRAWING //
	/////////////

	///Present renderer
	Renderer& present()
	{
		SDL_RenderPresent(renderer_);
		return *this;
	}

	///Clear renderer
	void clear()
	{
		if (SDL_RenderClear(renderer_) != 0) throw Exception{"SDL_RenderClear"};
	}

	///Clear with specified color
	void clear(Color const& c)
	{
		set_drawcolor(c);
		clear();
	}

	///Draw line between two points
	void draw_line(Vec2i const& pos1, Vec2i const& pos2)
	{
		if (SDL_RenderDrawLine(renderer_, pos1.x, pos1.y, pos2.x, pos2.y) != 0)
			throw Exception{"SDL_RenderDrawLine"};
	}

	///Draw line between two points with specified color
	void draw_line(Vec2i const& pos1, Vec2i const& pos2, Color const& c)
	{
		set_drawcolor(c);
		draw_line(pos1, pos2);
	}

	///Draw array of lines
	void draw_lines(std::vector<Vec2i> const& points)
	{
		if (SDL_RenderDrawLines(renderer_, &points[0], (int)points.size()) != 0)
			throw Exception{"SDL_RenderDrawLines"};
	}

	///Draw array of lines with specified color
	void draw_lines(std::vector<Vec2i> const& points, Color const& c)
	{
		set_drawcolor(c);
		draw_lines(points);
	}

	///Draw point
	void draw_point(Vec2i const& point)
	{
		if (SDL_RenderDrawPoint(renderer_, point.x, point.y) != 0)
			throw Exception{"SDL_RenderDrawPoint"};
	}

	///Draw point with specified coolor
	void draw_point(Vec2i const& point, Color const& c)
	{
		set_drawcolor(c);
		draw_point(point);
		;
	}

	///Draw array of points
	void draw_points(std::vector<Vec2i> const& points)
	{
		if (SDL_RenderDrawPoints(renderer_, &points[0], (int)points.size())
			!= 0)
			throw Exception{"SDL_RenderDrawPoints"};
	}

	///Draw array of points with specified color
	void draw_points(std::vector<Vec2i> const& points, Color const& c)
	{
		set_drawcolor(c);
		draw_points(points);
	}

	void draw_ray(Vec2i const& orig, Vec2i const& ray)
	{
		draw_line(orig, orig + ray);
	}
	void draw_ray(Vec2i const& orig, Vec2i const& ray, Color const& c)
	{
		draw_line(orig, orig + ray, c);
	}

	///Draw rectangle
	void draw_rect(Rect const& rect)
	{
		if (SDL_RenderDrawRect(renderer_, &rect) != 0)
			throw Exception{"SDL_RenderDrawRect"};
	}

	///Draw rectangle with specified color
	void draw_rect(Rect const& rect, Color const& c)
	{
		set_drawcolor(c);
		draw_rect(rect);
	}

	///Draw array of rectangles
	void draw_rects(std::vector<Rect> const& rects)
	{
		if (SDL_RenderDrawRects(renderer_, &rects[0], (int)rects.size()) != 0)
			throw Exception{"SDL_RenderDrawRects"};
	}
	///Draw array of rectangles with specified colors
	void draw_rects(std::vector<Rect> const& rects, const Color& c)
	{
		set_drawcolor(c);
		draw_rects(rects);
	}

	///Fill rectangle
	void fill_rect(Rect const& rect)
	{
		if (SDL_RenderFillRect(renderer_, &rect) != 0)
			throw Exception{"SDL_RenderFillRect"};
	}
	///Fill rectangle with specified color
	void fill_rect(Rect const& rect, Color const& c)
	{
		set_drawcolor(c);
		fill_rect(rect);
	}

	///Fill array of rectangles
	void fill_rects(std::vector<Rect> const& rects)
	{
		if (SDL_RenderFillRects(renderer_, &rects[0], (int)rects.size()) != 0)
			throw Exception{"SDL_RenderDrawRects"};
	}
	///Fill array of rectangles with specified colors
	void fill_rects(std::vector<Rect> const& rects, Color const& c)
	{
		set_drawcolor(c);
		fill_rects(rects);
	}

private:
	///Pointer to raw SDL_Renderer
	SDL_Renderer* renderer_;
};

} // namespace sdl