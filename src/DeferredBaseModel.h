/*=================================================
//								FILE : DeferredBaseModel.h
//		ファイル説明 :
//		ディファードレンダリングで利用するモデルデータを包括的に取り扱う
//
//							HAL大阪 : 松本 雄之介
=================================================*/
#pragma once
#include <string>
#include "Mesh3D.h"
#include "MatrixCoord.h"
#include "DeferredConst.h"
#include "DeferredRendering.h"
#include "Tessellation.h"

/// <summary>
/// 基本的に初期化にLoadData、更新処理を行ってから
/// DrawMeshを呼び出せばOK。
/// モデルによって自発光を設定
/// </summary>
class cDeferredModel : public cMatrix {
public:
	cDeferredModel();
	~cDeferredModel() = default;
	void LoadData(std::string _fileName, int _type);	//typeはShaderTypeをビット演算する
	void DrawMesh(int _anmNo = 0, const float _time = 0);
	void SetEmission(DirectX::XMFLOAT4 _color);	//自発光色を設定する
	void SetShaderType(int _type);	//typeはShaderTypeをビット演算するといい
	void LoadTessellation(std::string _FileName);
	const Mesh3D& GetMeshData()const;
	static void DefaultRenderFlag(bool flag);	//true : 通常レンダリング false : シャドウレンダリング
	void AddFunctionToLua(lua_State* L, std::string LuaVarName);
private:
	void SetConstant();		//保存されたデータを定数バッファに設定する

	static int DrawGlue(lua_State* L);
	std::shared_ptr<Mesh3D> m_pMesh;
	std::shared_ptr<TessellationStruct> m_pTesse;
	cDeferredConst m_Constant;		//定数バッファ
	int m_ShaderType;						//ビット演算したシェーダ番号を格納する

	static bool m_DefaultRenderFlag;		//通常レンダリングフラグ。シャドウマップ作成時にテクスチャをセットしない管理に使用
};