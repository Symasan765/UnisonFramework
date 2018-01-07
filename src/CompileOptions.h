/*=================================================
//								FILE : CompileOptions.h
//		ファイル説明 :
//		シェーダをコンパイルする際のオプションを指定しておく
//
//							HAL大阪 : 松本 雄之介
=================================================*/
#pragma once
#include "WinMain.h"

namespace CompileOptions {
	// シェーダのコンパイル オプション
#if defined(DEBUG) || defined(_DEBUG)
	constexpr  UINT g_flagCompile = D3D10_SHADER_DEBUG | D3D10_SHADER_SKIP_OPTIMIZATION
		| D3D10_SHADER_ENABLE_STRICTNESS | D3D10_SHADER_PACK_MATRIX_COLUMN_MAJOR;
#else
	constexpr UINT g_flagCompile = D3D10_SHADER_ENABLE_STRICTNESS | D3D10_SHADER_PACK_MATRIX_COLUMN_MAJOR;
#endif
}