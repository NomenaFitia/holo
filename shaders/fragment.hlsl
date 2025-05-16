struct PS_INPUT
{
    float4 position : SV_POSITION; // Position du sommet projetée
};

float4 main(PS_INPUT input) : SV_TARGET
{
    // Couleur blanche
    return float4(1.0f, 1.0f, 1.0f, 1.0f);
}