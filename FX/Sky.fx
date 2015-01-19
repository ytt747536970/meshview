//=============================================================================
// Sky.fx by Frank Luna (C) 2011 All Rights Reserved.
//
// Effect used to shade sky dome.
//=============================================================================

cbuffer cbPerFrame
{
	float4x4 gWorldViewProj;
	float4x4 gWorld;
	float4x4 g_mInverseProjection;
	float3 gEyePosW;
	float4x4 g_mInvView;
};
#define PI 3.14159265
Texture2D Ftable;
cbuffer cbImmutable
{
    float4 pointLightColor = float4(1.0,1.0,1.0,1.0);
    float3 g_PointLightPos = float3(  3.7,5.8,3.15);     
    float3 g_PointLightPos2 = float3(-3.7,5.8,3.15);
	float3 g_beta = float3(0.04,0.04,0.04);

	float g_PointLightIntensity = 2;
	float dirLightIntensity = 0.2;
    float g_fXOffset = 0; 
    float g_fXScale = 0.6366198; //1/(PI/2)
    float g_fYOffset = 0;        
    float g_fYScale = 0.5;
    
    float g_20XOffset = 0; 
    float g_20XScale = 0.6366198; //1/(PI/2) 
    float g_20YOffset = 0;
    float g_20YScale = 0.5;

    float g_diffXOffset = 0; 
    float g_diffXScale = 0.5;
    float g_diffYOffset = 0;        
    float g_diffYScale = 0.3183099;  //1/PI   
}
// Nonnumeric values cannot be added to a cbuffer.
SamplerState samLinearClamp
{
    Filter = MIN_MAG_LINEAR_MIP_POINT;
    AddressU = Clamp;
    AddressV = Clamp;
};
float3 calculateAirLightPointLight(float Dvp,float Dsv,float3 S,float3 V)
{
    float gamma = acos(dot(S, V));
    gamma = clamp(gamma,0.01,PI-0.01);
    float sinGamma = sin(gamma);
    float cosGamma = cos(gamma);
    float u = g_beta.x * Dsv * sinGamma;
    float v1 = 0.25*PI+0.5*atan((Dvp-Dsv*cosGamma)/(Dsv*sinGamma)); 
    float v2 = 0.5*gamma;
            
    float lightIntensity = g_PointLightIntensity * 40;        
            
    float f1= Ftable.SampleLevel(samLinearClamp, float2(/*(v1-g_fXOffset)**/g_fXScale, (u-g_fYOffset)*g_fYScale), 0).r;
    float f2= Ftable.SampleLevel(samLinearClamp, float2((v2-g_fXOffset)*g_fXScale, (u-g_fYOffset)*g_fYScale), 0).r;
    float airlight = (g_beta.x*lightIntensity*exp(-g_beta.x*Dsv*cosGamma))/(2*PI*Dsv*sinGamma)*(f1-f2);
    //A0=(g_beta.x*lightIntensity*exp(-g_beta.x*Dsv*cosGamma))/(2*PI*Dsv*sinGamma)
	//A1=Dsv*sinGamma;
    return float3(airlight.x,0,0);
}
TextureCube gCubeMap;
 
SamplerState samTriLinearSam
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};

struct VertexIn
{
	float3 PosL : POSITION;
};

struct VertexOut
{
	float4 PosH : SV_POSITION;
    float3 PosL : POSITION;
};
 
VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	vout.PosH = float4(vin.PosL, 1.0f);
	float4 unprojectedPos = mul( float4( vin.PosL, 1 ), g_mInverseProjection );
	unprojectedPos.xy *= 1.0;
    unprojectedPos.z = 1.0;
    unprojectedPos.w = 1;
	vout.PosL = mul(unprojectedPos, g_mInvView).xyz;
	// Set z = w so that z/w = 1 (i.e., skydome always on far plane).
	///vout.PosH = mul(float4(vin.PosL, 1.0f), gWorldViewProj).xyww;
	//vout.PosH = mul(float4(vin.PosL, 1.0f), gWorldViewProj).xyzw;
	// Use local vertex position as cubemap lookup vector.
	//vout.PosL = vin.PosL;
	
	return vout;
}

float4 PS(VertexOut pIn) : SV_Target
{
	float4 outputColor = float4(0,0,0,0);    
    float4 sceneColor =  float4(0,0,0,0);

    float3 viewVec = pIn.PosL - gEyePosW;
    float Dvp = length(viewVec);//视点到天空的距离
    float3 V =  normalize(viewVec);//视点到天空的方向
    float3 exDir = float3( exp(-g_beta.x*Dvp),  exp(-g_beta.y*Dvp),  exp(-g_beta.z*Dvp)  );

    // air light
	float3 lightEyeVec = g_PointLightPos - gEyePosW;//光源到视点的
	float Dsv = length(lightEyeVec);//光源到视点的距离
	float3 S = normalize(lightEyeVec);//光源到视点的方向
    float3 airlightColor = calculateAirLightPointLight(Dvp,Dsv,S,V);

	//directional light
    outputColor = float4( airlightColor.xyz, 1); 
	//return gCubeMap.Sample(samTriLinearSam, pin.PosL);
	return outputColor;
	return float4(0,1,0,0);
}

RasterizerState NoCull
{
    CullMode = None;
};

DepthStencilState LessEqualDSS
{
	// Make sure the depth function is LESS_EQUAL and not just LESS.  
	// Otherwise, the normalized depth values at z = 1 (NDC) will 
	// fail the depth test if the depth buffer was cleared to 1.
    DepthFunc = LESS_EQUAL;
};

technique11 SkyTech
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS() ) );
        
        SetRasterizerState(NoCull);
        SetDepthStencilState(LessEqualDSS, 0);
    }
}
