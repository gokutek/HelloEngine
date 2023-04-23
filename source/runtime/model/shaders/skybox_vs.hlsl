struct VSInput
{
    float4 position : POSITION;
    float4 color : COLOR;
};

struct VSOutput
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

VSOutput main(VSInput vin)
{
    VSOutput vout;
    vout.position = vin.position;
    vout.color = vin.color;
    return vout;
}
