-- 敵の動きをロードするスクリプトを記述する
function EnemyBullet(sTime,eTime,x,y)
    for i = 1,sTime do
        -- 弾チャージ演出
        coroutine.yield("Charge",x,y) --チャージ中
    end

    -- 弾発射呼び出し
    coroutine.yield("Firing",x,y)      --撃った瞬間

    --弾発射後の待ち時間
    for i = 1,eTime do
        coroutine.yield("Heat",x,y)     --放熱
    end
end

function EnemyLerp(startX,startY,endX,endY,frame)
    -- 1フレーム毎にどれだけタイム値を進めるか？
    local add = 1.0 / frame
    local x,y

    for t = 0.0, 1.0, add do
        --敵が一定時間待機してから発射命令を出す
        x = (endX - startX) * t + startX
        y = (endY - startY) * t + startY
        coroutine.yield("Move",x,y) --移動中
    end
    coroutine.yield("Arrival",x,y)  --到着
end

function EnemyMove(No)
    while true do
    --こいつらに何かしらの方法で位置を設定する
    local StartPosX,StartPosY,StartFrame,StartWait = Enemies:GetStart(No)
    local MidX,MidY,ShotLanch,Rigor = Enemies:GetBullet(No) --中間
    local endX,endY,endFrame = Enemies:GetEnd(No) --最終

    EnemyLerp(StartPosX,StartPosY,MidX,MidY,StartFrame)
    EnemyBullet(ShotLanch,Rigor,MidX,MidY)
    EnemyLerp(MidX,MidY,endX,endY,endFrame)

    coroutine.yield("end",0,0)  --一連の動作終了
    end
end