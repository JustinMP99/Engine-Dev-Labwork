#include "mvp.hlsli"

struct VSIn
{
    uint vertexId : SV_VertexID;
};

struct VSOut
{
    float4 pos : SV_POSITION;
    float4 normal : NORMAL;
    float4 color : COLOR;
};




static const float4 cube_n[6] =
{
	float4(-1.0f, 0.0f, 0.0f, 0.0f),
	float4(1.0f, 0.0f, 0.0f, 0.0f),
	float4(0.0f, -1.0f, 0.0f, 0.0f),
	float4(0.0f, 1.0f, 0.0f, 0.0f),
	float4(0.0f, 0.0f, -1.0f, 0.0f),
	float4(0.0f, 0.0f, 1.0f, 0.0f),
};

static const float4 cube_v[8] =
{
	float4(-1.0f,-1.0f,-1.0f, 1.0f),
	float4(-1.0f,-1.0f, 1.0f, 1.0f),
	float4(-1.0f, 1.0f,-1.0f, 1.0f),
	float4(-1.0f, 1.0f, 1.0f, 1.0f),

	float4(1.0f,-1.0f,-1.0f, 1.0f),
	float4(1.0f,-1.0f, 1.0f, 1.0f),
	float4(1.0f, 1.0f,-1.0f, 1.0f),
	float4(1.0f, 1.0f, 1.0f, 1.0f),
};

static const uint cube_i[36] =
{
	0,1,2, // -x
	1,3,2,

	4,6,5, // +x
	5,6,7,

	0,5,1, // -y
	0,4,5,

	2,7,6, // +y
	2,3,7,

	0,6,4, // -z
	0,2,6,

	1,7,3, // +z
	1,5,7
};


//Line Struct
static const float3 LineVertsPOS[16] =
{
    //Vertical
    //line 1
    float3(0.0f, 0.0f, 0.0f),
    float3(0.0f, 0.0f, -3.0f),

    //line 2
    float3(1.0f, 0.0f, 0.0f),
    float3(1.0f, 0.0f, -3.0f),

    //line 3
    float3(2.0f, 0.0f, 0.0f),
    float3(2.0f, 0.0f, -3.0f),

    //line 4
    float3(3.0f, 0.0f, 0.0f),
    float3(3.0f, 0.0f, -3.0f),


    //Horizontal
    //line 1
    float3(0.0f, 0.0f, 0.0f),
    float3(3.0f, 0.0f, 0.0f),

    //line 2
    float3(0.0f, 0.0f, 1.0f),
    float3(3.0f, 0.0f, 1.0f),

    //line 3
    float3(0.0f, 0.0f, 2.0f),
    float3(3.0f, 0.0f, 2.0f),

    //line 4
    float3(0.0f, 0.0f, 3.0f),
    float3(3.0f, 0.0f, 3.0f),


};

static const uint LineIndexer[16] =
{
  
    //Vertical
    //line1
    0,
    1,
    //line2
    2,
    3,
    //line3
    4,
    5,
    //line4
    6,
    7,

    //Horizontal
    //line1
    8,
    9,
    //line2
    10,
    11,
    //line3
    12,
    13,
    //line4
    14,
    15,



};





VSOut main(VSIn input)
{
    VSOut output;

    uint vert_index = LineIndexer[input.vertexId];

    float3 vert_pos = LineVertsPOS[vert_index];
    //float4 vert_norm = cube_n[input.vertexId / 6];

    output.pos = mul(vert_pos, modeling);
    output.pos = mul(output.pos, view);
    output.pos = mul(output.pos, projection);



    //output.color = float4((vert_norm.xyz + 1.0f) * 0.5f, 1.0f);

    //output.normal = mul(vert_norm, modeling);

    return output;
}
	


//VSOut main(VSIn input)
//{
//	VSOut output;

//	uint vert_index = cube_i[input.vertexId];

//	float4 vert_pos = cube_v[vert_index];
//	float4 vert_norm = cube_n[input.vertexId / 6];

//	output.pos = mul( vert_pos, modeling);
//	output.pos = mul(output.pos, view);
//	output.pos = mul(output.pos, projection);

//	output.color = float4((vert_norm.xyz + 1.0f) * 0.5f, 1.0f);

//	output.normal = mul(vert_norm, modeling);

//	return output;
//}
	