-- トゥーンレンダリングのパラメータを設定する
function LightingParam()
    -- ハーフランバートの照明計算値で判定
    -- LightBorderからShadowBorderの間は線形補間が入る
    local LightBorder = 0.11
    local ShadowBorder = 0.08

    return LightBorder,ShadowBorder
end

function OutlineParam()
    -- 輪郭線に対するパラメータ
    local offsetScall = 1.0     -- 解像度で割る値。数値が大きいとそれだけ多くのテクセルが輪郭線となる
    local DepthRange = 0.1     -- 輪郭線として取り扱うサイズ。線の太さではなく輪郭線判定する範囲(深度)
    local NormalRange = 1.0     -- 輪郭線として取り扱うサイズ。線の太さではなく輪郭線判定する範囲(法線)
    local OutlineSum = 0.3      -- 周囲の色から算出した輪郭線にかける値(小さくなれば黒くなる)

    return offsetScall,DepthRange,NormalRange,OutlineSum
end

function CookTorranceParam()
    -- クックトランス適用部のHDR処理で照明度を減算する値とその後積算する値
    local HighlightSub = 1.5
    local HighlightSum = 30.0

    return HighlightSub,HighlightSum
end