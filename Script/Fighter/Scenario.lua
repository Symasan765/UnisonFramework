-- �Q�[�����ŌĂяo�����Z���t

--�����Ńe�L�X�g��`�悷��
function WhiteSerif(text,FrameCnt,color)
    --�w��t���[�����̊ԁA������`�悷��
    for i = 1,FrameCnt do
        coroutine.yield(text,color)
    end
end

function DefaultSerif()
    WhiteSerif("����̓p�^�[��1�̃Z���t�ł�",200,"White")
    WhiteSerif("�Q�[���̐i�s�󋵂ɍ��킹��",400,"Blue")
    WhiteSerif("�Z���t��؂�ւ����܂�",100,"Red")
    WhiteSerif("�G�ɂ��ꂽ��G��|������",80,"Green")
    WhiteSerif("�F�X�ȏ󋵂Ő؂�ւ����܂���",400,"Green")
    return "����ł͎��̃X�N���v�g�֐��Ɉڂ�܂�",300,"White"
end

function SpecialSerif()
    WhiteSerif("����̓p�^�[���Q�̃Z���t�ł�",200,"White")
    WhiteSerif("�����̊Ǘ��̓Z���t�����ł͂Ȃ�",400,"Blue")
    WhiteSerif("�J�������[�N�┭�����鉉�o�ɕ֗��ł�",100,"Red")
    WhiteSerif("�����Ƃ����ƌ������グ�Ă��������ł���",400,"Green")
end

function MainScenario()
    WhiteSerif("���ꂩ��o�Ă���Z���t�͗����Ő؂�ւ��܂�",200,"White")
    WhiteSerif("�܂�����������X�N���v�g�Ŏ��R��",200,"White")
    WhiteSerif("�ݒ肷�邱�Ƃ��o���܂���",200,"White")
    WhiteSerif("����ł͗����ŃZ���t�̐؂�ւ����e�X�g���܂�",200,"White")
    local a = math.random()
    while true do
        a = math.random()
        if a > 0.5 then
            DefaultSerif()
        elseif a > 0.1 then
            SpecialSerif()
        else
            WhiteSerif("���I���̃Z���t�������Ă����烌�A�ł���I",400,"Blue")
        end
        
    end
    return "End","End"
end