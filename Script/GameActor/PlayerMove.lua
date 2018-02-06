-- プレイヤーの速度に関するパラメータを返す
function RetMoveSpeed()
    local walk = 0.002   --フレームあたりの最大速度
    local dash = 0.004
    return walk , dash
end

function RetPlazaClamp()
    --左下座標
    local LDX = -37.0
    local LDY = 1.0
    --右上座標
    local RUX = 42.0
    local RUY = 33.0

    return LDX,LDY,RUX,RUY
end

function RetAisleClamp()
    --左下座標
    local LDX = -37.0
    local LDY = -45.0
    --右上座標
    local RUX = -18.5
    local RUY = 1.0

    return LDX,LDY,RUX,RUY
end

-- 回転に関するパラメータを返す
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
    -- 歩きとダッシュの切り替わる値(0 ~ 1)
    local swi = 0.5
    return swi
end

function MaxRotatAngle()
    -- １フレーム当たりの最大回転角度
    local MaxAngle = 5.0
    return MaxAngle
end