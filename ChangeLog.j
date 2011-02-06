2011-XX-XX  UnNetHack 3.5.4
	* 次のように設定することで怪物の色を変更可能とした
	  MONSTERCOLOR=acid blob:lightcyan
	* 反転された階層でロングワームを攻撃したときクラッシュする問題を修正
        * HTMLダンプ：IBMgraphicsとDECgraphicsキャラクタを出力

	* ユーザインタフェースの改善：
	  - メニューオプションに新カテゴリ：「未識別アイテム」
	  - 'X'で二刀流に切り替え、探索モードは alt-x または #xplore で行う

	* バニラのバグ修正：
	  - C343-198

2010-09-04  UnNetHack 3.5.3
	* 争い状態は闇市の店主を怒らせる
	* 完全に腐食した防具は壊れる
	* クエスト実施に必要な経験レベルを下げた
	* HTMLダンプ
	* 円周率の日をサポート
	* 倉庫番分岐の深さを一つ減らした
	* 吸血鬼を開始種族として選択可能に (Slash'Emから)
	* 毒による突然死をなくし、代わりに HP と最大HP が減るようにした
	  (SporkHackから)

	* パッチ：
	  - 自発的挑戦：Elberethを追跡
	    Conduct: Elbereth tracking (by Ray Kulhanek)
	  - 迷宮地図の概要
	    Dungeon Map Overview 3 (by Hojita Discordia)
	  - 自発的挑戦拡張
	    Extended Conducts v2.3 (by Andreas Dorn)
	  - 偏執的確認パッチ
	    Paranoid Patch (by various)
	  - 分離可能な#adjustパッチ
	    Splittable #adjust Patch (by Sam Dennis and Jukka Lahtinen)
	  - チュートリアル
	    Tutorial (by Alex Smith)

	* ユーザインタフェースの改善：
	  - 怪我をしたとき回復の指輪を自動識別
	  - プレイヤーの隣の怪物が一匹だけならば話しかける方向指定を自動に
	  - Ctrl-eで自動的に足元に"Elbereth"を刻む
	  - 床の上の複数アイテムは反転表示する
	  - #chat なしに店内のアイテムの値段を表示する
	  - Curses GUI (装飾文字のコードがShift-JISとかぶる為、日本語未対応)
	  - 体力を棒グラフ表示
	  - 新偏執的確認オプション：paranoid_trap 既知の罠に踏み込む前に確認する

	* バニラのバグ修正：
	  - C343-12, C343-94, C343-189, C343-235
	  - SC343-8, SC343-20

	* アイテムの追加：
	  - 水難の巻物

	* アイテムの削除：
	  - 記憶喪失の巻物

	* 新しい部屋：
	  - 楽器店 (NetHack Brass RS から適用)
	  - ペット店 (Slash'Emから)
	  - 缶詰店 (Slash'Emから)

	* 新しい怪物:
	  - 巨大亀 (視界を遮る) (SporkHackから)

2010-01-10  UnNetHack 3.5.2-1
	異常終了バグ修正：存在しない薬での錬金術を防ぐ


2009-12-09  UnNetHack 3.5.2
	* スキルレベルの増加
	* 吸血と噛み付く吸血鬼 (Slash'Emから)
	* 新オプション： showdmg (wizmodeのみ)
	* トロルベーン： 空腹を促進しない回復能力の付与
	* ボーパルブレード： +1d8 追加ダメージ
	* rn2に0以下を指定して呼んだときのクラッシュを防ぐ
	* 振動する床： そばにいるプレイヤーにメッセージ

	* 新アイテム：
	  - 血液と吸血鬼の血液 (Slash'Emから)

	* パッチ：
	  - トリックの鞄拡張パッチ
	    Bag of Tricks Enhancement Patch (by Nephi)
	  - カメラを壊すと絵描きの悪魔が解放される
	    Breaking a camera may release a picture painting demon (by L)
	  - カメラの悪魔
	    Camera Demon (by L)
	  - 無力化パッチ
	    Cancellation Patch (by Malcolm Ryan)
	  - 色による錬金術パッチ
	    Color alchemy Patch (by Nephi)
	  - おまけの占いクッキー
	    Free Fortune Cookie (by Nephi)
	  - リトマスパッチ
	    Litmus Patch (by Dylan O'Donnell)
	  - 武闘家用のあいさつ文の追加
	    Monk specific greeting and goodbye (by Haakon Studebaker)
	  - 油っぽい死体
	    Oily corpses (by L)
	  - Qwertzレイアウトパッチ
	    Qwertz_layout Patch (by Stanislav Traykov)
	  - なすすべもなく殺された
	    While Helpless (by Pasi Kallinen)

	* バニラのバグ修正：
	  - C343-55, C343-116, C343-136, C343-211, C343-231

	* ユーザインタフェースの改善：
	  - 体力や魔力が回復した場合に複数ターン消費する行動を中断


2009-06-13  UnNetHack 3.5.1
	* 新しい階層：
	  - フォートローディオスレベル
	  - 倉庫番レベル (Pasi Kallinenより)
	  - メデューサレベル (Pasi Kallinenより)
	  - 城レベル (Pasi Kallinenより)
	  - ビッグルーム (Pasi Kallinenより)
	  - ヘックレベル (by Kelly Bailey)
	  - 新鉱山街：オークの街 (SporkHackから)
	  - ニンフレベル (Slash'Emから)
	  - 闇市 (Massimo Campostriniより)
	  - 階層反転パッチ
	    flipped levels patch (by Pasi Kallinen)

	* 新しい部屋：
	  - 庭園
	  - 荒らされた武器庫 (Lより)

	* 新アイテム：
	  - 鉄の金庫 (SporkHackから)
	  - 知力/賢さ/素早さの指輪 (Slash'Emから)
	  - アルミ箔の帽子 (Lより)

	* アイテムの挙動を変更：
	  - イェンダーの魔除けは落とすとどこかに瞬間移動する
	  - 金貨を探す巻物：混乱して読むとランダムなアイテムクラスを見せる
	  - 虐殺の巻物は現階層(呪われていない)か全階層(祝福)の怪物を抹殺する

	* パッチ：
	  - 精霊界での脱出
	    Astral Escape (via GruntHack from Chi)
	  - ダンプパッチ
	    Dump patch (by Jukka Lahtinen)
	  - ダンジョンで生長
	    Dungeon growths (by Pasi Kallinen)
	  - エキゾチックペット
	    Exotic Pets (by Rob Ellwood)
	  - 怪物の絶滅と生成数の表示
	    Extinct and showborn patch (from Jukka Lahtinen)
	  - 精霊界の順番のランダム化
	    Randomized Planes (by Patric Mueller)
	  - 簡易メール
	    Simple mail (from dgamelaunch)
	  - xlogfile (by Aardvark Joe)

	* バニラのバグ修正：
	  - Astral Call Bug (SC343-11, SC343-12)
	  - C343-8, C343-19, C343-100, C343-179, C343-218, C343-258, C343-259,
	    C343-268, C343-275, C343-276, C343-298, C343-318, C343-324

	* 新しい怪物：
	  - クトゥルフ (Slash'Em Letheから)
	  - 反物質の渦 (NetHack Brassから)
	  - 粉砕の怪物 (Nicholas Webbs biodiversity patch)
	  - ゴールドドラゴン (Nephiより)
	  - 雪蟻 (Slash'Emから)
	  - ボーパルジャバウォック (by L)

	* ユーザインタフェースの改善：
	  - 扉を自動で開ける (Stefano Bustiより)
	  - menucolors (Pasi Kallinenより)
	  - 投げたアイテムを拾う (Roderick Schertlerより)
	  - ステータスのカラー化 (Shachaf & Oren Ben-Kikiより)
	  - ウィンドウのふち (Pasi Kallinenより)

	* スコアが無制限に伸びるのを防ぐ
	* 買収される悪魔はさらに金を要求する (SporkHackから)
	* プリンの最大体力を半減させることで極端なプリン牧場を防止
	* 倉庫番の報酬を選択制に (NetHack Brassから)
	* クエストメネシスはElberethを恐れない
	* 悪魔ユニークはElberethを恐れない
	* アーリマンの心臓は幻影と魔力回復を授ける
	* 王座： 願えない
	* タオルの日をサポート
	* デスは大文字でしゃべる(和訳では強調で表現)
