-- �v���C���[�̑��x�Ɋւ���p�����[�^��Ԃ�
function RetMoveSpeed()
    local speed = 0.2   --�t���[��������̍ő呬�x
    return 0.2
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