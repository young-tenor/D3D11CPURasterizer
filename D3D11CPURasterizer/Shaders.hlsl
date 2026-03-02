struct VSOut
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD;
};

Texture2D g_texture : register(t0);
SamplerState g_sampler : register(s0);

VSOut vs_main(uint id : SV_VertexID)
{
    VSOut output;
    output.uv = float2((id << 1) & 2, id & 2);
    output.pos = float4(output.uv * float2(2.0f, -2.0f) + float2(-1.0f, 1.0f), 0.0f, 1.0f);
    return output;
}

float4 ps_main(VSOut input) : SV_TARGET
{
    return g_texture.Sample(g_sampler, input.uv);
}