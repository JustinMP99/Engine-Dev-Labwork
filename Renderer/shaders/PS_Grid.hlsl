#include "mvp.hlsli"

struct PS_IN
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

struct PS_OUT
{
    
    float4 color : COLOR;
};



float4 main(PS_IN input) : SV_Target
{
    float4 output = (float4)0;

    output = input.color;

    return output;
}
