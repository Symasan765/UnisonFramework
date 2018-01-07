/*=================================================
//								FILE : CompileOptions.h
//		�t�@�C������ :
//		�V�F�[�_���R���p�C������ۂ̃I�v�V�������w�肵�Ă���
//
//							HAL��� : ���{ �Y�V��
=================================================*/
#pragma once
#include "WinMain.h"

namespace CompileOptions {
	// �V�F�[�_�̃R���p�C�� �I�v�V����
#if defined(DEBUG) || defined(_DEBUG)
	constexpr  UINT g_flagCompile = D3D10_SHADER_DEBUG | D3D10_SHADER_SKIP_OPTIMIZATION
		| D3D10_SHADER_ENABLE_STRICTNESS | D3D10_SHADER_PACK_MATRIX_COLUMN_MAJOR;
#else
	constexpr UINT g_flagCompile = D3D10_SHADER_ENABLE_STRICTNESS | D3D10_SHADER_PACK_MATRIX_COLUMN_MAJOR;
#endif
}