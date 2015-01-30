//=============================================================================
// drawshadowmap.fx by Frank Luna (C) 2008 All Rights Reserved.
//
// Effect used to display a shadow map on a quad.  The quad vertices are
// given in NDC space.
//=============================================================================
 
//Texture2D gShadowMap;
float4x4  WVP;
/*SamplerState samPoint
{
	Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	AddressU = Wrap;
	AddressV = Wrap;
};*/
SamplerState samLinear
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};
struct VS_IN
{
	float3 posL : POSITION;
//	float2 texC : TEXCOORD;
};

struct VS_OUT
{
	float4 posH : SV_POSITION;
//	float2 texC : TEXCOORD;
};
 
VS_OUT VS(VS_IN vIn)
{
	VS_OUT vOut;

	vOut.posH = mul(float4(vIn.posL, 1.0f),WVP);
//	vOut.posH = float4(vIn.posL, 1.0f);
//	vOut.texC = vIn.texC;
	
	return vOut;
}

float4 PS(VS_OUT pIn) : SV_Target
{
	//float r = gShadowMap.Sample(samLinear, pIn.texC).r;
	float r = 1;
	// draw as grayscale
	return float4(1,0,0,0);
	
}
 
technique11 DrawShadowMapTech
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS() ) );
    }
}
