-- プレイヤーのパラメータの設定を行う

RollingTime = 30    -- ローリングにかかるフレーム数

function Init()
    transform:SetTrans(0.0,25.0,-60.0)
end

function RollingInterpolation(t)
    t = 1.0 - t
    return t * t
    --return (t * t) * (3 - 2 * t)
end

-- プレイヤーの移動を処理する。
function PlayerMove(moveX)
    MoveRange = 205.0
    local x,y,z = transform:GetPosition()
    -- 移動範囲を制限する
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