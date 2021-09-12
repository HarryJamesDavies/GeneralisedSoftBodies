//--------------------------------------------------------------------------------------
// texture
//--------------------------------------------------------------------------------------
Texture2D		myTexture : register(t0);
SamplerState	mySampler : register(s0);

//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
cbuffer ConstantBuffer : register(b0)
{
	matrix World;
	matrix View;
	matrix Projection;
	matrix rot;
	float4 lightCol;
	float4 ambientCol;
	float3 lightPos;
}

struct PS_INPUT
{
	float4 Pos : SV_POSITION;
	float4 worldPos: POSITION;
	float4 Norm : NORMAL;
	float4 Color : COLOR;
	float2 texCoord : TEXCOORD;
};


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 main(PS_INPUT input) : SV_Target
{
	float4 vertexCol = input.Color * myTexture.Sample(mySampler, input.texCoord);
	float3 lightDir = normalize(input.worldPos - lightPos);
	float4 diffuse = saturate(max(0.0f, dot(lightDir, normalize(input.Norm))) * lightCol);
	return saturate((diffuse + ambientCol) * input.Color);
	return input.Color;
}
