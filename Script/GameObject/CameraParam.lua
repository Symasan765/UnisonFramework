function CameraRotatAngle()
    -- �J�����̉�]�A���O���̍ő�l��Ԃ�
    local angle = 1.0
    return angle
end

function CameraDelay()
    -- �J�����̒x�����𒲐�����
    local Delay = 10
    return 10
end

function Offset()
    -- �I�t�Z�b�g�̒l
    local x = 0.0
    local y = 1.5
    local z = -4.0

    return x,y,z
end

function ShortOffset()
    -- �I�t�Z�b�g�̒l
    local sx = 0.0
    local sy = 0.5
    local sz = -1.5

    return sx,sy,sz
end

function MaxRotetAngle()
    -- �P�t���[���ŉ�]����ő�p�x��ݒ肷��
    local maxY = 2.0
    local maxX = 2.0
    return maxY,maxX
end

function CameraSpeed()
    --�J���������ۂɏ������x�A�P�t���[���̉��ő呬�x�Ɖ����x��Ԃ�
    local init = 0.2
    local accel = 1.1
    return init , accel
end

function CameraDeceleration()
    --�����̃p�����[�^�𒲐�����
    local Dece = 0.7
    return Dece
end

function RotAngleLimit()
    local limitX = 22.0
    local DownX = -24.0
    return limitX , DownX
end