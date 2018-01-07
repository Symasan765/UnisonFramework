NO_CHANGE_STATE = 0
RollingState = 2
NOMAL_STATE = 1

function Init()
    Camera:SetLook(0,0,0)
    Camera:SetPos(0,600,0.0)
    Camera:SetUp(0,0,1)
end

function SetEnemyData1()
    Enemies:SetNum(5)

    Enemies:SetStart(0,-200,0,120,50,0)
    Enemies:SetBullet(0,-200,0,20,30,10)
    Enemies:SetEnd(0,-200,0,180,100)

    Enemies:SetStart(1,-100,0,120,70,0)
    Enemies:SetBullet(1,-100,0,20,30,10)
    Enemies:SetEnd(1,-100,0,180,100)

    Enemies:SetStart(2,0,0,120,100,0)
    Enemies:SetBullet(2,0,0,20,30,10)
    Enemies:SetEnd(2,0,0,180,100)

    Enemies:SetStart(3,100,0,120,70,0)
    Enemies:SetBullet(3,100,0,20,30,10)
    Enemies:SetEnd(3,100,0,180,100)

    Enemies:SetStart(4,200,0,120,50,0)
    Enemies:SetBullet(4,200,0,20,30,10)
    Enemies:SetEnd(4,200,0,180,100)
end

function SetEnemyData2()
    Enemies:SetNum(5)

    Enemies:SetStart(4,-200,0,120,50,0)
    Enemies:SetBullet(0,-200,0,20,30,10)
    Enemies:SetEnd(4,-200,0,180,100)

    Enemies:SetStart(3,-100,0,120,70,0)
    Enemies:SetBullet(1,-100,0,20,30,10)
    Enemies:SetEnd(3,-100,0,180,100)

    Enemies:SetStart(2,0,0,120,100,0)
    Enemies:SetBullet(2,0,0,20,30,10)
    Enemies:SetEnd(2,0,0,180,100)

    Enemies:SetStart(1,100,0,120,70,0)
    Enemies:SetBullet(3,100,0,20,30,10)
    Enemies:SetEnd(1,100,0,180,100)

    Enemies:SetStart(0,200,0,120,50,0)
    Enemies:SetBullet(4,200,0,20,30,10)
    Enemies:SetEnd(0,200,0,180,100)
end

function SetEnemyData3()
    Enemies:SetNum(5)

    Enemies:SetStart(0,250,0,0,80,0)
    Enemies:SetBullet(0,180,0,0,30,10)
    Enemies:SetEnd(0,250,0,0,180)

    Enemies:SetStart(1,250,0,20,80,0)
    Enemies:SetBullet(1,100,0,20,30,10)
    Enemies:SetEnd(1,250,0,20,170)

    Enemies:SetStart(2,250,0,40,80,0)
    Enemies:SetBullet(2,0,0,40,30,10)
    Enemies:SetEnd(2,250,0,40,140)

    Enemies:SetStart(3,250,0,60,80,0)
    Enemies:SetBullet(3,-100,0,60,30,10)
    Enemies:SetEnd(3,250,0,60,110)

    Enemies:SetStart(4,250,0,80,80,0)
    Enemies:SetBullet(4,-180,0,80,30,10)
    Enemies:SetEnd(4,250,0,80,80)
end

--ゲーム内の制御を行う。
--基本的に各マネージャにイベントのトリガーとなる情報を
--投げていく処理を記述する。
function SceneControl()
    math.randomseed(os.time())
    while true do
        local flag = Enemies:Has()
        if flag == false then
            local r = math.random()
            if r > 0.7 then
                SetEnemyData1()
            elseif r > 0.3 then
                SetEnemyData2()
            else
                SetEnemyData3()
            end
        end
        coroutine.yield()
    end
end
