//***************************************************************************************
// Sky.cpp by Frank Luna (C) 2011 All Rights Reserved.
//***************************************************************************************

#include "Sky.h"
#include "GeometryGenerator.h"
#include "Camera.h"
#include "Vertex.h"
#include "Effects.h"
#include <fstream>
using namespace std;
Sky::Sky(ID3D11Device* device, const std::wstring& cubemapFilename, float skySphereRadius)
{
	HR(D3DX11CreateShaderResourceViewFromFile(device, cubemapFilename.c_str(), 0, 0, &mCubeMapSRV, 0));

	/*GeometryGenerator::MeshData sphere;
	GeometryGenerator geoGen;
	geoGen.CreateSphere(skySphereRadius, 30, 30, sphere);

	std::vector<XMFLOAT3> vertices(sphere.Vertices.size());

	for(size_t i = 0; i < sphere.Vertices.size(); ++i)
	{
		vertices[i] = sphere.Vertices[i].Position;
	}*/
	std::vector<XMFLOAT3> vertices(4);
	vertices[0]=XMFLOAT3(1.0,1.0,0.0);
	vertices[1]=XMFLOAT3(1,-1,0);
	vertices[2]=XMFLOAT3(-1,-1,0);
	vertices[3]=XMFLOAT3(-1,1,0);
    D3D11_BUFFER_DESC vbd;
    vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(XMFLOAT3) * vertices.size();
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA vinitData;
    vinitData.pSysMem = &vertices[0];

    HR(device->CreateBuffer(&vbd, &vinitData, &mVB));
	

	//mIndexCount = sphere.Indices.size();
	mIndexCount = 6;
	D3D11_BUFFER_DESC ibd;
    ibd.Usage = D3D11_USAGE_IMMUTABLE;
    ibd.ByteWidth = sizeof(DWORD) * mIndexCount;
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;
	ibd.StructureByteStride = 0;
    ibd.MiscFlags = 0;

	/*std::vector<USHORT> indices16;
	indices16.assign(sphere.Indices.begin(), sphere.Indices.end());*/
	DWORD indices[6] = {0,1,2,
	0,2,3};
	

    D3D11_SUBRESOURCE_DATA iinitData;
    iinitData.pSysMem = &indices[0];

    HR(device->CreateBuffer(&ibd, &iinitData, &mIB));

	if(loadLUTS("F_512_data.csv","Ftable",512,512, device) == S_FALSE)
		   loadLUTS("F_512_data.csv","Ftable",512,512, device); 
}

Sky::~Sky()
{
	ReleaseCOM(mVB);
	ReleaseCOM(mIB);
	ReleaseCOM(mCubeMapSRV);
}

ID3D11ShaderResourceView* Sky::CubeMapSRV()
{
	return mCubeMapSRV;
}

void Sky::Draw(ID3D11DeviceContext* dc, const Camera& camera)
{
	// center Sky about eye in world space
	XMFLOAT3 eyePos = camera.GetPosition();
//	XMMATRIX T = XMMatrixTranslation(eyePos.x, eyePos.y, eyePos.z);
	XMMATRIX T = XMMatrixTranslation(2, 2, 2);

	XMMATRIX WVP = XMMatrixMultiply(T, camera.ViewProj());
	XMMATRIX V = camera.View();
	XMVECTOR detv = XMMatrixDeterminant(V);
	XMMATRIX invView = XMMatrixInverse(&detv,V);

	XMMATRIX P = camera.Proj();
	XMVECTOR detp = XMMatrixDeterminant(P);
	XMMATRIX invProj = XMMatrixInverse(&detp,P);
	
	Effects::SkyFX->SetInvProj(invProj);
	Effects::SkyFX->SetInvView(invView);
	Effects::SkyFX->SetWorldViewProj(WVP);
	Effects::SkyFX->SetCubeMap(mCubeMapSRV);
	Effects::SkyFX->SetEye(eyePos);
	Effects::SkyFX->SetWorld(T);

	UINT stride = sizeof(XMFLOAT3);
    UINT offset = 0;
    dc->IASetVertexBuffers(0, 1, &mVB, &stride, &offset);
	dc->IASetIndexBuffer(mIB, DXGI_FORMAT_R32_UINT, 0);
	dc->IASetInputLayout(InputLayouts::Pos);
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
	D3DX11_TECHNIQUE_DESC techDesc;
    Effects::SkyFX->SkyTech->GetDesc( &techDesc );

    for(UINT p = 0; p < techDesc.Passes; ++p)
    {
        ID3DX11EffectPass* pass = Effects::SkyFX->SkyTech->GetPassByIndex(p);

		pass->Apply(0, dc);

		dc->DrawIndexed(mIndexCount, 0, 0);
	}
}
HRESULT Sky::loadLUTS(char* fileName, LPCSTR shaderTextureName, int xRes, int yRes, ID3D11Device* pd3dDevice)
{
    HRESULT hr = S_OK;
	
    ifstream infile (fileName ,ios::in);
    if (infile.is_open())
    {   
        float* data = new float[xRes*yRes];
        int index = 0;
        char tempc;
        for(int j=0;j<yRes;j++)
        {   for(int i=0;i<xRes-1;i++)  
               infile>>data[index++]>>tempc;
            infile>>data[index++];
            
        }
        
        D3D11_SUBRESOURCE_DATA InitData;
        InitData.SysMemPitch = sizeof(float) * xRes;
        InitData.pSysMem = data;

        ID3D11Texture2D* texture = NULL;
        D3D11_TEXTURE2D_DESC texDesc;
        ZeroMemory( &texDesc, sizeof(D3D11_TEXTURE2D_DESC) );
        texDesc.Width = xRes;
        texDesc.Height = yRes;
        texDesc.MipLevels = 1;
        texDesc.ArraySize = 1;
        texDesc.Format = DXGI_FORMAT_R32_FLOAT;
        texDesc.SampleDesc.Count = 1;
        texDesc.SampleDesc.Quality = 0;
        texDesc.Usage = D3D11_USAGE_DEFAULT;
        texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

        pd3dDevice->CreateTexture2D(&texDesc,&InitData,&texture);

        D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
        ZeroMemory( &SRVDesc, sizeof(SRVDesc) );
        SRVDesc.Format = texDesc.Format;
        SRVDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
        SRVDesc.Texture2D.MipLevels = 1;
        SRVDesc.Texture2D.MostDetailedMip = 0;

        ID3D11ShaderResourceView* textureRview;
        pd3dDevice->CreateShaderResourceView( texture, &SRVDesc, &textureRview);
		Effects::SkyFX->SetFtable(textureRview);
		//ID3D11EffectShaderResourceVariable* textureRVar = fx::SkyFX->GetVariableByName( shaderTextureName )->AsShaderResource();
        //textureRVar->SetResource( textureRview );

		texture->Release();
		textureRview->Release();
        delete[] data;
    }
    else
       hr = S_FALSE;
    return hr;
}