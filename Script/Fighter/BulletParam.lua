-- 弾に関するパラメータや処理を記述する

-- プレイヤーの弾の情報を返す関数。
function ExportParam()
    local ChargeTime = 30   -- 弾のリチャージタイム
    local BulletSpeed = 15.0 -- 弾の1フレームの速度

    return ChargeTime,BulletSpeed
end

function EnemyExportParam()
    local BulletSpeed = -1.0 -- 弾の1フレームの速度

    return BulletSpeed
end