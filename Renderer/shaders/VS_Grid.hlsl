#include "mvp.hlsli"

struct VS_IN
{
    float4 position : POSITION;
    float4 color : COLOR;
};

struct VS_OUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};


VS_OUT main ( VS_IN input)
{
    VS_OUT output;
    output.position = mul(float4(input.position.xyz, 1.0f), modeling);
    output.position = mul(output.position, view);
    output.position = mul(output.position, projection);
    output.color = input.color;
    return output;
}