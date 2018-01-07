#include "ShaderMonitoring.h"
#include "Tessellation.h"
#include "Shader.h"

void cShaderMonitoring::MonitorChanges()
{
#if defined(DEBUG) || defined(_DEBUG)
	static float Cnt = 0.0f;
	m_TimeCnt.TimerEnd();
	Cnt += m_TimeCnt.GetProcessingTime();
	if (Cnt > SHADER_REROAD_TIME_SEC) {

		for (auto itr = m_Data.begin(); itr != m_Data.end(); itr++) {

			//ファイルを開いて時間を取得したら時間を確認しすぐにファイルを閉じている
			itr->vFileHnd = CreateFile(itr->vFileName.c_str(),
				GENERIC_READ | GENERIC_WRITE,
				FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
				NULL,
				OPEN_EXISTING,
				FILE_ATTRIBUTE_NORMAL,
				0);
			if (m_Data[m_Data.size() - 1].vFileHnd == INVALID_HANDLE_VALUE) {
				MessageBox(
					NULL, TEXT("シェーダを監視下に置けません"),
					NULL, MB_OK
				);
			}
			auto newTime = GetFileChangeTime(itr->vFileHnd);
			itr->vFileHnd = NULL;

			if (newTime != itr->vTime) {
				//ここで更新処理をかける

				//テッセレーションの読み込み
				while (FAILED(cTessellationManager::GetInstance().ShaderReload(itr->vFileName))) { //引数でフォルダのパスを完成させてファイル実行
					//エラー。読み込み失敗
					std::string ErrMessage = itr->vFileName + " のテッセレーションビルド失敗。リロードします";
					int res = MessageBox(0, ErrMessage.c_str(), NULL, MB_OKCANCEL);
					//キャンセルされたらループを抜ける
					if (res == IDCANCEL)
						break;
				}

				//通常シェーダの読み込み
				while (FAILED(SHADER::GetInstance()->ShaderReload(itr->vFileName))) { //引数でフォルダのパスを完成させてファイル実行
					 //エラー。読み込み失敗
					std::string ErrMessage = itr->vFileName + " の通常シェーダビルド失敗。リロードします";
					int res = MessageBox(0, ErrMessage.c_str(), NULL, MB_OKCANCEL);
					//キャンセルされたらループを抜ける
					if (res == IDCANCEL)
						break;
				}

				itr->vTime = newTime;		//時間の更新
			}
		}
		Cnt = 0.0f;

	}
	m_TimeCnt.TimerStart();
#endif
}

void cShaderMonitoring::AddFileDate(std::string _FileName)
{
#if defined(DEBUG) || defined(_DEBUG)
	ShaderMonitStruct AddData;
	AddData.vFileName = _FileName;

	char szFullPath[256] = { "\0" };
	char* szFilePath = nullptr;
	const char* path = (_FileName).c_str();

	_fullpath(szFullPath, path, sizeof(szFullPath) / sizeof(szFullPath[0]));

	std::string AllPath = std::string(szFullPath);

	m_Data.push_back(AddData);

	//まずはハンドルの作成を行う
	m_Data[m_Data.size() - 1].vFileHnd = CreateFile(AllPath.c_str(),
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		0);
	if (m_Data[m_Data.size() - 1].vFileHnd == INVALID_HANDLE_VALUE) {
		MessageBox(
			NULL, TEXT("シェーダを監視下に置けません"),
			NULL, MB_OK
		);
	}

	m_Data[m_Data.size() - 1].vTime = GetFileChangeTime(m_Data[m_Data.size() - 1].vFileHnd);
	m_Data[m_Data.size() - 1].vFileHnd = NULL;

#endif
}

cShaderMonitoring::~cShaderMonitoring()
{
#if defined(DEBUG) || defined(_DEBUG)
	for (auto itr = m_Data.begin(); itr != m_Data.end(); itr++) {
		if (itr->vFileHnd != NULL)
			CloseHandle(itr->vFileHnd);
	}
#endif
}

DWORD cShaderMonitoring::GetFileChangeTime(HANDLE fileHnd)
{
#if defined(DEBUG) || defined(_DEBUG)
	FILETIME buf;
	GetFileTime(fileHnd, NULL, NULL, &buf);
	CloseHandle(fileHnd);		//ハンドルを閉じる
	return buf.dwLowDateTime;
#else
	return 0;
#endif
}
