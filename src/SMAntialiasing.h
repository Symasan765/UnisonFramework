/*=================================================
//								FILE : SMAntialiasing.h
//		�t�@�C������ :
//		SMAA�̏������s���V�F�[�_����������
//		http://www.iryoku.com/smaa/
//		http://hikita12312.hatenablog.com/entry/2017/12/16/160429
//							HAL��� : ���{ �Y�V��
=================================================*/
#pragma once
#include "PostEffects.h"
#include "SMAAEdge.h"
#include "SMAABlendingWeight.h"
#include "SMAABlending.h"

//SMAA�������s���N���X
class cSMAntialiasing {
public:
	cSMAntialiasing();
	~cSMAntialiasing();

	void DrawSMAA(ID3D11ShaderResourceView * _color);

	ID3D11ShaderResourceView* Get();
private:
	HRESULT CreateSMAATexture();
	cSMAAEdge* m_pEdge;
	cSMAABlendingWeight* m_pBlendingWeight;
	cSMAABlending* m_pBlending;

	ID3D11Texture2D* pAreaTex;
	ID3D11ShaderResourceView* pAreaSRV;

	ID3D11Texture2D* pSearchTex;
	ID3D11ShaderResourceView* pSearchSRV;
};