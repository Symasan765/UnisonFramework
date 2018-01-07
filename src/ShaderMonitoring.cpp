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

			//�t�@�C�����J���Ď��Ԃ��擾�����玞�Ԃ��m�F�������Ƀt�@�C������Ă���
			itr->vFileHnd = CreateFile(itr->vFileName.c_str(),
				GENERIC_READ | GENERIC_WRITE,
				FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
				NULL,
				OPEN_EXISTING,
				FILE_ATTRIBUTE_NORMAL,
				0);
			if (m_Data[m_Data.size() - 1].vFileHnd == INVALID_HANDLE_VALUE) {
				MessageBox(
					NULL, TEXT("�V�F�[�_���Ď����ɒu���܂���"),
					NULL, MB_OK
				);
			}
			auto newTime = GetFileChangeTime(itr->vFileHnd);
			itr->vFileHnd = NULL;

			if (newTime != itr->vTime) {
				//�����ōX�V������������

				//�e�b�Z���[�V�����̓ǂݍ���
				while (FAILED(cTessellationManager::GetInstance().ShaderReload(itr->vFileName))) { //�����Ńt�H���_�̃p�X�����������ăt�@�C�����s
					//�G���[�B�ǂݍ��ݎ��s
					std::string ErrMessage = itr->vFileName + " �̃e�b�Z���[�V�����r���h���s�B�����[�h���܂�";
					int res = MessageBox(0, ErrMessage.c_str(), NULL, MB_OKCANCEL);
					//�L�����Z�����ꂽ�烋�[�v�𔲂���
					if (res == IDCANCEL)
						break;
				}

				//�ʏ�V�F�[�_�̓ǂݍ���
				while (FAILED(SHADER::GetInstance()->ShaderReload(itr->vFileName))) { //�����Ńt�H���_�̃p�X�����������ăt�@�C�����s
					 //�G���[�B�ǂݍ��ݎ��s
					std::string ErrMessage = itr->vFileName + " �̒ʏ�V�F�[�_�r���h���s�B�����[�h���܂�";
					int res = MessageBox(0, ErrMessage.c_str(), NULL, MB_OKCANCEL);
					//�L�����Z�����ꂽ�烋�[�v�𔲂���
					if (res == IDCANCEL)
						break;
				}

				itr->vTime = newTime;		//���Ԃ̍X�V
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

	//�܂��̓n���h���̍쐬���s��
	m_Data[m_Data.size() - 1].vFileHnd = CreateFile(AllPath.c_str(),
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		0);
	if (m_Data[m_Data.size() - 1].vFileHnd == INVALID_HANDLE_VALUE) {
		MessageBox(
			NULL, TEXT("�V�F�[�_���Ď����ɒu���܂���"),
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
	CloseHandle(fileHnd);		//�n���h�������
	return buf.dwLowDateTime;
#else
	return 0;
#endif
}
