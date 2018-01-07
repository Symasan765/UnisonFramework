-- �G�̓��������[�h����X�N���v�g���L�q����
function EnemyBullet(sTime,eTime,x,y)
    for i = 1,sTime do
        -- �e�`���[�W���o
        coroutine.yield("Charge",x,y) --�`���[�W��
    end

    -- �e���ˌĂяo��
    coroutine.yield("Firing",x,y)      --�������u��

    --�e���ˌ�̑҂�����
    for i = 1,eTime do
        coroutine.yield("Heat",x,y)     --���M
    end
end

function EnemyLerp(startX,startY,endX,endY,frame)
    -- 1�t���[�����ɂǂꂾ���^�C���l��i�߂邩�H
    local add = 1.0 / frame
    local x,y

    for t = 0.0, 1.0, add do
        --�G����莞�ԑҋ@���Ă��甭�˖��߂��o��
        x = (endX - startX) * t + startX
        y = (endY - startY) * t + startY
        coroutine.yield("Move",x,y) --�ړ���
    end
    coroutine.yield("Arrival",x,y)  --����
end

function EnemyMove(No)
    while true do
    --������ɉ�������̕��@�ňʒu��ݒ肷��
    local StartPosX,StartPosY,StartFrame,StartWait = Enemies:GetStart(No)
    local MidX,MidY,ShotLanch,Rigor = Enemies:GetBullet(No) --����
    local endX,endY,endFrame = Enemies:GetEnd(No) --�ŏI

    EnemyLerp(StartPosX,StartPosY,MidX,MidY,StartFrame)
    EnemyBullet(ShotLanch,Rigor,MidX,MidY)
    EnemyLerp(MidX,MidY,endX,endY,endFrame)

    coroutine.yield("end",0,0)  --��A�̓���I��
    end
end