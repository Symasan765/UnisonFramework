-- �g�D�[�������_�����O�̃p�����[�^��ݒ肷��
function LightingParam()
    -- �n�[�t�����o�[�g�̏Ɩ��v�Z�l�Ŕ���
    -- LightBorder����ShadowBorder�̊Ԃ͐��`��Ԃ�����
    local LightBorder = 0.11
    local ShadowBorder = 0.08

    return LightBorder,ShadowBorder
end

function OutlineParam()
    -- �֊s���ɑ΂���p�����[�^
    local offsetScall = 1.0     -- �𑜓x�Ŋ���l�B���l���傫���Ƃ��ꂾ�������̃e�N�Z�����֊s���ƂȂ�
    local DepthRange = 0.1     -- �֊s���Ƃ��Ď�舵���T�C�Y�B���̑����ł͂Ȃ��֊s�����肷��͈�(�[�x)
    local NormalRange = 1.0     -- �֊s���Ƃ��Ď�舵���T�C�Y�B���̑����ł͂Ȃ��֊s�����肷��͈�(�@��)
    local OutlineSum = 0.3      -- ���͂̐F����Z�o�����֊s���ɂ�����l(�������Ȃ�΍����Ȃ�)

    return offsetScall,DepthRange,NormalRange,OutlineSum
end

function CookTorranceParam()
    -- �N�b�N�g�����X�K�p����HDR�����ŏƖ��x�����Z����l�Ƃ��̌�ώZ����l
    local HighlightSub = 1.5
    local HighlightSum = 30.0

    return HighlightSub,HighlightSum
end