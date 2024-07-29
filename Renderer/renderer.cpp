#include "renderer.h"

#ifdef FSGD_END_USE_D3D
#include "d3d11_renderer_impl.h"
#endif

namespace end
{
	renderer_t::renderer_t(native_handle_type window_handle)
	{
		p_impl = new impl_t(window_handle, default_view);
	}

	/*
	renderer_t::renderer_t(renderer_t&& other)
	{
		std::swap(p_impl, other.p_impl);
	}
	*/
	renderer_t::~renderer_t()
	{
		// Clean up implementation
		delete p_impl;
	}

	void renderer_t::Update(end::view_t default_view)
	{
	}

	void renderer_t::Input(end::view_t default_view)
	{
	}

	void renderer_t::draw(float AccumulatedX, float AccumulatedY, bool MouseLk)
	{
		// draw views...
		//p_impl->draw_view(default_view);
		//Update(default_view);
		
		p_impl->Update(default_view, AccumulatedX, AccumulatedY, MouseLk);
		p_impl->draw_lines(default_view);
		// draw views...
		// draw views...
	}

}