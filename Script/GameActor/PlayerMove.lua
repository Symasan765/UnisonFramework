-- �v���C���[�̑��x�Ɋւ���p�����[�^��Ԃ�
function RetMoveSpeed()
    local walk = 0.002   --�t���[��������̍ő呬�x
    local dash = 0.004
    return walk , dash
end

function RetPlazaClamp()
    --�������W
    local LDX = -37.0
    local LDY = 1.0
    --�E����W
    local RUX = 42.0
    local RUY = 33.0

    return LDX,LDY,RUX,RUY
end

function RetAisleClamp()
    --�������W
    local LDX = -37.0
    local LDY = -45.0
    --�E����W
    local RUX = -18.5
    local RUY = 1.0

    return LDX,LDY,RUX,RUY
end

-- ��]�Ɋւ���p�����[�^��Ԃ�
function RetRotateData()
    return 5.0
end

function MaxSpeed()
    local walkMax = 0.2
    local dashMax = 0.4
    local accele = 1.1
    local Decele = 0.9
    return walkMax,dashMax,accele,Decele
end

function Switching()
    -- �����ƃ_�b�V���̐؂�ւ��l(0 ~ 1)
    local swi = 0.5
    return swi
end

function MaxRotatAngle()
    -- �P�t���[��������̍ő��]�p�x
    local MaxAngle = 5.0
    return MaxAngle
end