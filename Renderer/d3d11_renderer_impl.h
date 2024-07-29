#pragma once


#include "frustum_culling.h"
// NOTE: This header file must *ONLY* be included by renderer.cpp

namespace
{
	template<typename T>
	void safe_release(T* t)
	{ 
		if (t)
			t->Release();
	}

	float rand_float(float min, float max)
	{
		float ratio = rand() / (float)RAND_MAX;
		return (max - min) * ratio + min;
	}
}

namespace end
{
	using namespace DirectX;

	struct Particle
	{
		float4 Position;
		float4 Prev_Position;
		float4 Color;
		//Velocity
		float4 ParticleVelocity;
	};

	struct Emitter
	{
		XMMATRIX EmitterWorld;
		float4 SpawnPos;
		float4 SpawnColor;
		//indices into the shared pool
		sorted_pool_t<int16_t, 256>indices;
	};

	struct aabbContainer
	{
		aabb_t aabb;
		XMMATRIX aabbWorld;
		colored_vertex aabbVerts[8];
		bool inFrustum;
	};


	struct AxisModel
	{
		//World Matrix 
		XMMATRIX AxisWorld = XMMatrixIdentity();
		//vertex buffer
		ID3D11Buffer* VertexBuffer = nullptr;
		//index buffer
		ID3D11Buffer* IndexBuffer = nullptr;
		//index count
		UINT IndexCount = 0;
		//vertex count
		UINT VertexCount = 0;

	};

	struct renderer_t::impl_t
	{
		// platform/api specific members, functions, etc.
		// Device, swapchain, resource views, states, etc. can be members here
		HWND hwnd;

		ID3D11Device *device = nullptr;
		ID3D11DeviceContext *context = nullptr;
		IDXGISwapChain *swapchain = nullptr;

		ID3D11RenderTargetView*		render_target[VIEW_RENDER_TARGET::COUNT]{};

		ID3D11DepthStencilView*		depthStencilView[VIEW_DEPTH_STENCIL::COUNT]{};

		ID3D11DepthStencilState*	depthStencilState[STATE_DEPTH_STENCIL::COUNT]{};

		ID3D11RasterizerState*		rasterState[STATE_RASTERIZER::COUNT]{};

		ID3D11Buffer*				vertex_buffer[VERTEX_BUFFER::COUNT]{};

		ID3D11Buffer*				index_buffer[INDEX_BUFFER::COUNT]{};
		
		ID3D11InputLayout*			input_layout[INPUT_LAYOUT::COUNT]{};

		ID3D11VertexShader*			vertex_shader[VERTEX_SHADER::COUNT]{};

		ID3D11PixelShader*			pixel_shader[PIXEL_SHADER::COUNT]{};

		ID3D11Buffer*				constant_buffer[CONSTANT_BUFFER::COUNT]{};

		D3D11_VIEWPORT				view_port[VIEWPORT::COUNT]{};

		/* Add more as needed...
		ID3D11SamplerState*			sampler_state[STATE_SAMPLER::COUNT]{};

		ID3D11BlendState*			blend_state[STATE_BLEND::COUNT]{};
		*/

#pragma region LineData

		//line vertex shader
		ID3D11VertexShader* LineVS;
		//line pixel shader
		ID3D11PixelShader* LinePS = nullptr;
		ID3D11Buffer* LineVertexBuffer = nullptr;
		ID3D11Buffer* LineIndexBuffer = nullptr;
		ID3D11InputLayout* GridInputLayout = nullptr;

		//matrix
		int Width;
		int Height;
	
		//Matrices for camera movement
		XMMATRIX T;
		XMMATRIX Camera;

		//basic Matrices 
		XMMATRIX View;
		XMMATRIX World;
		XMMATRIX Projection;

		XMMATRIX scale = XMMatrixIdentity();
		XMMATRIX Translation = XMMatrixIdentity();

		UINT Stride = sizeof(colored_vertex);
		UINT Offset = 0;
		//BLOB* layoutblob = nullptr;
		binary_blob_t vs_Gridblob;

		int LineVertCount = 0;

		float GridDelta;

		float StartXVal = 0.0f;
		float StartYVal = 0.0f;
		float StartZVal = 0.0f;

		float EndXVal = 0.0f;
		float EndYVal = 0.0f;
		float EndZVal = 0.0f;

		int ColorTimer = 0;
		bool ColorUp = true;
		bool ColorDown = false;

		float StartColor = 0.1f;
		float EndColor = 1.0f;

		float Colorx = 0.5f;
		float Colory = 0.5f;
		float Colorz = 0.5f;
#pragma endregion

#pragma region MatrixFunctionVariables

		bool ActivateLookAt = false;
		bool ActivateTurnTo = false;
		bool ActivateMouseLook = false;

#pragma endregion

#pragma region ParticleData
	public:
		//particle and pool information
		end::sorted_pool_t <Particle, 2000> SortedPool;

		//free pool
		end::pool_t<Particle, 4000>FreePool;
		//first emitter
		end::Emitter ParticleEmitter;


		colored_vertex* ParticleArray = nullptr;
		int ParticleCount = 0;
		float Velocity = 1.0f;
		float GravityConst = 0.98f;

		float4 TrailLength = { 0.05f, 0.05f, 0.05f, 0.5f };
		int ParticleArrayIndex = 0;
		int result;
		unsigned int FrameSelected = 0;
		double Time;
		bool RenderParticles = false;

		ID3D11Buffer* ParticleVertexBuffer = nullptr;
		ID3D11PixelShader* ParticlePixelShader = nullptr;
		ID3D11VertexShader* ParticleVertexShader = nullptr;
#pragma endregion

#pragma region AxisModelVariables
public:  

		//AxisModel *Axis = new AxisModel;
		XMMATRIX Movement;
		XMMATRIX UserAxis;
		XMMATRIX LAAxis;
		XMMATRIX TTAxis;
		XMMATRIX MLAxis;


		bool KeyBuffer = false;

#pragma endregion

#pragma region MouseLookAt

		

#pragma endregion


		// Constructor for renderer implementation
		// 
		impl_t(native_handle_type window_handle, view_t& default_view);

#pragma region Timer

		XTime Timer;
#pragma endregion

		float Rand_FLOAT(float min, float max);

#pragma region RenderFunctions
		void draw_view(view_t& view);

		void draw_lines(view_t& view);
#pragma endregion

#pragma region CleanUp
		~impl_t()
		{
			// TODO:
			//Clean-up
			//
			// In general, release objects in reverse order of creation

			for (auto& ptr : constant_buffer)
				safe_release(ptr);

			for (auto& ptr : pixel_shader)
				safe_release(ptr);

			for (auto& ptr : vertex_shader)
				safe_release(ptr);

			for (auto& ptr : input_layout)
				safe_release(ptr);

			for (auto& ptr : index_buffer)
				safe_release(ptr);

			for (auto& ptr : vertex_buffer)
				safe_release(ptr);

			for (auto& ptr : rasterState)
				safe_release(ptr);

			for (auto& ptr : depthStencilState)
				safe_release(ptr);

			for (auto& ptr : depthStencilView)
				safe_release(ptr);

			for (auto& ptr : render_target)
				safe_release(ptr);


			//release Grid/Line stuff
			safe_release(LinePS);
			safe_release(LineVS);
			safe_release(LineVertexBuffer);
			safe_release(LineIndexBuffer);
			safe_release(GridInputLayout);
			//safe_release(vs_Gridblob);

			//release grid stuff
			safe_release(ParticlePixelShader);
			safe_release(ParticleVertexShader);
			safe_release(ParticleVertexBuffer);


			safe_release(context);
			safe_release(swapchain);
			safe_release(device);
		}
#pragma endregion
	
#pragma region CreateBasicDirectXFeatures

		void create_device_and_swapchain();

		void create_Matrices();

		void create_main_render_target();

		void setup_depth_stencil();

		void setup_rasterizer();

		void Create_Vertex_Shader();

		void Create_Pixel_Shader();

		void create_constant_buffers();

		HRESULT Create_Index_Buffer();

		HRESULT Create_Vertex_Buffer();

		HRESULT CreateInputLayout();

#pragma endregion


#pragma region CreateAxisModel

		void CreateAxisModel();
		
		void AddAxisLines(XMMATRIX M, float4 Color, float4 ColorY, float4 ColorZ);
		HRESULT CreateAxisVertexBuffer(AxisModel* M);
		//HRESULT CreateAxisIndexBuffer(AxisModel* M);
		float4 ColorX = { 1.0f, 0.0f, 0.0f, 0.0f };
		float4 ColorY = { 0.0f, 1.0f, 0.0f, 0.0f };
		float4 ColorZ = { 0.0f, 0.0f, 1.0f, 0.0f };

#pragma endregion


#pragma region ExtraFunctions
		void Update(view_t & view, float AccumulatedX, float AccumulatedY,bool MouseLook);
		XMFLOAT4 XM_Normalize( XMFLOAT4 v);
		float4 Normalize(float4 v);
		void Input(view_t & view);
		void Generate_Points_Array();
		void Particle_Add_Line(float3 point_a, float3 point_b, float4 color_a, float4 color_b);
		HRESULT CreateParticalVertexBuffer();
		void allocateParticles(int size);
		void updateParticles();

		void InitializeSharedPool();

		void InitializeEmitters();

		void LookAtFunction(XMMATRIX M, XMFLOAT4 TargetPos, XMFLOAT4 LocalUp);

		void TurnToFunction(XMMATRIX M, XMVECTOR TargetPos, float SpeedScalar);

		void AddAABBLines(end::aabb_t & aabb_Container, float4 color);

		void CreateAABB(end::aabb_t & aabb_Container);

		XMMATRIX MouseLookFunction(XMMATRIX M, float dx, float dy);

		void UpdateFreeParticles();

		void AllocateFreeParticles(int size);

		void ClearParticleArray();

		HRESULT CreateFreeVertexBuffer();
		
#pragma endregion

	};
}