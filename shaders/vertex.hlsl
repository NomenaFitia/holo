struct VS_INPUT
{
    float3 inPosition : POSITION; // Position du sommet en entrée (sémantique POSITION)
};

struct VS_OUTPUT
{
    float4 position : SV_POSITION; // Position du sommet projetée (sémantique SV_POSITION)
    float pointSize : PSIZE; // Taille du point (sémantique PSIZE)
};

cbuffer MatrixBuffer : register(b0) // Constant buffer pour les matrices (registre b0)
{
    float4x4 u_view;
    float4x4 u_model;
    float4x4 u_proj;
};

cbuffer PointSizeBuffer : register(b1) // Constant buffer pour la taille du point (registre b1)
{
    float u_pointSize;
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;
    output.position = mul(mul(mul(float4(input.inPosition, 1.0f), u_model), u_view), u_proj);
    output.pointSize = u_pointSize; // Nécessite l'état D3D11_RASTERIZER_DESC::PointSpriteEnable = TRUE;
    return output;
}