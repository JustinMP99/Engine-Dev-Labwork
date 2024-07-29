#ifdef __cplusplus
#define cbuffer struct
#define matrix DirectX::XMMATRIX
#endif

//cbuffer MVP_t : register(b0)
//{
//	matrix modeling;
//	matrix view;
//	matrix projection;
//};

cbuffer MVP_t 
{
    matrix modeling;
    matrix view;
    matrix projection;
};


//struct VS_OUT
//{
//    float4 position : POSITION;
//    float4 color : COLOR;
//};

//struct PS_OUT
//{
//    float4 position : SV_POSITION;
//    float4 color : COLOR;
//};

