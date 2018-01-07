/*=================================================
//								FILE : Camera.h
//		ファイル説明 :
//		ゲームシーン内で取り扱うメインカメラを実装するクラス
//
//							HAL大阪 : 松本 雄之介
=================================================*/
#pragma once
#include "WinMain.h"
#include "ConstantBuffer.h"
#include <lua.hpp>
#include <string>

/// <summary>
/// カメラのプロジェクションに必要なデータをまとめた構造体
/// </summary>
struct CameraData {
	DirectX::XMFLOAT3 vPos;			//カメラ座標
	DirectX::XMFLOAT3 vLook;		//注視点
	DirectX::XMFLOAT3 vUp;			//上方向ベクトル
	float vNear;
	float vFar;
};

/// <summary>
/// カメラのビューとプロジェクションを管理する
/// </summary>
struct ViewProj {
	DirectX::XMFLOAT4X4 vView;
	DirectX::XMFLOAT4X4 vProj;
	DirectX::XMFLOAT4X4 vVP;		//上記二つの合成
};

/// <summary>
/// ゲームシーン内で管理されるカメラを定義する。
/// </summary>
class cCamera {
public:
	friend class cSceneManager;		//プロジェクションをシーンマネージャ内に限定するため
	cCamera();
	~cCamera();
	void SetPosition(const DirectX::XMFLOAT3& _pos);
	void SetUpVect(const DirectX::XMFLOAT3& _Up);
	void SetLookPoint(const DirectX::XMFLOAT3& _look);
	void SetCameraAllData(const CameraData& _data);
	void SetNear(const float Near,const float Far);

	void SetConstBuffer(const int pass,bool debugFlag = false);
	CameraData GetCameraData(bool _FreeCameraFlag);

	void VectMove(const float rate);
	void LookPointRotate(const float rotX, const float rotY, const float rotZ);
	const DirectX::XMFLOAT3 GetAxisZ()const;

	void AddFunctionToLua(lua_State* L, std::string LuaVarName = "Camera");
private:
	const ViewProj& Projection(bool _FreeCameraFlag = false);		//プロジェクションはどこでも出来るようにしない
	void FreeCameraOperation();			//フリーカメラの操作を行う。シーンから呼ぶことがあってはいけないのでプライベート化
	CameraData m_CameraData;			//通常のレンダリングに利用
	CameraData m_DebugCameraData;	//デバッグ中のフレーカメラ機能で利用
	ViewProj m_CameraVP;					//通常のレンダリングで使用したVPを保持
	ViewProj m_DebugVP;						//フリーカメラで利用したVPを保持
	cConstBuf<DirectX::XMFLOAT4> m_ShaderConst;
	// TODO 将来的には視錐台表示用のstaticな頂点バッファなどを作る

	//======================== Lua用Glue関数 ===============================
	static int VectMoveGlue(lua_State* L);
	static int LookRotGlue(lua_State* L);
	static int SetPosGlue(lua_State* L);
	static int SetLookGlue(lua_State* L);
	static int SetUpGlue(lua_State* L);
};