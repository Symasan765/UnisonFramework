-- プレイヤーが操作するキャラクターの動作を定義する

require('Script/Fighter/PlayerParam')


-- 通常移動処理
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

-- ローリングの処理
function Rolling()
    local vec = 0   -- 進行方向を保持する
    local a = 0.94   -- 加速度
    local speed = 9.0
    local rotDgr = 0.0
    for i = 1,RollingTime do
        rotDgr = vec * -20.0
        -- 進行方向がない場合のみ一度だけキー操作を受け付ける
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
        PlayerMove(vec * speed)  -- 回転中の移動処理
        transform:Rotation(0,0,rotDgr)         -- 回転処理。
        speed = speed * a   --加減速処理
        BulletJudge()       --球の射出処理
        coroutine.yield(0)
    end
    speed = 5.0
    return 1
end

-- ダメージ状態の時の処理
function Damage()

end