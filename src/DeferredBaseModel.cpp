/*=================================================
//								FILE : DeferredBaseModel.cpp
//		ファイル説明 :
//		ディファードレンダリングで利用することを想定した
//		メッシュモデルを取り扱うクラス
//							HAL大阪 : 松本 雄之介
=================================================*/
#include "DeferredBaseModel.h"
#include "LuaConvert.h"
using namespace LuaConv;

bool cDeferredModel::m_DefaultRenderFlag = false;

/// <summary>
/// 初期化
/// </summary>
cDeferredModel::cDeferredModel()
{
	//初期化処理
	m_ShaderType = 0;
	m_Constant.SetWorldMatrix(GetWorldMatrix());
	m_pTesse = nullptr;
}

/// <summary>
/// メッシュデータをロードする
/// </summary>
/// <param name="_fileName"></param>
/// <param name="_type"></param>
void cDeferredModel::LoadData(std::string _fileName, int _type)
{
	//メッシュとシェーダをロードする
	m_pMesh = MeshManager::GetInstance().LoadMeshData(_fileName);
	m_ShaderType = _type;		//渡された値を整数値として保存
}

/// <summary>
/// メッシュデータを描画する
/// </summary>
/// <param name="_anmNo"></param>
/// <param name="_time"></param>
void cDeferredModel::DrawMesh(int _anmNo, const float _time)
{
	//m_pMesh->Draw();		//標準の描画使うとシェーダセットが出来ないから使わない方向かな…？

	SetConstant();

	//バーテックスバッファーをセット（バーテックスバッファーは一つでいい）
	UINT stride = sizeof(VERTEX3D);
	UINT offset = 0;
	GetDirectX::Context()->IASetVertexBuffers(0, 1, &m_pMesh->vVertexBuf, &stride, &offset);

	//	アニメーション情報を持っていたらシェーダーにセットする
	if (m_pMesh->vpAnimetion != nullptr) {
		this->m_pMesh->vpAnimetion->SetBoneConst(_anmNo, _time);
		// この関数自体にデフォルト引数にてアニメーション番号を取得できるようにしておく
		//タイマーを設定できるようにしておくか自動で行う。
	}

	//マテリアルの数だけ、それぞれのマテリアルのインデックスバッファ－を描画
	for (int i = 0; i < m_pMesh->vMaterialCnt; i++)
	{
		//マテリアル情報をセットする
		m_Constant.SetMaterial(m_pMesh->vpMaterial[i].Ka, m_pMesh->vpMaterial[i].Kd, m_pMesh->vpMaterial[i].Ks);

		//使用されていないマテリアル対策
		if (m_pMesh->vpMaterial[i].dwNumFace == 0)
		{
			continue;
		}

		GetDirectX::Context()->IASetIndexBuffer(m_pMesh->vppIndexBuffer[i], DXGI_FORMAT_R32_UINT, 0);

		//テクスチャーをシェーダーに渡す
		if (m_pMesh->vpMaterial[i].pDefaultTex != NULL)
		{
			//通常レンダリング時のみテクスチャをセットする。深度マップ作成時は不要
			if (m_DefaultRenderFlag) {
				GetDirectX::Context()->PSSetShaderResources(0, 1, &m_pMesh->vpMaterial[i].pDefaultTex->vpTexture);
				GetDirectX::Context()->PSSetShaderResources(1, 1, &m_pMesh->vpMaterial[i].pShadowTex->vpTexture);
				GetDirectX::Context()->PSSetShaderResources(2, 1, &m_pMesh->vpMaterial[i].pLightTex->vpTexture);
				GetDirectX::Context()->PSSetShaderResources(3, 1, &m_pMesh->vpMaterial[i].pMaterialMaskTex->vpTexture);
			}
			else {
				GetDirectX::Context()->PSSetShaderResources(NULL, NULL, NULL);
			}
		}

		m_Constant.SetShader(true);		//マテリアル情報定数をシェーダのセットする

										//テッセレーションがある場合だけセットする
		if (m_pTesse) {
			m_pTesse->SetTessellation();
			//プリミティブ・トポロジーをセット
			GetDirectX::Context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);
		}
		else {
			cTessellationManager::GetInstance().SetTessellationToNull();
			//プリミティブ・トポロジーをセット
			GetDirectX::Context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		}
		//Draw
		GetDirectX::Context()->DrawIndexed(m_pMesh->vpMaterial[i].dwNumFace * 3, 0, 0);
	}
	GetDirectX::Context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	cTessellationManager::GetInstance().SetTessellationToNull();
}

/// <summary>
/// 自発光情報セット
/// </summary>
/// <param name="_color"></param>
void cDeferredModel::SetEmission(DirectX::XMFLOAT4 _color)
{
	m_Constant.SetEmission(_color);
}

void cDeferredModel::SetShaderType(int _type)
{
	m_ShaderType = _type;		//渡された値を整数値として保存
}

/// <summary>
/// テッセレーションステージを使用する場合追加で読み込む
/// </summary>
/// <param name="_FileName"></param>
void cDeferredModel::LoadTessellation(std::string _FileName)
{
	m_pTesse = cTessellationManager::GetInstance().LoadTessellationData(_FileName);
}

const Mesh3D & cDeferredModel::GetMeshData() const
{
	return *m_pMesh;
}

void cDeferredModel::DefaultRenderFlag(bool flag)
{
	m_DefaultRenderFlag = flag;
}

/// <summary>
/// Luaにバインドする
/// </summary>
/// <param name="L"></param>
/// <param name="LuaVarName"></param>
void cDeferredModel::AddFunctionToLua(lua_State * L, std::string LuaVarName)
{
	const luaL_Reg funcs[] = {
		{ "Scaling", cMatrix::ScalingGlue },
		{ "Translation", cMatrix::TranslationGlue },
		{ "Rotation", cMatrix::RotationGlue },
		{ "SetScaling", cMatrix::SetScalingGlue },
		{ "SetRotate", cMatrix::SetRotateGlue },
		{ "SetTrans", cMatrix::SetTransGlue },
		{ "VectMove", cMatrix::VectMoveGlue },
		{ "GetPosition", cMatrix::GetPositionGlue },
		{ "Draw", cDeferredModel::DrawGlue },
		{ NULL, NULL }
	};

	SetLuaGlobalObjct<cDeferredModel>(L, LuaVarName.c_str(), funcs, this);
}

/// <summary>
/// 定数をシェーダにセットする
/// </summary>
void cDeferredModel::SetConstant()
{
	//ワールド行列を保存
	m_Constant.SetWorldMatrix(GetWorldMatrix());
	m_Constant.SetShaderType(m_ShaderType);
	m_Constant.SetShader();
}

/// <summary>
/// Luaを使用するためのGlue関数
/// </summary>
/// <param name="L"></param>
/// <returns></returns>
int cDeferredModel::DrawGlue(lua_State * L)
{
	//テーブルからアドレスを持ってくる
	auto p = toPointer<cDeferredModel>(L);


	//関数実行
	p->DrawMesh();
	lua_settop(L, 0);	//関数呼び出し用スタックは戻り値では戻り値以外は削除して問題ない

	return 0;		//戻り値
}
