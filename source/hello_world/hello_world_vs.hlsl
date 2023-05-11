struct VSInput
{
    float3 position : POSITION;
};

struct VSOutput
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

VSOutput main(VSInput vin)
{
    VSOutput vout;
    vout.position = float4(vin.position, 1.0f);
    vout.color = float4(1, 1, 1, 1);
    return vout;
}
