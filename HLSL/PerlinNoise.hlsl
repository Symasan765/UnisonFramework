/*=================================================
//								FILE : PerlinNoise.hlsl
//		ファイル説明 :
//		パーリンノイズで利用する関数を定義する必要
//
//							HAL大阪 : 松本 雄之介
=================================================*/
#pragma once

//5次関数
float FifthOrderFunction(float pos)
{
    float t = abs(pos);		//絶対値に直しておく
    return (1.0f - (6.0f * pow(t, 5.0f) - 15.0f * pow(t, 4.0f) + 10.0f * pow(t, 3.0f)));
}

uint xorshift32( uint v ) {
    v = v ^ ( v << 13 );
    v = v ^ ( v >> 17 );
    v = v ^ ( v << 15 );
    return v;
}

float getGrad(float2 coord, int elem, int seed = 0)
{
    uint2 pos = (uint) (coord * 100000.0f);
    uint r = xorshift32(pos.x + seed);
    r = xorshift32(r + pos.y);
    r = xorshift32(r + elem) % 10000;
    
    return (float) (r / 5000.0f - 1.0f);
}


float Wavelet(float2 min, float2 max,float rate)
{
	//この関数で行うこと
	//1.座標XYでウェーブレット関数計算を行う
	//2.座標から適当な勾配を求める
	//3.それぞれを合成してリターンする
    
	 float _0 = FifthOrderFunction(rate) * (getGrad(min, 10, 10) * rate);
    float _1 = FifthOrderFunction((-1.0f + rate)) * (getGrad(max, 10, 10) * (-1.0f + rate));
   
    
    return lerp(_0,_1,rate);
}

float WaveletNoise(float2 pos, float freq, float lattice)
{
    float2 origin = abs(pos) * lattice;
    float2 min = origin / freq;
    min = floor(min) * freq;		//これでその座標が存在する格子のmin座標を求めることが出来た
    float2 max = min + freq;	//これでmax座標

    float2 rate = frac(origin / freq) + floor(origin / max);
  
    float x0 = Wavelet((float2) (min), (float2) (max.x, min.y),rate.x);
    float x1 = Wavelet((float2) (min.x,max.y), (float2) (max), rate.x);

    float y0 = Wavelet((float2) (min), (float2) (min.x, max.y), rate.y);
    float y1 = Wavelet((float2) (max.x, min.y), (float2) (max), rate.y);
    
    //return lerp(lerp(x0, x1, rate.y), lerp(y0, y1, rate.x), rate.y);
    return ((lerp(x0, x1, rate.y) * lerp(y0, y1, rate.x))) * 40.0f;
}

//pos : ノイズ計算座標  freq : 周波数, compNum : 合成回数
float PerlinNoise(float2 pos, float freq, float compNum)
{
    float pn = 0.0f;		//パーリンノイズ本体

	//徐算を行うので繰り返し処理のカウンタ自体もfloatとして定義している
    for (float i = 1.0f; i <= compNum; i += 1.0f)
    {
        pn += (1.0f / (i)) * WaveletNoise(pos, freq, pow(2.0f, compNum - (i)));
    }
	
    return (pn + 1.0f) / 2.0f;
}