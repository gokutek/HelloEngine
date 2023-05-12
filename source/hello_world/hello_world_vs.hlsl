struct VSInput
{
    float4 position : POSITION;
};

struct VSOutput
{
    float4 position : SV_POSITION;
};

VSOutput main(VSInput vin)
{
    VSOutput vout;
    vout.position = vin.position;
    return vout;
}
