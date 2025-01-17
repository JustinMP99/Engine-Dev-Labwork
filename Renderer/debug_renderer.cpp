#include "debug_renderer.h"
#include <array>

// Anonymous namespace
namespace
{
	// Declarations in an anonymous namespace are global BUT only have internal linkage.
	// In other words, these variables are global but are only visible in this source file.

	// Maximum number of debug lines at one time (i.e: Capacity)
	//constexpr size_t MAX_LINE_VERTS = 4096; 
	constexpr size_t MAX_LINE_VERTS = 9000;


	// CPU-side buffer of debug-line verts
	// Copied to the GPU and reset every frame.
	size_t line_vert_count = 0;
	std::array< end::colored_vertex, MAX_LINE_VERTS> line_verts;
	int LineIndexer[MAX_LINE_VERTS];
	int Index = 0;
}

namespace end
{
	namespace debug_renderer
	{

		int Line_Verts_Size;

		void add_line(float3 point_a, float3 point_b, float4 color_a, float4 color_b)
		{
			// Add points to debug_verts, increments debug_vert_count
			if (line_vert_count >= MAX_LINE_VERTS)
				return;
			
			//create first point
			colored_vertex Vert1;
			Vert1.color = color_a;
			Vert1.pos.xyz = point_a;

			//set first point
			line_verts[line_vert_count] = Vert1;
			//increment
			line_vert_count++;
			//Index++;

			//create second point
			colored_vertex Vert2;
			Vert2.color = color_b;
			Vert2.pos.xyz = point_b;

			//set second point
			line_verts[line_vert_count] = Vert2;
			//LineIndexer[Index] = Index;

			//increment
			line_vert_count++;
			//Index++;
		}

		void clear_lines()
		{
			// Resets debug_vert_count
			line_vert_count = 0;
		}

		const colored_vertex* get_line_verts()
		{ 
			// Does just what it says in the name
			return line_verts.data();
		}

		size_t get_line_vert_count() 
		{ 
			// Does just what it says in the name
			return line_vert_count;
		}

		size_t get_line_vert_capacity()
		{
			// Does just what it says in the name
			return MAX_LINE_VERTS;
		}
	}
}