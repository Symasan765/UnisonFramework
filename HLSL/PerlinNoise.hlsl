/*=================================================
//								FILE : PerlinNoise.hlsl
//		�t�@�C������ :
//		�p�[�����m�C�Y�ŗ��p����֐����`����K�v
//
//							HAL��� : ���{ �Y�V��
=================================================*/
#pragma once

//5���֐�
float FifthOrderFunction(float pos)
{
    float t = abs(pos);		//��Βl�ɒ����Ă���
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
	//���̊֐��ōs������
	//1.���WXY�ŃE�F�[�u���b�g�֐��v�Z���s��
	//2.���W����K���Ȍ��z�����߂�
	//3.���ꂼ����������ă��^�[������
    
	 float _0 = FifthOrderFunction(rate) * (getGrad(min, 10, 10) * rate);
    float _1 = FifthOrderFunction((-1.0f + rate)) * (getGrad(max, 10, 10) * (-1.0f + rate));
   
    
    return lerp(_0,_1,rate);
}

float WaveletNoise(float2 pos, float freq, float lattice)
{
    float2 origin = abs(pos) * lattice;
    float2 min = origin / freq;
    min = floor(min) * freq;		//����ł��̍��W�����݂���i�q��min���W�����߂邱�Ƃ��o����
    float2 max = min + freq;	//�����max���W

    float2 rate = frac(origin / freq) + floor(origin / max);
  
    float x0 = Wavelet((float2) (min), (float2) (max.x, min.y),rate.x);
    float x1 = Wavelet((float2) (min.x,max.y), (float2) (max), rate.x);

    float y0 = Wavelet((float2) (min), (float2) (min.x, max.y), rate.y);
    float y1 = Wavelet((float2) (max.x, min.y), (float2) (max), rate.y);
    
    //return lerp(lerp(x0, x1, rate.y), lerp(y0, y1, rate.x), rate.y);
    return ((lerp(x0, x1, rate.y) * lerp(y0, y1, rate.x))) * 40.0f;
}

//pos : �m�C�Y�v�Z���W  freq : ���g��, compNum : ������
float PerlinNoise(float2 pos, float freq, float compNum)
{
    float pn = 0.0f;		//�p�[�����m�C�Y�{��

	//���Z���s���̂ŌJ��Ԃ������̃J�E���^���̂�float�Ƃ��Ē�`���Ă���
    for (float i = 1.0f; i <= compNum; i += 1.0f)
    {
        pn += (1.0f / (i)) * WaveletNoise(pos, freq, pow(2.0f, compNum - (i)));
    }
	
    return (pn + 1.0f) / 2.0f;
}