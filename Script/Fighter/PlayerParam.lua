-- �v���C���[�̃p�����[�^�̐ݒ���s��

RollingTime = 30    -- ���[�����O�ɂ�����t���[����

function Init()
    transform:SetTrans(0.0,25.0,-60.0)
end

function RollingInterpolation(t)
    t = 1.0 - t
    return t * t
    --return (t * t) * (3 - 2 * t)
end

-- �v���C���[�̈ړ�����������B
function PlayerMove(moveX)
    MoveRange = 205.0
    local x,y,z = transform:GetPosition()
    -- �ړ��͈͂𐧌�����
    x = x + moveX
    if x > -MoveRange and x < MoveRange then
        transform:SetTrans(x,y,z)
    end
end

function BulletJudge()
    if GamePad:Trigger(AKey) then
        local posx , posY, posz = transform:GetPosition()
        Bullet:PlayerFiring(posx,posY,posz)
    end
end