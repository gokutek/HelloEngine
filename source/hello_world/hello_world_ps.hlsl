struct VSOutput
{
    float4 position : SV_POSITION;
};

float4 main(VSOutput vin) : SV_TARGET
{
    return float4(1, 1, 1, 1);
}
