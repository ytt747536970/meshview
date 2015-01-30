#ifndef Draw_SHADOW_MAPPER_H
#define Draw_SHADOW_MAPPER_H

#include "d3dUtil.h"
#include "Camera.h"
#include "Effects.h"
struct QuadVertex
{
	XMFLOAT3 pos;
	XMFLOAT2 texC;
};
class DrawShadowMap
{
public:
	DrawShadowMap();
	~DrawShadowMap();
	void init(ID3D11Device* device);
	void buildNDCQuad();
	void init_showmap();
	void showmap(/*ID3D11ShaderResourceView* m_pCurrStepMapSRV,*/const Camera& camera);

private:
	DrawShadowMap(const DrawShadowMap& rhs);
	DrawShadowMap& operator=(const DrawShadowMap& rhs);

private:

	ID3D11Device* pd3dDevice;
	ID3D11DeviceContext* pd3dImmediateContext;
	ID3D11Buffer* mNDCQuadVB;
	ID3D11Buffer* mIB;
	ID3D11InputLayout* PosTexlayout;
};

#endif // SHADOW_MAPPER_Hvoid buildNDCQuad(ID3D11Device* pd3dDevice)