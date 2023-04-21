struct VSOutput
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

float4 main(VSOutput vin) : SV_TARGET
{
    return vin.color;
}
