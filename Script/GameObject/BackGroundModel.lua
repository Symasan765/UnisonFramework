function FenceDraw()
    -- Fence:SetTrans(0 , 0 , 0)
    -- Fence:SetRotate(0 , 0 , 0)
    -- Fence:Draw()

    --�X�^�[�g�n�_�̈�Ԓ�ӕ����̃t�F���X
    for i = 1,5 do
        Fence:SetTrans(-22 - (i - 1) * 4, 0 , -41)
        Fence:Draw()
    end

    for i = 1,20 do
        Fence:SetTrans(38 - (i - 1) * 4, 0 , 41)
        Fence:Draw()
    end

    for i = 1,15 do
        Fence:SetTrans(38 - (i - 1) * 4, 0 , 8)
        Fence:Draw()
    end

    Fence:SetRotate(0 , 90 , 0)

    for i = 1,8 do
        Fence:SetTrans(40, 0 , 10 + (i - 1) * 4)
        Fence:Draw()
    end

    for i = 1,12 do
        Fence:SetTrans(-20, 0 , -39 + (i - 1) * 4)
        Fence:Draw()
    end

    for i = 1,20 do
        Fence:SetTrans(-41, 0 , -39 + (i - 1) * 4)
        Fence:Draw()
    end

    Fence:SetRotate(0 , 0 , 0)
end


function HouseDraw()
    --�܂��������̉Ƃ�\��
    House:SetRotate(0 , -90 , 0)
    House:SetTrans(-20, 0 , -34)
    House:Draw()

    House:SetTrans(-20, 0 , -24)
    House:Draw()

    House:SetTrans(-20, 0 , -14)
    House:Draw()

    House:SetTrans(-20, 0 , -4)
    House:Draw()

    House:SetRotate(0 , 90 , 0)

    offsetX = -41

    House:SetTrans(offsetX, 0 , -34)
    House:Draw()

    House:SetTrans(offsetX, 0 , -24)
    House:Draw()

    House:SetTrans(offsetX, 0 , -14)
    House:Draw()

    House:SetTrans(offsetX, 0 , -4)
    House:Draw()
end

function CityDraw()
    City:SetScaling(0.01)
    City:SetTrans(-50, 0 , -11)
    City:Draw()
end