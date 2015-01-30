#include"drawshadowmap.h"
DrawShadowMap::DrawShadowMap():pd3dDevice(0),pd3dImmediateContext(0),mNDCQuadVB(0),PosTexlayout(0)
{
	
	
}
DrawShadowMap::~DrawShadowMap()
{
	pd3dDevice->Release();
	pd3dImmediateContext->Release();
	mNDCQuadVB->Release();
	PosTexlayout->Release();
}
void DrawShadowMap::init(ID3D11Device* device)
{
	pd3dDevice = device;
	pd3dDevice->GetImmediateContext(&pd3dImmediateContext);
}
void DrawShadowMap::buildNDCQuad()
{
	/*XMFLOAT3 pos[] = 
	{
		XMFLOAT3(0.0f, 0.0f, 0.0f),
		XMFLOAT3(0.0f,  1.0f, 0.0f),
		XMFLOAT3(1.0f,  1.0f, 0.0f),

		XMFLOAT3(0.0f, 0.0f, 0.0f),
		XMFLOAT3(1.0f,  1.0f, 0.0f),
		XMFLOAT3(0.0f, 0.0f, 0.0f)

	};*/
	XMFLOAT3 pos[] = 
	{
		XMFLOAT3(0.0f, 0.0f, 0.0f),
		XMFLOAT3(1.0f,  0.0f, 0.0f),
		XMFLOAT3(1.0f,  1.0f, 0.0f),
		XMFLOAT3(0.0f, 1.0f, 0.0f)
	};
	XMFLOAT2 tex[] = 
	{
		XMFLOAT2(0.0f, 1.0f),
		XMFLOAT2(0.0f, 0.0f),
		XMFLOAT2(1.0f, 0.0f),

		XMFLOAT2(0.0f, 1.0f),
		XMFLOAT2(1.0f, 0.0f),
		XMFLOAT2(1.0f, 1.0f)
	};

	QuadVertex qv[6];

	for(int i = 0; i < 6; ++i)
	{
		qv[i].pos  = pos[i];
		qv[i].texC = tex[i];
	}
	DWORD indices[6] = {
		// front face
		0, 1, 2,
		0, 2, 3
	};
	int mNumIndices = 6;
	D3D11_BUFFER_DESC ibd;
    ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(DWORD) * 6;
    ibd.BindFlags = D3D10_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;
    ibd.MiscFlags = 0;
    D3D11_SUBRESOURCE_DATA iinitData;
    iinitData.pSysMem = &indices[0];
    pd3dDevice->CreateBuffer(&ibd, &iinitData, &mIB);

	D3D11_BUFFER_DESC vbd;
    vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(QuadVertex) * 6;
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;
    D3D11_SUBRESOURCE_DATA vinitData;
    vinitData.pSysMem = qv;
    pd3dDevice->CreateBuffer(&vbd, &vinitData, &mNDCQuadVB);
	
}
void DrawShadowMap::init_showmap()
{
	buildNDCQuad();
	/*ID3DBlob* pBlobVS1 = NULL;
	ID3DBlob* pBlobPS1 = NULL;
	CompileShaderFromFile(L"showmap.hlsl", "VS", "vs_4_0", &pBlobVS1);
	CompileShaderFromFile(L"showmap.hlsl", "PS", "ps_4_0", &pBlobPS1);
	assert(pBlobVS1);
	assert(pBlobPS1);
	pd3dDevice->CreateVertexShader(pBlobVS1->GetBufferPointer(), pBlobVS1->GetBufferSize(), NULL, &g_pVS);
	pd3dDevice->CreatePixelShader(pBlobPS1->GetBufferPointer(), pBlobPS1->GetBufferSize(), NULL, &g_pPS);
	assert(g_pVS);
	assert(g_pPS);*/
	D3DX11_PASS_DESC passDesc;
	D3D11_INPUT_ELEMENT_DESC quad_desc[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0},
	//	{"TEXCOORD",  0, DXGI_FORMAT_R32G32_FLOAT, 12,0, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};
	Effects::DrawShadowMapFX->DrawShadowMapTech->GetPassByIndex(0)->GetDesc(&passDesc);
	pd3dDevice->CreateInputLayout(quad_desc, 1, passDesc.pIAInputSignature, 
		passDesc.IAInputSignatureSize, &PosTexlayout);
//	pd3dDevice->CreateInputLayout(quad_desc,2, pBlobVS1->GetBufferPointer(), pBlobVS1->GetBufferSize(), &PosTexlayout);

	
/*	SAFE_RELEASE(pBlobVS1);
	SAFE_RELEASE(pBlobPS1);*/
	assert(PosTexlayout);
}
void DrawShadowMap::showmap(/*ID3D11ShaderResourceView* m_pCurrStepMapSRV,*/const Camera& camera)
{
	init_showmap();
//	Effects::DrawShadowMapFX->SetTexture(m_pCurrStepMapSRV);
	XMMATRIX W = XMMatrixTranslation(-2, 1, 0);
	XMMATRIX WVP = camera.ViewProj()*W;
	Effects::DrawShadowMapFX->SetWVP(WVP);
	ID3DX11EffectTechnique* tech = Effects::DrawShadowMapFX->DrawShadowMapTech;
	//init_tex(pd3dDevice);
	UINT stride = sizeof(QuadVertex);
    UINT offset = 0;
   
	D3DX11_TECHNIQUE_DESC techDesc;
    tech->GetDesc( &techDesc );	
	pd3dImmediateContext->IASetInputLayout(PosTexlayout);
	pd3dImmediateContext->IASetVertexBuffers(0, 1, &mNDCQuadVB, &stride, &offset);
	pd3dImmediateContext->IASetIndexBuffer(mIB, DXGI_FORMAT_R32_UINT, 0);
	
	pd3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    for(UINT p = 0; p < techDesc.Passes; ++p)
    {
		ID3DX11EffectPass* pass = tech->GetPassByIndex(p);	
		pass->Apply(0,pd3dImmediateContext);
		pd3dImmediateContext->DrawIndexed(6, 0, 0);
	//	pd3dImmediateContext->Draw(6, 0);
	}
}