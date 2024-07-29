#pragma once
#include <memory>
#include "view.h"
#include "pools.h"


#define FSGD_END_USE_D3D

namespace end
{
	// APIs/Platforms have their own types for representing a handle to a 'window'
	// They will generally all fit in the 8-bytes of a void*.
	// HWND is actually just a typedef/alias for a void*.
	using native_handle_type = void*;



	//struct Particle
	//{
	//	float4 Position;
	//	float4 Prev_Position;
	//	float4 Color;

	//	/*float3 Position;
	//	float3 Prev_Position;
	//	float4 Color;*/
	//};


	//bool MForward = false;
	//bool MBack = false;
	//bool MLeft = false;
	//bool MRight = false;





	// Interface to the renderer
	class renderer_t
	{
	public:

		

		float StartXVal = 0.0f;
		float StartYVal = 0.0f;
		float StartZVal = 0.0f;

		float EndXVal = 0.0f;
		float EndYVal = 0.0f;
		float EndZVal = 0.0f;

		int ColorTimer = 0;
		bool ColorUp = true;
		bool ColorDown = false;

		float Colorx = 0.1f;
		float Colory = 0.1f;
		float Colorz = 0.1f;



		renderer_t(native_handle_type window_handle);
		//renderer_t(renderer_t&& other);

		~renderer_t();

		void draw(float AccumulatedX, float AccumulatedY, bool MouseLk);

		void Update(end::view_t default_view);

		void Input(end::view_t default_view);

		view_t default_view;

	private:

		// PImpl idiom ("Pointer to implementation")

		// 'impl' will define the implementation for the renderer elsewhere
		struct impl_t;

		// Pointer to the implementation
		impl_t* p_impl = nullptr;
		
	};

	// The following types just defines scopes for enum values.
	// The enum values can be used as indices in arrays.
	// These enum values can be added to as needed.

	struct VIEWPORT {
		enum { DEFAULT = 0, COUNT };
	};

	struct CONSTANT_BUFFER {
		enum { MVP = 0, COUNT };
	};

	struct VERTEX_SHADER {
		enum { BUFFERLESS_CUBE = 0, COLORED_VERTEX, COUNT };
	};

	struct PIXEL_SHADER {
		enum { BUFFERLESS_CUBE = 0, COLORED_VERTEX = 0, COUNT };
	};

	struct VIEW_RENDER_TARGET {
		enum { DEFAULT = 0, COUNT };
	};

	struct INDEX_BUFFER {
		enum { DEFAULT = 0, COUNT };
	};

	struct INPUT_LAYOUT {
		enum { COLORED_VERTEX = 0, COUNT };
	};

	struct STATE_RASTERIZER {
		enum { DEFAULT = 0, COUNT };
	};

	struct VIEW_DEPTH_STENCIL {
		enum { DEFAULT = 0, COUNT };
	};

	struct STATE_DEPTH_STENCIL {
		enum { DEFAULT = 0, COUNT };
	};

	struct VERTEX_BUFFER {
		enum { COLORED_VERTEX = 0, COUNT };
	};

	/* Add more as needed...
	enum STATE_SAMPLER{ DEFAULT = 0, COUNT };

	enum STATE_BLEND{ DEFAULT = 0, COUNT };
	*/
}