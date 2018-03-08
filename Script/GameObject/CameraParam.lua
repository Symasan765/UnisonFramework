function CameraRotatAngle()
    -- カメラの回転アングルの最大値を返す
    local angle = 1.0
    return angle
end

function CameraDelay()
    -- カメラの遅延数を調整する
    local Delay = 10
    return 10
end

function Offset()
    -- オフセットの値
    local x = 0.0
    local y = 1.5
    local z = -4.0

    return x,y,z
end

function ShortOffset()
    -- オフセットの値
    local sx = 0.0
    local sy = 0.5
    local sz = -1.5

    return sx,sy,sz
end

function MaxRotetAngle()
    -- １フレームで回転する最大角度を設定する
    local maxY = 2.0
    local maxX = 2.0
    return maxY,maxX
end

function CameraSpeed()
    --カメラが回る際に初期速度、１フレームの回る最大速度と加速度を返す
    local init = 0.2
    local accel = 1.1
    return init , accel
end

function CameraDeceleration()
    --減速のパラメータを調整する
    local Dece = 0.7
    return Dece
end

function RotAngleLimit()
    local limitX = 22.0
    local DownX = -24.0
    return limitX , DownX
end