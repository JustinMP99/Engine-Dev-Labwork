#include "d3d11_renderer_impl.h"

#define NUMPARTICLES 15

end::frustum_t frustum;
end::aabbContainer aabbArray[10];
//end::aabbContainer *aabb_Container;

#pragma region Initialization

end::renderer_t::impl_t::impl_t(native_handle_type window_handle, view_t & default_view)
{
	srand(time(0));

	//frustum = new frustum_t;

	hwnd = (HWND)window_handle;

	create_device_and_swapchain();

	create_Matrices();

	create_main_render_target();

	setup_depth_stencil();

	setup_rasterizer();

	Create_Vertex_Shader();

	CreateInputLayout();

	Create_Pixel_Shader();

	create_constant_buffers();

#pragma region GridCreation
	//create line buffer
	Create_Vertex_Buffer();

#pragma endregion

#pragma region AxisCreation
	CreateAxisModel();

#pragma endregion

#pragma region Particles
	//Particle allocation
	//allocateParticles(NUMPARTICLES);


	InitializeEmitters();
	//AllocateFreeParticles(NUMPARTICLES);
	//InitializeSharedPool();

#pragma endregion

#pragma region MatrixStuff

	//set matrices to identity
	default_view.World = DirectX::XMMatrixIdentity();
	default_view.View = DirectX::XMMatrixIdentity();
	default_view.Projection = DirectX::XMMatrixIdentity();
	Movement = XMMatrixIdentity();

	//create View matrix vectors
	XMVECTOR eye = XMVectorSet(0.0f, 10.0f, -15.0f, 1.0f);
	XMVECTOR at = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	//set view matrix
	default_view.View = XMMatrixLookAtLH(eye, at, up);

	//Set Camera
	default_view.Camera = XMMatrixInverse(nullptr, XMMatrixLookAtLH(eye, at, up));

	//create perspective matrix
	default_view.Projection = XMMatrixPerspectiveFovLH(XM_PIDIV4, (float)Width / (float)Height, 0.01f, 100.0f);

#pragma endregion


	//calculate aabb's
	for (int i = 0; i < ARRAYSIZE(aabbArray); ++i)
	{
		CreateAABB(aabbArray[i].aabb);
	}

}
#pragma endregion

#pragma region DirectX_SetUp

void end::renderer_t::impl_t::create_device_and_swapchain()
{
	RECT crect;
	GetClientRect(hwnd, &crect);

	// Setup the viewport
	D3D11_VIEWPORT &vp = view_port[VIEWPORT::DEFAULT];

	Width = (float)crect.right;
	Height = (float)crect.bottom;

	vp.Width = (float)crect.right;
	vp.Height = (float)crect.bottom;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;

	// Setup swapchain
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 1;
	sd.BufferDesc.Width = crect.right;
	sd.BufferDesc.Height = crect.bottom;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hwnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;

	D3D_FEATURE_LEVEL  FeatureLevelsSupported;

	const D3D_FEATURE_LEVEL lvl[] =
	{
		D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3, D3D_FEATURE_LEVEL_9_2, D3D_FEATURE_LEVEL_9_1
	};

	UINT createDeviceFlags = 0;

#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	HRESULT hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, lvl, _countof(lvl), D3D11_SDK_VERSION, &sd, &swapchain, &device, &FeatureLevelsSupported, &context);
	if (hr == E_INVALIDARG)
	{
		hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, &lvl[1], _countof(lvl) - 1, D3D11_SDK_VERSION, &sd, &swapchain, &device, &FeatureLevelsSupported, &context);
	}
	assert(!FAILED(hr));
}

void end::renderer_t::impl_t::create_Matrices()
{
}

void end::renderer_t::impl_t::create_main_render_target()
{
	ID3D11Texture2D* pBackBuffer;
	// Get a pointer to the back buffer
	HRESULT hr = swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D),
		(LPVOID*)&pBackBuffer);

	assert(!FAILED(hr));

	// Create a render-target view
	device->CreateRenderTargetView(pBackBuffer, NULL,
		&render_target[VIEW_RENDER_TARGET::DEFAULT]);

	pBackBuffer->Release();
}

void end::renderer_t::impl_t::setup_depth_stencil()
{
	/* DEPTH_BUFFER */
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	ID3D11Texture2D *depthStencilBuffer;

	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

	depthBufferDesc.Width = (UINT)view_port[VIEWPORT::DEFAULT].Width;
	depthBufferDesc.Height = (UINT)view_port[VIEWPORT::DEFAULT].Height;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	HRESULT hr = device->CreateTexture2D(&depthBufferDesc, NULL, &depthStencilBuffer);

	assert(!FAILED(hr));

	/* DEPTH_STENCIL */
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;

	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	hr = device->CreateDepthStencilView(depthStencilBuffer, &depthStencilViewDesc, &depthStencilView[VIEW_DEPTH_STENCIL::DEFAULT]);

	assert(!FAILED(hr));

	depthStencilBuffer->Release();

	/* DEPTH_STENCIL_DESC */
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;

	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

	hr = device->CreateDepthStencilState(&depthStencilDesc, &depthStencilState[STATE_DEPTH_STENCIL::DEFAULT]);

	assert(!FAILED(hr));
}

void end::renderer_t::impl_t::setup_rasterizer()
{
	D3D11_RASTERIZER_DESC rasterDesc;

	ZeroMemory(&rasterDesc, sizeof(rasterDesc));

	rasterDesc.AntialiasedLineEnable = true;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = false;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	HRESULT hr = device->CreateRasterizerState(&rasterDesc, &rasterState[STATE_RASTERIZER::DEFAULT]);

	assert(!FAILED(hr));
}

void end::renderer_t::impl_t::Create_Vertex_Shader()
{
	binary_blob_t vs_blob = load_binary_blob("vs_cube.cso");
	vs_Gridblob = load_binary_blob("VS_Grid.cso");

	HRESULT hr = device->CreateVertexShader(vs_blob.data(), vs_blob.size(), NULL, &vertex_shader[VERTEX_SHADER::BUFFERLESS_CUBE]);
	assert(!FAILED(hr));
	//create line Vertex Shader
	hr = device->CreateVertexShader(vs_Gridblob.data(), vs_Gridblob.size(), NULL, &LineVS);
	//assert(!FAILED(hr));
	if (FAILED(hr))
	{
		MessageBox(hwnd, L"Error creating grid Vertex Shader", L"Error", MB_OK);
	}
}

void end::renderer_t::impl_t::Create_Pixel_Shader()
{	
	binary_blob_t ps_Gridblob = load_binary_blob("PS_Grid.cso");
	binary_blob_t ps_blob = load_binary_blob("ps_cube.cso");

	HRESULT hr = device->CreatePixelShader(ps_blob.data(), ps_blob.size(), NULL, &pixel_shader[PIXEL_SHADER::BUFFERLESS_CUBE]);
	assert(!FAILED(hr));
	//create line Pixel Shader
	hr = device->CreatePixelShader(ps_Gridblob.data(), ps_Gridblob.size(), NULL, &LinePS);
	//assert(!FAILED(hr));
	if (FAILED(hr))
	{
		MessageBox(hwnd, L"Error creating grid Pixel Shader", L"Error", MB_OK);
	}
}

void end::renderer_t::impl_t::create_constant_buffers()
{
	D3D11_BUFFER_DESC mvp_bd;
	ZeroMemory(&mvp_bd, sizeof(mvp_bd));

	mvp_bd.Usage = D3D11_USAGE_DEFAULT;
	mvp_bd.ByteWidth = sizeof(MVP_t);
	mvp_bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	mvp_bd.CPUAccessFlags = 0;

	HRESULT hr = device->CreateBuffer(&mvp_bd, NULL, &constant_buffer[CONSTANT_BUFFER::MVP]);
}

HRESULT end::renderer_t::impl_t::Create_Index_Buffer()
{
	//HRESULT hr;
	////line index buffer
	//D3D11_BUFFER_DESC bd = {};
	//bd.Usage = D3D11_USAGE_DEFAULT;
	//bd.ByteWidth = sizeof(colored_vertex) * end::debug_renderer::get_line_vert_capacity();
	//bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	//bd.CPUAccessFlags = 0;

	//D3D11_SUBRESOURCE_DATA InitData = {};
	//InitData.pSysMem = &LineIndexer;
	//hr = device->CreateBuffer(&bd, &InitData, &LineIndexBuffer);
	//if (FAILED(hr))
	//{
	//	return hr;
	//}

	return S_OK;
}

HRESULT end::renderer_t::impl_t::Create_Vertex_Buffer()
{
	HRESULT hr;

	//line vertex buffer
	D3D11_BUFFER_DESC vb = {};
	vb.Usage = D3D11_USAGE_DEFAULT;
	vb.ByteWidth = sizeof(colored_vertex) * end::debug_renderer::get_line_vert_capacity();
	vb.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vb.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData = {};
	InitData.pSysMem = end::debug_renderer::get_line_verts();
	hr = device->CreateBuffer(&vb, &InitData, &LineVertexBuffer);
	if (FAILED(hr))
	{
		return hr;
	}
	return S_OK;
}

HRESULT end::renderer_t::impl_t::CreateInputLayout()
{
	HRESULT hr;
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT numElements = ARRAYSIZE(layout);

	//create layout
	hr = device->CreateInputLayout(layout, numElements, vs_Gridblob.data(), vs_Gridblob.size(),  &GridInputLayout);
	if (FAILED(hr))
	{
		MessageBox(hwnd, L"Error creating grid input layout!", L"Error", MB_OK);
		return hr;
	}
	context->IASetInputLayout(GridInputLayout);

	return S_OK;
}

#pragma endregion

#pragma region RenderFunctions

float end::renderer_t::impl_t::Rand_FLOAT(float min, float max)
{
	float ratio = rand() / (float)RAND_MAX;
	return  (max - min) * ratio + min;
}

#pragma region OtherRenderer


void end::renderer_t::impl_t::draw_view(view_t & view)
{
	/*const float4 black{ 0.0f, 0.0f, 0.0f, 1.0f };

	context->OMSetDepthStencilState(depthStencilState[STATE_DEPTH_STENCIL::DEFAULT], 1);
	context->OMSetRenderTargets(1, &render_target[VIEW_RENDER_TARGET::DEFAULT], depthStencilView[VIEW_DEPTH_STENCIL::DEFAULT]);

	context->ClearRenderTargetView(render_target[VIEW_RENDER_TARGET::DEFAULT], black.data());
	context->ClearDepthStencilView(depthStencilView[VIEW_DEPTH_STENCIL::DEFAULT], D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	context->RSSetState(rasterState[STATE_RASTERIZER::DEFAULT]);
	context->RSSetViewports(1, &view_port[VIEWPORT::DEFAULT]);

	context->VSSetShader(vertex_shader[VERTEX_SHADER::BUFFERLESS_CUBE], nullptr, 0);
	context->PSSetShader(pixel_shader[PIXEL_SHADER::BUFFERLESS_CUBE], nullptr, 0);

	context->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	context->VSSetConstantBuffers(0, 1, &constant_buffer[CONSTANT_BUFFER::MVP]);

	MVP_t mvp;

	mvp.modeling = XMMatrixTranspose(XMMatrixIdentity());
	mvp.projection = XMMatrixTranspose((XMMATRIX&)view.proj_mat);
	mvp.view = XMMatrixTranspose(XMMatrixInverse(nullptr, (XMMATRIX&)view.view_mat));

	context->UpdateSubresource(constant_buffer[CONSTANT_BUFFER::MVP], 0, NULL, &mvp, 0, 0);

	context->Draw(36, 0);

	swapchain->Present(0, 0);*/
}


#pragma endregion

#pragma region Debug_Renderer

void end::renderer_t::impl_t::draw_lines(view_t & view)
{

#pragma region GridRendering
	const float4 black{ 0.0f, 0.0f, 0.0f, 1.0f };

	context->OMSetDepthStencilState(depthStencilState[STATE_DEPTH_STENCIL::DEFAULT], 1);
	context->OMSetRenderTargets(1, &render_target[VIEW_RENDER_TARGET::DEFAULT], depthStencilView[VIEW_DEPTH_STENCIL::DEFAULT]);

	context->ClearRenderTargetView(render_target[VIEW_RENDER_TARGET::DEFAULT], black.data());
	context->ClearDepthStencilView(depthStencilView[VIEW_DEPTH_STENCIL::DEFAULT], D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	context->RSSetState(rasterState[STATE_RASTERIZER::DEFAULT]);
	context->RSSetViewports(1, &view_port[VIEWPORT::DEFAULT]);

	//set grid shaders
	context->VSSetShader(LineVS, nullptr, 0);
	context->PSSetShader(LinePS, nullptr, 0);

	//set topology to line list
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	//set vertex Buffer
	context->IASetVertexBuffers(0, 1, &LineVertexBuffer, (const UINT*)&Stride, &Offset);

	//set Index Buffer
	//context->IASetIndexBuffer(LineIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	context->VSSetConstantBuffers(0, 1, &constant_buffer[CONSTANT_BUFFER::MVP]);

	MVP_t mvp;

	mvp.modeling = DirectX::XMMatrixTranspose(view.World);
	mvp.projection = DirectX::XMMatrixTranspose(view.Projection);
	mvp.view = DirectX::XMMatrixTranspose(view.View);

	context->UpdateSubresource(constant_buffer[CONSTANT_BUFFER::MVP], 0, NULL, &mvp, 0, 0);
	context->UpdateSubresource(LineVertexBuffer, 0, NULL, debug_renderer::get_line_verts(), 0, 0);
	context->Draw(debug_renderer::get_line_vert_count(), 0);
#pragma endregion

#pragma region AxisRendering

	//context->RSSetState(rasterState[STATE_RASTERIZER::DEFAULT]);
	//context->RSSetViewports(1, &view_port[VIEWPORT::DEFAULT]);

	////set grid shaders
	//context->VSSetShader(LineVS, nullptr, 0);
	//context->PSSetShader(LinePS, nullptr, 0);

	////set vertex Buffer
	//context->IASetVertexBuffers(0, 1, &Axis->VertexBuffer, (const UINT*)&Stride, &Offset);

	//context->VSSetConstantBuffers(0, 1, &constant_buffer[CONSTANT_BUFFER::MVP]);

	//mvp.modeling = DirectX::XMMatrixTranspose(Axis->AxisWorld);
	//mvp.projection = DirectX::XMMatrixTranspose(view.Projection);
	//mvp.view = DirectX::XMMatrixTranspose(view.View);

	//context->UpdateSubresource(constant_buffer[CONSTANT_BUFFER::MVP], 0, NULL, &mvp, 0, 0);
	//context->Draw(Axis->IndexCount, 0);

	//ClearParticleArray();
	debug_renderer::clear_lines();
#pragma endregion

	swapchain->Present(0, 0);
}

#pragma endregion

#pragma endregion

#pragma region SortedParticleManipulation
void end::renderer_t::impl_t::Particle_Add_Line(float3 CurrentPos, float3 Prev_Position, float4 color_a, float4 color_b)
{
	//create first point
	colored_vertex Vert1;
	Vert1.color = color_a;
	Vert1.pos.xyz = CurrentPos;

	//set first point
	ParticleArray[ParticleArrayIndex] = Vert1;
	//increment
	ParticleArrayIndex++;
	
	//create second point
	colored_vertex Vert2;
	Vert2.color = color_b;
	Vert2.pos.xyz = Prev_Position;

	//set second point
	ParticleArray[ParticleArrayIndex] = Vert2;

	//increment
	ParticleArrayIndex++;
}

void end::renderer_t::impl_t::allocateParticles(int size)
{
	for (int i = 0; i < size; i++)
	{
		//allocate new particle 
		 int index = SortedPool.alloc();
		 if (index == -1)
		 {
			 return;
		 }
		//set particles postions
		SortedPool[index].Position = { 0.0f, 1.0f,5.0f, 0.0f };
		SortedPool[index].Prev_Position = { 0.0f, 0.9f, 5.0f, 0.0f };
		SortedPool[index].Color = { 0.5f, 1.0f, 0.5f, 0.0f };
		//SortedPool[index].Color = { Colorx, Colory, Colorz, 0.0f };
		SortedPool[index].ParticleVelocity = { Rand_FLOAT(-1, 1), Rand_FLOAT(1, 4),  Rand_FLOAT(-1, 1), 1.0f};	
	}
}

DirectX::XMFLOAT4 end::renderer_t::impl_t::XM_Normalize(DirectX::XMFLOAT4 v)
{

	XMFLOAT4 result = v;
	float sqrtResult = sqrt(v.x * v.x + v.y * v.y + v.z * v.z);

	result.x /= sqrtResult;
	result.y /= sqrtResult;
	result.z /= sqrtResult;
	//Final = v;
	return result;
}

end::float4 end::renderer_t::impl_t::Normalize(end::float4 v)
{
	float4 result = v;
	float sqrtResult = sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
	result.x /= sqrtResult;
	result.y /= sqrtResult;
	result.z /= sqrtResult;

	return result;
}

HRESULT end::renderer_t::impl_t::CreateParticalVertexBuffer()
{
	HRESULT hr;

	//fillArray
	int s = 0;
	ParticleCount = 0;
	int Size = 2 * SortedPool.size();
	ParticleArray = new colored_vertex[Size];

	for (int i = 0; i < Size; i++)
	{
		//current position
		ParticleArray[i].pos = { SortedPool[s].Position.x, SortedPool[s].Position.y, SortedPool[s].Position.z };
		ParticleArray[i].color = { SortedPool[s].Color.x, SortedPool[s].Color.y, SortedPool[s].Color.z };

		//iterate Particle count
		ParticleCount++;

		//iterate i
		i++;

		//second position
		ParticleArray[i].pos = { SortedPool[s].Prev_Position.x, SortedPool[s].Prev_Position.y, SortedPool[s].Prev_Position.z };
		//ParticleArray[i].pos = { Prev_Pos.x, Prev_Pos.y, Prev_Pos.z, Prev_Pos.w };
		ParticleArray[i].color = { SortedPool[s].Color.x, SortedPool[s].Color.y, SortedPool[s].Color.z };

		//iterate Particle count
		ParticleCount++;

		//iterate S
		s++;
	}

	//line vertex buffer
	D3D11_BUFFER_DESC vb = {};
	vb.Usage = D3D11_USAGE_DEFAULT;
	vb.ByteWidth = sizeof(colored_vertex) * ParticleCount;
	vb.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vb.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData = {};
	InitData.pSysMem = &ParticleArray;
	hr = device->CreateBuffer(&vb, &InitData, &ParticleVertexBuffer);
	if (FAILED(hr))
	{
		return hr;
	}
	return S_OK;
}

void end::renderer_t::impl_t::updateParticles()
{
	//update particle positions
	for (int i = 0; i < SortedPool.size(); i++)
	{
		if (SortedPool[i].Position.y <= -1.0f)
		{
			SortedPool.free(i);
			--i;
			continue;
		}

		float4 NewVel = { Velocity, Velocity, Velocity, Velocity };
		NewVel = Normalize(NewVel);
		float4 Result;
		Result.xyz = SortedPool[i].Position.xyz - NewVel.xyz * TrailLength.xyz;
		SortedPool[i].Prev_Position.xyz = Result.xyz;

		float delta = (float)Timer.Delta();
		//set Current position
		SortedPool[i].Position.x += SortedPool[i].ParticleVelocity.x * delta;
		SortedPool[i].Position.y += SortedPool[i].ParticleVelocity.y * delta;
		SortedPool[i].Position.z += SortedPool[i].ParticleVelocity.z * delta;

		SortedPool[i].ParticleVelocity.y -= (GravityConst * delta);

	}
}
#pragma endregion

#pragma region FreeParticleManipulation
void end::renderer_t::impl_t::InitializeSharedPool()
{
	//allocate particles
	for (int i = 0; i < ParticleEmitter.indices.capacity(); i++)
	{
		FreePool.alloc();
	}
	//set particle postions
	for (int i = 0; i < ParticleEmitter.indices.capacity(); i++)
	{
		FreePool[i].Position = { 0.0f,0.0f,0.0f,0.0f };
	}
}

void end::renderer_t::impl_t::InitializeEmitters()
{
	//set Emitter postion
	ParticleEmitter.SpawnPos = {0.0f,0.0f,0.0f};
	//set Emitter Color
	ParticleEmitter.SpawnColor = { 1.0f, 1.0f, 1.0f, 1.0f };
	//set world
	ParticleEmitter.EmitterWorld = XMMatrixIdentity();
	scale = XMMatrixScaling(0.2f, 0.2f, 0.2f);
	Translation = XMMatrixTranslation(0.0f, 0.2f, 0.0f);
	ParticleEmitter.EmitterWorld = Translation * scale;
}

void end::renderer_t::impl_t::UpdateFreeParticles()
{
	//update particle positions
	for (int i = 0; i < ParticleEmitter.indices.size(); i++)
	{
		int Index = ParticleEmitter.indices[i];
		if (FreePool[Index].Position.y <= -1.0f)
		{
			FreePool.free(Index);
			ParticleEmitter.indices.free(i);
			--i;
			continue;
		}

		//set previous position
		FreePool[Index].Prev_Position.x = FreePool[Index].Position.x;
		FreePool[Index].Prev_Position.y = FreePool[Index].Position.y;
		FreePool[Index].Prev_Position.z = FreePool[Index].Position.z;

		float delta = (float)Timer.Delta();
		//set Current position
		FreePool[Index].Position.x += FreePool[Index].ParticleVelocity.x * delta;
		FreePool[Index].Position.y += FreePool[Index].ParticleVelocity.y * delta;
		FreePool[Index].Position.z += FreePool[Index].ParticleVelocity.z * delta;

		FreePool[Index].ParticleVelocity.y -= (GravityConst * delta);
	}
}

void end::renderer_t::impl_t::AllocateFreeParticles(int size)
{
	for (int i = 0; i < size; i++)
	{
		//allocate new particle in free pool
		int index = FreePool.alloc();
		if (index == -1)
		{
			return;
		}
		int index2 = ParticleEmitter.indices.alloc();
		if (index2 == -1)
		{
			return;
		}

		ParticleEmitter.indices[index2] = index;

		//set particles postions
		FreePool[index].Position = { 7.0f, 1.0f,0.0f, 0.0f };
		FreePool[index].Prev_Position = { 7.0f, 0.9f, 0.0f, 0.0f };
		FreePool[index].Color = { 0.5f, 1.0f, 0.5f, 0.0f };
		//SortedPool[index].Color = { Colorx, Colory, Colorz, 0.0f };
		FreePool[index].ParticleVelocity = { Rand_FLOAT(-1, 1), Rand_FLOAT(1, 4),  Rand_FLOAT(-1, 1), 1.0f };
	}
	//for (int i = 0; i < size; i++)
	//{
	//	//allocate new particle 
	//	int index = ParticleEmitter.indices.alloc();
	//	if (index == -1)
	//	{
	//		return;
	//	}
	//	//set particles postions
	//	FreePool[index].Position = { 0.0f, 1.0f,0.0f, 0.0f };
	//	FreePool[index].Prev_Position = { 0.0f, 0.9f, 0.0f, 0.0f };
	//	FreePool[index].Color = { 0.5f, 1.0f, 0.5f, 0.0f };
	//	//SortedPool[index].Color = { Colorx, Colory, Colorz, 0.0f };
	//	FreePool[index].ParticleVelocity = { Rand_FLOAT(-1, 1), Rand_FLOAT(1, 4),  Rand_FLOAT(-1, 1), 1.0f };
	//}
}

void end::renderer_t::impl_t::ClearParticleArray()
{
	for (int i = 0; i < ParticleCount; i++)
	{
		ParticleArray[i].pos = { 0.0f, 0.0f, 0.0f, 0.0f };
	}
}

HRESULT end::renderer_t::impl_t::CreateFreeVertexBuffer()
{
	ParticleArray = new colored_vertex[ParticleEmitter.indices.size()];
	for (int i = 0; i < ParticleEmitter.indices.size(); i++)
	{
		ParticleArray[i].pos = { FreePool[i].Position.x, FreePool[i].Position.y, FreePool[i].Position.z, 0.0f };
		ParticleArray[i].color = { FreePool[i].Color.x, FreePool[i].Color.y,  FreePool[i].Color.z,  FreePool[i].Color.w };
		ParticleCount++;
	}

	HRESULT hr;
	//line vertex buffer
	D3D11_BUFFER_DESC vb = {};
	vb.Usage = D3D11_USAGE_DEFAULT;
	vb.ByteWidth = sizeof(colored_vertex) * ParticleCount;
	vb.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vb.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData = {};
	InitData.pSysMem = &ParticleArray;
	hr = device->CreateBuffer(&vb, &InitData, &ParticleVertexBuffer);
	if (FAILED(hr))
	{
		return hr;
	}

	return S_OK;
}

#pragma endregion

#pragma region AxisCreation

void end::renderer_t::impl_t::CreateAxisModel()
{

	//Set original Matrix
	UserAxis.r[0] = { 1.0f, 0.0f, 0.0f, 0.0f };
	UserAxis.r[1] = { 0.0f, 1.0f, 0.0f, 0.0f };
	UserAxis.r[2] = { 0.0f, 0.0f, 1.0f, 0.0f };
	UserAxis.r[3] = { 1.0f, 1.0f, 1.0f, 0.0f };

	/*LAAxis.r[0] = { 1.0f, 0.0f, 0.0f, 0.0f };
	LAAxis.r[1] = { 0.0f, 1.0f, 0.0f, 0.0f };
	LAAxis.r[2] = { 0.0f, 0.0f, 1.0f, 0.0f };
	LAAxis.r[3] = { 5.0f, 5.0f, 5.0f, 0.0f };


	TTAxis.r[0] = { 1.0f, 0.0f, 0.0f, 0.0f };
	TTAxis.r[1] = { 0.0f, 1.0f, 0.0f, 0.0f };
	TTAxis.r[2] = { 0.0f, 0.0f, 1.0f, 0.0f };
	TTAxis.r[3] = { 10.0f, 10.0f, 5.0f, 0.0f };*/

} 

void end::renderer_t::impl_t::AddAxisLines(DirectX::XMMATRIX M, float4 ColorX, float4 ColorY, float4 ColorZ)
{
	//X Vector
	
	//set first point
	colored_vertex StartX;
	StartX.pos = { XMVectorGetX(M.r[3]), XMVectorGetY(M.r[3]), XMVectorGetZ(M.r[3]) };
	//set second point 
	colored_vertex EndX;
	EndX.pos = { XMVectorGetX(M.r[0]), XMVectorGetY(M.r[0]), XMVectorGetZ(M.r[0]), 0.0f };
	EndX.pos = Normalize(EndX.pos);
	EndX.pos = { EndX.pos.x * 2, EndX.pos.y * 2, EndX.pos.z * 2, EndX.pos.w * 2 };
	EndX.pos = { StartX.pos.x + EndX.pos.x, StartX.pos.y + EndX.pos.y, StartX.pos.z + EndX.pos.z, StartX.pos.w + EndX.pos.w };
	//add point to line
	end::debug_renderer::add_line(StartX.pos.xyz, EndX.pos.xyz, ColorX);


	//Y Vector
	
	//set first point
	colored_vertex StartY;
	StartY.pos = { XMVectorGetX(M.r[3]), XMVectorGetY(M.r[3]), XMVectorGetZ(M.r[3]) };
	//set second point 
	colored_vertex EndY;
	EndY.pos = { XMVectorGetX(M.r[1]), XMVectorGetY(M.r[1]), XMVectorGetZ(M.r[1]), 0.0f };
	EndY.pos = Normalize(EndY.pos);
	EndY.pos = { EndY.pos.x * 2, EndY.pos.y * 2, EndY.pos.z * 2, EndY.pos.w * 2 };
	EndY.pos = { StartY.pos.x + EndY.pos.x, StartY.pos.y + EndY.pos.y, StartY.pos.z + EndY.pos.z, StartY.pos.w + EndY.pos.w };
	//add point to line
	end::debug_renderer::add_line(StartY.pos.xyz, EndY.pos.xyz, ColorY);

	//Z Vector
	
	//set first point
	colored_vertex StartZ;
	StartZ.pos = { XMVectorGetX(M.r[3]), XMVectorGetY(M.r[3]), XMVectorGetZ(M.r[3]) }; 
	//set second point 
	colored_vertex EndZ;
	EndZ.pos = { XMVectorGetX(M.r[2]), XMVectorGetY(M.r[2]), XMVectorGetZ(M.r[2]), 0.0f };
	EndZ.pos = Normalize(EndZ.pos);
	EndZ.pos = { EndZ.pos.x * 2, EndZ.pos.y * 2, EndZ.pos.z * 2, EndZ.pos.w * 2 };
	EndZ.pos = { StartZ.pos.x + EndZ.pos.x, StartZ.pos.y + EndZ.pos.y, StartZ.pos.z + EndZ.pos.z, StartZ.pos.w + EndZ.pos.w };
	//add point to line
	end::debug_renderer::add_line(StartZ.pos.xyz, EndZ.pos.xyz, ColorZ);
}

HRESULT end::renderer_t::impl_t::CreateAxisVertexBuffer(AxisModel * M)
{
	colored_vertex VertexArray[6];
	//set Y line
	VertexArray[0].pos   = { 0.0f, 0.0f, 0.0f };
	VertexArray[0].color = { 0.5f, 0.0f, 0.0f };

	VertexArray[1].pos   = { 0.0f, 1.0f, 0.0f };
	VertexArray[1].color = { 0.5f, 0.0f, 0.0f };
	//end::debug_renderer::add_line(VertexArray[0].pos, VertexArray[1].pos, VertexArray[1].color);

	//Set X Line
	VertexArray[2].pos   = { 0.0f, 0.0f, 0.0f };
	VertexArray[2].color = { 1.0f, 0.0f, 0.0f };

	VertexArray[3].pos   = { 1.0f, 0.0f, 0.0f };
	VertexArray[3].color = { 1.0f, 0.0f, 0.0f };

	//Set Z Line
	VertexArray[4].pos   = { 0.0f, 0.0f, 0.0f };
	VertexArray[4].color = { 1.0f, 1.0f, 1.0f };

	VertexArray[5].pos   = { 0.0f, 0.0f, 1.0f };
	VertexArray[5].color = { 1.0f, 1.0f, 1.0f };

	//fill Axis vertex buffer
	HRESULT hr;
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(colored_vertex) * 6;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData = {};
	InitData.pSysMem = VertexArray;
	hr = device->CreateBuffer(&bd, &InitData, &M->VertexBuffer);
	if (FAILED(hr))
	{
		return hr;
	}
}






#pragma endregion

#pragma region GridCreation

void end::renderer_t::impl_t::Generate_Points_Array()
{

	StartXVal = 0.0f;
	StartYVal = 0.0f;
	StartZVal = 0.0f;

	EndXVal = 0.0f;
	EndYVal = 0.0f;
	EndZVal = 0.0f;

	// clear array
	//end::debug_renderer::clear_lines();

	//Vertical Lines
	for (int i = 0; i < 20; i++)
	{
		//create first point
		float3 FirstPoint;
		FirstPoint.x = StartXVal;
		FirstPoint.y = 0.0f;
		FirstPoint.z = 0.0f;

		//first color
		float4 ColorOne;
		ColorOne.x = Colorx;
		ColorOne.y = Colory;
		ColorOne.z = Colorz;
		ColorOne.w = 0.0f;

		//generate Second point
		float3 SecondPoint;
		SecondPoint.x = EndXVal;
		SecondPoint.y = 0.0f;
		SecondPoint.z = 19.0f;

		//second color
		float4 ColorTwo;
		ColorTwo.x = Colorx;
		ColorTwo.y = Colory;
		ColorTwo.z = Colorz;
		ColorTwo.w = 0.0f;

		//add point array using add_line function
		end::debug_renderer::add_line(FirstPoint, SecondPoint, ColorOne, ColorTwo);

		//increment Start X and End X 
		StartXVal++;
		EndXVal++;
	}

	//Horizontal Lines
	for (int i = 0; i < 20; i++)
	{
		//create first point
		float3 FirstPoint2;
		FirstPoint2.x = 0.0f;
		FirstPoint2.y = 0.0f;
		FirstPoint2.z = StartZVal;

		//first color
		float4 ColorOne2;
		ColorOne2.x = Colorx;
		ColorOne2.y = Colory;
		ColorOne2.z = Colorz;
		ColorOne2.w = 0.0f;

		//generate Second point
		float3 SecondPoint2;
		SecondPoint2.x = 19.0f;
		SecondPoint2.y = 0.0f;
		SecondPoint2.z = EndZVal;

		//second color
		float4 ColorTwo2;
		ColorTwo2.x = Colorx;
		ColorTwo2.y = Colory;
		ColorTwo2.z = Colorz;
		ColorTwo2.w = 0.0f;

		//add point array using add_line function
		end::debug_renderer::add_line(FirstPoint2, SecondPoint2, ColorOne2, ColorTwo2);

		//increment Start X and End X 
		StartZVal++;
		EndZVal++;

	}
	//LineVertCount = end::debug_renderer::get_line_vert_count();
}

#pragma endregion

#pragma region InputCapture

void end::renderer_t::impl_t::Input(view_t & view)
{

#pragma region WASD_Movement


	if (GetAsyncKeyState('W'))
	{
		//Set translation matrix
		view.T = DirectX::XMMatrixTranslation(0.0f, 0.0f, 0.1f);
		//set camera equal to the multiplication of T and Camera matrices
		view.Camera = DirectX::XMMatrixMultiply(view.T, view.Camera);
		view.View = DirectX::XMMatrixInverse(nullptr, view.Camera);

	}


	

	if (GetAsyncKeyState('S'))
	{
		//Set translation matrix
		view.T = DirectX::XMMatrixTranslation(0.0f, 0.0f, -0.1f);
		//set camera equal to the multiplication of T and Camera matrices
		view.Camera = DirectX::XMMatrixMultiply(view.T, view.Camera);
		view.View = DirectX::XMMatrixInverse(nullptr, view.Camera);

	}
	if (GetAsyncKeyState('A'))
	{
		//Set translation matrix
		view.T = DirectX::XMMatrixTranslation(-0.1f, 0.0f, 0.0f);
		//set camera equal to the multiplication of T and Camera matrices
		view.Camera = DirectX::XMMatrixMultiply(view.T, view.Camera);
		view.View = DirectX::XMMatrixInverse(nullptr, view.Camera);
	}
	if (GetAsyncKeyState('D'))
	{
		//Set translation matrix
		view.T = DirectX::XMMatrixTranslation(0.1f, 0.0f, 0.0f);
		//set camera equal to the multiplication of T and Camera matrices
		view.Camera = DirectX::XMMatrixMultiply(view.T, view.Camera);
		view.View = DirectX::XMMatrixInverse(nullptr, view.Camera);
	}



#pragma endregion

#pragma region UpAndDown

	if (GetAsyncKeyState('E'))
	{

		view.T = XMMatrixTranslation(0, 0.05, 0);
		view.Camera = XMMatrixMultiply(view.T, view.Camera);
		view.View = XMMatrixInverse(nullptr, view.Camera);

	}

	if (GetAsyncKeyState('Q'))
	{

		view.T = XMMatrixTranslation(0, -0.05, 0);
		view.Camera = XMMatrixMultiply(view.T, view.Camera);
		view.View = XMMatrixInverse(nullptr, view.Camera);

	}

#pragma endregion

#pragma region LookAround

	//Look Up & Down
	if (GetAsyncKeyState(VK_NUMPAD8))
	{

		//create rotation matrix
		XMMATRIX RotateX = XMMatrixRotationX(-0.005);
		//create Vector which contains the translation information of the camera matrix
		XMVECTOR Position = view.Camera.r[3];
		//clear out the translation data from the camera matrix
		view.Camera.r[3] = XMVectorSet(0, 0, 0, 1);
		//multiply the rotation matrix by the camera matrix
		view.Camera = XMMatrixMultiply(RotateX, view.Camera);
		//set the cameras translation vector data to the position vector data
		view.Camera.r[3] = Position;
		//set the camera/view matrix equal to the inverse of the camera matrix
		view.View = XMMatrixInverse(nullptr, view.Camera);

	}
	if (GetAsyncKeyState(VK_NUMPAD2))
	{

		XMMATRIX RotateX = XMMatrixRotationX(0.005);
		XMVECTOR Position = view.Camera.r[3];
		view.Camera.r[3] = XMVectorSet(0, 0, 0, 1);
		view.Camera = XMMatrixMultiply(RotateX, view.Camera);
		view.Camera.r[3] = Position;
		view.View = XMMatrixInverse(nullptr, view.Camera);

	}

	//Look Left & Right
	if (GetAsyncKeyState(VK_NUMPAD4))
	{
		XMMATRIX RotateY = XMMatrixRotationY(-0.005);
		XMVECTOR Position = view.Camera.r[3];
		view.Camera.r[3] = XMVectorSet(0, 0, 0, 1);
		view.Camera = XMMatrixMultiply(view.Camera, RotateY);
		view.Camera.r[3] = Position;
		view.View = XMMatrixInverse(nullptr, view.Camera);
	}
	if (GetAsyncKeyState(VK_NUMPAD6))
	{

		XMMATRIX RotateY = XMMatrixRotationY(0.005);
		XMVECTOR Position = view.Camera.r[3];
		view.Camera.r[3] = XMVectorSet(0, 0, 0, 1);
		view.Camera = XMMatrixMultiply(view.Camera, RotateY);
		view.Camera.r[3] = Position;
		view.View = XMMatrixInverse(nullptr, view.Camera);
	}

#pragma endregion

#pragma region AxisControl

	if (GetAsyncKeyState(VK_UP))
	{
		//Set translation matrix
		XMMATRIX T = DirectX::XMMatrixTranslation(0.0f, 0.0f, -0.1f);
		//set camera equal to the multiplication of T and Camera matrices
		Movement = DirectX::XMMatrixMultiply(T, Movement);
		UserAxis = DirectX::XMMatrixInverse(nullptr, Movement);

	}

	if (GetAsyncKeyState(VK_DOWN))
	{
		//Set translation matrix
		XMMATRIX T = DirectX::XMMatrixTranslation(0.0f, 0.0f, 0.1f);
		//set camera equal to the multiplication of T and Camera matrices
		Movement = DirectX::XMMatrixMultiply(T, Movement);
		UserAxis = DirectX::XMMatrixInverse(nullptr, Movement);

	}

	if (GetAsyncKeyState(VK_LEFT))
	{
		//Set translation matrix
		XMMATRIX T = DirectX::XMMatrixTranslation(0.1f, 0.0f, 0.0f);
		//set camera equal to the multiplication of T and Camera matrices
		Movement = DirectX::XMMatrixMultiply(T, Movement);
		UserAxis = DirectX::XMMatrixInverse(nullptr, Movement);

	}

	if (GetAsyncKeyState(VK_RIGHT))
	{
		//Set translation matrix
		XMMATRIX T = DirectX::XMMatrixTranslation(-0.1f, 0.0f, 0.0f);
		//set camera equal to the multiplication of T and Camera matrices
		Movement = DirectX::XMMatrixMultiply(T, Movement);
		UserAxis = DirectX::XMMatrixInverse(nullptr, Movement);

	}



	if (GetAsyncKeyState('O'))
	{
		//Set translation matrix
		XMMATRIX T = DirectX::XMMatrixTranslation(0.0f, -0.1f, 0.0f);
		//set camera equal to the multiplication of T and Camera matrices
		Movement = DirectX::XMMatrixMultiply(T, Movement);
		UserAxis = DirectX::XMMatrixInverse(nullptr, Movement);

	}

	if (GetAsyncKeyState('P'))
	{
		//Set translation matrix
		XMMATRIX T = DirectX::XMMatrixTranslation(0.0f, 0.1f, 0.0f);
		//set camera equal to the multiplication of T and Camera matrices
		Movement = DirectX::XMMatrixMultiply(T, Movement);
		UserAxis = DirectX::XMMatrixInverse(nullptr, Movement);

	}

#pragma endregion

#pragma region AxisRotation

	if (GetAsyncKeyState('K'))
	{
		//capture position
		XMFLOAT4 Pos = { XMVectorGetX(UserAxis.r[3]), XMVectorGetY(UserAxis.r[3]),  XMVectorGetZ(UserAxis.r[3]),XMVectorGetW(UserAxis.r[3]) };
		//create rotation array
		XMMATRIX RotY = XMMatrixRotationY(0.005f);
		//mult useraxis matrix and rotation matrix
		Movement = XMMatrixMultiply(Movement, RotY);
		UserAxis = XMMatrixInverse(nullptr, Movement);


	}
	if (GetAsyncKeyState('L'))
	{
		//capture position
		XMFLOAT4 Pos = { XMVectorGetX(UserAxis.r[3]), XMVectorGetY(UserAxis.r[3]),  XMVectorGetZ(UserAxis.r[3]),XMVectorGetW(UserAxis.r[3]) };
		//create rotation array
		XMMATRIX RotY = XMMatrixRotationY(-0.005f);
		//mult useraxis matrix and rotation matrix
		Movement = XMMatrixMultiply(Movement, RotY);
		UserAxis = XMMatrixInverse(nullptr, Movement);

	}


#pragma endregion

#pragma region ActivateMatrixFunctions

	if (GetAsyncKeyState('L'))
	{
		if (KeyBuffer == false)
		{
			KeyBuffer = true;
			ActivateLookAt = !ActivateLookAt;
		}
	}
	else
	{
		KeyBuffer = false;
	}


	if (GetAsyncKeyState('K'))
	{
		if (KeyBuffer == false)
		{
			KeyBuffer = true;
			ActivateTurnTo = !ActivateTurnTo;
		}
	}
	else
	{
		KeyBuffer = false;
	}


#pragma endregion

#pragma region ActivateParticles
	if (GetAsyncKeyState('M'))
	{
		if (KeyBuffer == false)
		{
			KeyBuffer = true;
			RenderParticles = !RenderParticles;
		}
	}
	else
	{
		KeyBuffer = false;
	}
#pragma endregion

}

#pragma endregion

#pragma region CameraFunctions
void end::renderer_t::impl_t::LookAtFunction( DirectX::XMMATRIX M, end::XMFLOAT4 TargetPos, end::XMFLOAT4 LocalUp)
{
	//Capture Viewer Position (Axis Position)
	XMFLOAT4 ViewerPos = { XMVectorGetX(M.r[3]), XMVectorGetY(M.r[3]), XMVectorGetZ(M.r[3]), XMVectorGetW(M.r[3]) };

	//calculate new Z vector
	XMFLOAT4 Z = { 
				   TargetPos.x - ViewerPos.x,
				   TargetPos.y - ViewerPos.y,
				   TargetPos.z - ViewerPos.z,
				   TargetPos.w - ViewerPos.w
				 };
	//float4 ZZ = { Z.x, Z.y, Z.z, 0.0f };
	//Normalize Z
	Z = XM_Normalize(Z);
	//define up

	float3 up = { LocalUp.x, LocalUp.y, LocalUp.z, };
	float3 Tempz = { Z.x, Z.y, Z.z };

	//calculate New X vector (X = crossProduct(Up, ZZ))
	float3 NewX = cross(up, Tempz);
	//Normalize X vector
	XMFLOAT4 NormX = XM_Normalize({ NewX.x, NewX.y, NewX.z, 1.0f });
	float3 Tempx = { NormX.x, NormX.y, NormX.z };
	//calculate New Y vector (Y = crossProduct(ZZ, X))
	float3 NewY = cross(Tempz, Tempx);
	//normalize Y vector
	XMFLOAT4 NormY = XM_Normalize({ NewY.x, NewY.y, NewY.z, 1.0f });
	//create view matrix using the new X, Y, Z vectors 

	M.r[0] = { NormX.x, NormX.y, NormX.z, NormX.w };
	M.r[1] = { NormY.x, NormY.y, NormY.z, NormY.w };
	M.r[2] = { Z.x, Z.y, Z.z, Z.w };
	M.r[3] = { ViewerPos.x, ViewerPos.y, ViewerPos.z, ViewerPos.w };

	LAAxis = M;

	FLOAT XPOS2 = XMVectorGetX(M.r[2]);
	FLOAT YPOS2 = XMVectorGetY(M.r[2]);
	FLOAT ZPOS2 = XMVectorGetZ(M.r[2]);

	int stp = 0;
}

void end::renderer_t::impl_t::TurnToFunction(DirectX::XMMATRIX M, DirectX::XMVECTOR TargetPos, float SpeedScalar)
{
	//capture Axis models Postion
	XMVECTOR ViewerPos = { XMVectorGetX(M.r[3]), XMVectorGetY(M.r[3]), XMVectorGetZ(M.r[3]), XMVectorGetW(M.r[3]) };

	//create vector from viewer to object
	XMVECTOR ToObject = TargetPos - ViewerPos;
	
	//Normalize vector*
	ToObject = XMVector3Normalize(ToObject);
	XMVECTOR DotX = XMVector3Dot(ToObject, M.r[1]);
	XMVECTOR DotY = XMVector3Dot(ToObject, M.r[0]);
	
	XMMATRIX RotY = XMMatrixIdentity();
	XMMATRIX RotX = XMMatrixIdentity();

	RotY = XMMatrixRotationY(SpeedScalar * XMVectorGetX(DotY) );
	RotX = XMMatrixRotationX(-SpeedScalar * XMVectorGetX(DotX) );

	//set Viewer matrix
	M = XMMatrixMultiply(RotY, M);
	M = XMMatrixMultiply(RotX, M);

	//nomralize Viewer Matrix
	XMVECTOR NewX   = XMVector3Normalize(M.r[0]);
	XMVECTOR NewY   = XMVector3Normalize(M.r[1]);
	XMVECTOR NewZ   = XMVector3Normalize(M.r[2]);
	//XMVECTOR NewPos = XMVector4Normalize(M.r[3]);

	//M.r[0] = NewX;
	//M.r[1] = NewY;
	//M.r[2] = NewZ;
	//M.r[3] = ViewerPos;

	//redo look at algorithm
	//use Z from beginning of function
	float3 UP = { 0.0f, 1.0f, 0.0f };
	float3 ZZ = { XMVectorGetX(NewZ), XMVectorGetY(NewZ),  XMVectorGetZ(NewZ) };
	
	float3 float3X = cross(UP, ZZ);
	XMFLOAT4 VecX = XM_Normalize({ float3X.x, float3X.y, float3X.z, 0.0f });
	float3 NormX = { VecX.x, VecX.y, VecX.z };

	float3 float3Y = cross(ZZ, NormX);
	XMFLOAT4 VecY = XM_Normalize({ float3Y.x, float3Y.y, float3Y.z, 0.0f });

	XMVECTOR XVec = { VecX.x, VecX.y, VecX.z, 0 };
	XMVECTOR YVec = { VecY.x, VecY.y, VecY.z, 0 };

	M.r[0] = XVec;
	M.r[1] = YVec;
	M.r[2] = NewZ;
	M.r[3] = ViewerPos;
	
	TTAxis = M;

	int stop2 = 0;
}

DirectX::XMMATRIX end::renderer_t::impl_t::MouseLookFunction(DirectX::XMMATRIX M, float dx, float dy)
{

	//capture viewer matrix position
	XMVECTOR ViewerPos = M.r[3];
	//create Y Rotation matrix 
	XMMATRIX RotY = XMMatrixIdentity();
	float RotYVal = -0.005f * dx;
	if (RotYVal == -0.0f)
	{
		RotYVal = 0.0f;
	}
	RotY = XMMatrixRotationY(RotYVal);

	//create X Rotation Matrix
	XMMATRIX RotX = XMMatrixIdentity();
	float RotXVal = -0.005f * dy;
	if (RotXVal == -0.0f)
	{
		RotXVal = 0.0f;
	}
	RotX = XMMatrixRotationX(RotXVal);

	//M = XMMatrixMultiply(RotY, M);
	//M = XMMatrixMultiply(RotX, M);

	M = XMMatrixMultiply(M, RotY);
	M = XMMatrixMultiply(M, RotX);

	//take current Z 
	XMVECTOR Z = XMVector3Normalize(M.r[2]);
	//create X vector
	XMVECTOR X = XMVector3Cross({0.0f, 1.0f, 0.0f, 0.0f}, Z);
	//Normalize X
	X = XMVector3Normalize(X);
	//Create Y vector
	XMVECTOR Y = XMVector3Cross(Z, X);
	//Normalize Y Vector
	Y = XMVector3Normalize(Y);
	//set Matrix
	M.r[0] = X;
	M.r[1] = Y;
	M.r[2] = Z;
	M.r[3] = ViewerPos;

	return M;	 
}
#pragma endregion

#pragma region CreateTheAABB's'

void end::renderer_t::impl_t::AddAABBLines(end::aabb_t & aabb_Container, float4 color)
{
	float3 center = aabb_Container.center;
	float3 extents = aabb_Container.extents;

	float3 ntl = { center.x - extents.x, center.y + extents.y, center.z - extents.z };
	float3 ntr = { center.x + extents.x, center.y + extents.y, center.z - extents.z };
	float3 nbr = { center.x + extents.x, center.y - extents.y, center.z - extents.z };
	float3 nbl = { center.x - extents.x, center.y - extents.y, center.z - extents.z };

	float3 ftl = { center.x - extents.x, center.y + extents.y, center.z + extents.z };
	float3 ftr = { center.x + extents.x, center.y + extents.y, center.z + extents.z };
	float3 fbr = { center.x + extents.x, center.y - extents.y, center.z + extents.z };
	float3 fbl = { center.x - extents.x, center.y - extents.y, center.z + extents.z };


	//back face 
	end::debug_renderer::add_line(ntl, ntr, color);
	end::debug_renderer::add_line(ntr, nbr, color);
	end::debug_renderer::add_line(nbr, nbl, color);
	end::debug_renderer::add_line(nbl, ntl, color);

	//front face 
	end::debug_renderer::add_line(ftl, ftr, color);
	end::debug_renderer::add_line(ftr, fbr, color);
	end::debug_renderer::add_line(fbr, fbl, color);
	end::debug_renderer::add_line(fbl, ftl, color);

	//sides 
	end::debug_renderer::add_line(ntl, ftl, color);
	end::debug_renderer::add_line(ntr, ftr, color);
	end::debug_renderer::add_line(nbr, fbr, color);
	end::debug_renderer::add_line(nbl, fbl, color);
}

void end::renderer_t::impl_t::CreateAABB(end::aabb_t & aabb_Container)
{
	float range = 30.0f;
	aabb_Container.center = { Rand_FLOAT(-range, range), 0.0f, Rand_FLOAT(-range, range) };
	aabb_Container.extents = { Rand_FLOAT(1.0f, 5.0f), Rand_FLOAT(1.0f, 5.0f), Rand_FLOAT(1.0f, 5.0f) };
}

#pragma endregion


#pragma region Update

void end::renderer_t::impl_t::Update(view_t & view, float AccumulatedX, float AccumulatedY, bool MouseLook)
{
	//capture time
	Timer.Signal();

	//redefine grid Verts
	Generate_Points_Array();
	
	//update particles
	//updateParticles();
	Time += Timer.SmoothDelta();

	//Get Input
	Input(view);

	//set grid color based off time
	GridDelta = (float)Timer.Delta();

#pragma region GridColorChange


	if (ColorTimer == 0)
	{
		if (ColorUp == true)
		{
			if (Colory >= 1.0f)
			{
				ColorDown = true;
				ColorUp = false;
			}
			else
			{
				Colory += Colory * GridDelta;
			}
		}
		if (ColorDown == true)
		{
			if (Colory <= 0.1f)
			{
				ColorDown = false;
				ColorUp = true;
			}
			else
			{
				Colory -= Colory * GridDelta;
			}
		}
		ColorTimer = 4;
	}



#pragma endregion

#pragma region Particles

	//sorted pool particles 
	if (RenderParticles == true)
	{
		updateParticles();
		allocateParticles(NUMPARTICLES);

		UpdateFreeParticles();
		AllocateFreeParticles(NUMPARTICLES);

	}
	
#pragma endregion

#pragma region AxisCreation

	//call axis creation
	AddAxisLines(UserAxis, ColorX, ColorY, ColorZ);
	/*AddAxisLines(LAAxis, ColorX, ColorY, ColorZ);
	AddAxisLines(TTAxis, ColorX, ColorY, ColorZ);*/

#pragma endregion

#pragma region CreateFrustum


	//set frustum matrix
	view.FrustumMatrix = UserAxis;
	//calculate frustum
	end::calculate_frustum(frustum, view);
	
	//check if aabb is inside frustum

	for (int i = 0; i < ARRAYSIZE(aabbArray); ++i)
	{
		if (aabb_to_frustum(aabbArray[i].aabb, frustum) == true)
		{
			AddAABBLines(aabbArray[i].aabb, { 1.0f, 0.0f, 0.0f, 1.0f });
		}
		else
		{
			AddAABBLines(aabbArray[i].aabb, { 0.0f, 0.0f, 1.0f, 1.0f });
		}
	}

	
#pragma endregion


#pragma region Pools

	for (int i = 0; i < SortedPool.size(); i++)
	{
		float3 start = { SortedPool[i].Prev_Position.x, SortedPool[i].Prev_Position.y, SortedPool[i].Prev_Position.z };
		float3 end = { SortedPool[i].Position.x, SortedPool[i].Position.y, SortedPool[i].Position.z };
		debug_renderer::add_line(start, end, SortedPool[i].Color);
	}


	for (int i = 0; i < ParticleEmitter.indices.size(); i++)
	{
		int index = ParticleEmitter.indices[i];
		float3 start2 = { FreePool[index].Prev_Position.x, FreePool[index].Prev_Position.y, FreePool[index].Prev_Position.z };
		float3 end2 = { FreePool[index].Position.x, FreePool[index].Position.y,FreePool[index].Position.z };
		debug_renderer::add_line(start2, end2, ParticleEmitter.SpawnColor);
	}

#pragma endregion

#pragma region LookAtFunction

	if (MouseLook == true )
	{
		//call mouse look function
		view.Camera = MouseLookFunction(view.Camera, AccumulatedX, AccumulatedY);
		view.View = XMMatrixInverse(nullptr, view.Camera);
		//view.View =  MouseLookFunction(view.View, AccumulatedX, AccumulatedY);
	}

	if (ActivateLookAt == true)
	{		
		XMFLOAT4 TargetPos = { XMVectorGetX(UserAxis.r[3]), XMVectorGetY(UserAxis.r[3]), XMVectorGetZ(UserAxis.r[3]), XMVectorGetW(UserAxis.r[3]) };
		LookAtFunction(LAAxis, TargetPos, { 0.0f,1.0f,0.0f,0.0f });
	}

	if (ActivateTurnTo == true)
	{
		XMVECTOR TargetPos = { XMVectorGetX(UserAxis.r[3]), XMVectorGetY(UserAxis.r[3]), XMVectorGetZ(UserAxis.r[3]), XMVectorGetW(UserAxis.r[3]) };
		TurnToFunction(TTAxis, TargetPos, Timer.Delta());

		int Stop = 0;
	}

#pragma endregion

	int d = end::debug_renderer::get_line_vert_count();
	int breakpoint = 0;

	if (ColorTimer != 0)
	{
		ColorTimer--;
	}
}
#pragma endregion

