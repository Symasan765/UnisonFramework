-- プレイヤーの速度に関するパラメータを返す
function RetMoveSpeed()
    local speed = 0.2   --フレームあたりの最大速度
    return 0.2
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