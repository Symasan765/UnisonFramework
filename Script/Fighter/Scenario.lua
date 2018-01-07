-- ゲーム内で呼び出されるセリフ

--白字でテキストを描画する
function WhiteSerif(text,FrameCnt,color)
    --指定フレーム数の間、文字を描画する
    for i = 1,FrameCnt do
        coroutine.yield(text,color)
    end
end

function DefaultSerif()
    WhiteSerif("これはパターン1のセリフです",200,"White")
    WhiteSerif("ゲームの進行状況に合わせて",400,"Blue")
    WhiteSerif("セリフを切り替えられます",100,"Red")
    WhiteSerif("敵にやられたり敵を倒したり",80,"Green")
    WhiteSerif("色々な状況で切り替えられますね",400,"Green")
    return "それでは次のスクリプト関数に移ります",300,"White"
end

function SpecialSerif()
    WhiteSerif("これはパターン２のセリフです",200,"White")
    WhiteSerif("動きの管理はセリフだけではなく",400,"Blue")
    WhiteSerif("カメラワークや発動する演出に便利です",100,"Red")
    WhiteSerif("もっともっと効率を上げていきたいですね",400,"Green")
end

function MainScenario()
    WhiteSerif("これから出てくるセリフは乱数で切り替わります",200,"White")
    WhiteSerif("また分岐条件もスクリプトで自由に",200,"White")
    WhiteSerif("設定することが出来ますよ",200,"White")
    WhiteSerif("それでは乱数でセリフの切り替えをテストします",200,"White")
    local a = math.random()
    while true do
        a = math.random()
        if a > 0.5 then
            DefaultSerif()
        elseif a > 0.1 then
            SpecialSerif()
        else
            WhiteSerif("お！このセリフが見えていたらレアですよ！",400,"Blue")
        end
        
    end
    return "End","End"
end