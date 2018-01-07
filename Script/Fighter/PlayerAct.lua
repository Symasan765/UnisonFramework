-- �v���C���[�����삷��L�����N�^�[�̓�����`����

require('Script/Fighter/PlayerParam')


-- �ʏ�ړ�����
function NormalMove()
    local x,z = GamePad:LeftStick()
    PlayerMove(x * 2.0)
    transform:SetRotate(0,0,-x * 35.0)

    if GamePad:Trigger(XKey) then
        return 2
    end

    BulletJudge()
    return 0
end

-- ���[�����O�̏���
function Rolling()
    local vec = 0   -- �i�s������ێ�����
    local a = 0.94   -- �����x
    local speed = 9.0
    local rotDgr = 0.0
    for i = 1,RollingTime do
        rotDgr = vec * -20.0
        -- �i�s�������Ȃ��ꍇ�݈̂�x�����L�[������󂯕t����
        if vec == 0 then
            rotDgr = 35.0
            local x,y = GamePad:LeftStick()
            if(x > 0.0) then
                vec = 1
            end
            if(x < 0.0) then
                vec = -1
            end
        end
        PlayerMove(vec * speed)  -- ��]���̈ړ�����
        transform:Rotation(0,0,rotDgr)         -- ��]�����B
        speed = speed * a   --����������
        BulletJudge()       --���̎ˏo����
        coroutine.yield(0)
    end
    speed = 5.0
    return 1
end

-- �_���[�W��Ԃ̎��̏���
function Damage()

end