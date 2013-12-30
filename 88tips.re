= Tips集

//lead{
この章では802.11で遊ぶ, 触るにあたってのtipsとしてキャプチャの仕方やキャプチャし
た結果の中身について参考となる情報をいくつか紹介します.
//}

== 802.11キャプチャ

ここでは, 無線LANのフレームであるところの802.11フレームを
手元のPCでキャプチャするにあたっての基本から小技あたりまでを紹介します.
ツールとしてはWiresharkを用います.これを使って基本的なEthernetのキャプチャ
をしたことがある, という前提で話を進めます.

=== Windowsをお使いの皆様への残念なお知らせ
残念ながらこれから説明する802.11フレームのキャプチャはWindows + 
無線LANアダプタでは@<strong>{基本的に利用できません}.
WindowsのWiresharkでは無線LANアダプタをキャプチャ対象として選択することは
可能なのですが, Ethernetフレームでのキャプチャしかできません.

Windowsでもやりたい！という場合, AirPcapを買うというのが今のところ一番確実な
方法です.
ただし802.11b/g対応品で$198, 802.11a/b/g/n対応品だと$698もするのでよっぽど
酔狂が過ぎないかぎりは手が出せません....

というわけでここからはLinux, Macに限ってお話を進めます.

=== 事前準備

意気揚々とWiresharkを使って802.11なフレームをダンプする, その前に以下の様な
お膳立てが必要です.

 1. キャプチャしたいチャネルを設定
 2. インタフェースをMonitorモードにセットする

==== 事前準備その1: キャプチャ時のチャネル設定
Wiresharkで実際にキャプチャを始める前に, 観測したいチャネルをコマンドラインから
指定してやる必要があります.
無線LANは1〜13チャネルまであり, 無線LANアダプタはこのいずれかを
「現在の自分の動作チャネルである」として内部的に動いています.
このため, ある瞬間ではひとつのチャネルでしか(通信|観測)ができません.
以前にどこかのAPに繋いだ後だとこれが残ったままになります.

てきとうにキャプチャするのであれば問題ないのですが, もし特定の無線LAN APに関連する
フレームを見たい場合はそのAPの動作チャネルに切り替えてやる必要があります.
以下はMacとLinuxそれぞれで無線LANアダプタの動作チャネルを変えるコマンドです.

//list[mac_channel_fix][Macでのチャネル設定]{
% airport -c (パケットキャプチャしたいチャネル番号)
//}

//list[linux_channel_fix][Linuxでのチャネル設定]{
% iw dev wlan0 set channel (パケットキャプチャしたいチャネルの番号)
//}

なお, 上記によるチャネル変更はキャプチャ中でも可能です.
定期的にチャネルを切り替えてやることで, 擬似的に全チャネルのフレームをキャプチャ
するといったことも可能です. 特定のAPの通信だけ見たい, というのでなく周囲を
流れているフレームをある程度集めたいといった場合は,
裏で定期的にチャネル番号を変えながら上記コマンドを発行するようなものを
組み込んでおくとよいでしょう.

なお, このチャネル設定は現在いずれかのAPに接続している場合には注意が必要です.
そのチャネルにてキャプチャをする場合は問題ありませんが, 別のチャネルを見ようと
チャネル設定を行ってしまうと通信ができなくなります. これはAPが通信に用いている
チャネルとクライアントの見ている(かつキャプチャしようとしている)チャネルに
ずれが生じてしまうためです. このため, チャネルを弄る場合は通信できることはあまり
当てにしないほうが吉です.

#@# ここに図がほしい?

==== 事前準備その2: Monitorモード設定

チャネルの設定とともに, キャプチャの口としたい無線LANアダプタのインタフェース
をMonitorモードに落としてやる必要があります.
この手順を踏まないと, 延々とEthernetフレームしか見られない!といったことに
なります. とはいえWiresharkが大体よしなにやってくれるので手順は至極シンプルです.

 1. CaptureメニューからOptionsを選んでCapture Optionsウィンドウを表示
 2. ウィンドウ上部に一覧されるインタフェースにて使う物をダブルクリック
 3. 出てきたウィンドウでインタフェースをモニタモードに設定
 4. OKを押してそのインタフェースでキャプチャ開始

モニタモードに設定するには以下の項目をチェックしてください.

 * @<em>{"Link-layer header type"}として@<b>{802.11 plus radiotap header}を選択
 * @<em>{"Capture packets in promiscuous mode"}のチェックボックスをOnに
 * @<em>{"Capture packets in monitor mode"}のチェックボックスをOnに

インタフェースの表示が以下の様になっていれば, 802.11フレームのキャプチャができる
ようになっています.

//image[monitormode][Monitor Modeに設定されたインタフェース]

=== WiresharkでのWEP/WPA復号化設定

多くのAPではWPAやWEPを導入しており, データが入っているペイロードは暗号化されてい
ます。このため, Wiresharkで見ても今どんな通信が行われているかは見えません.
しかし, 鍵さえ分かっているならばWiresharkの復号化機能を使って中身を
見ることができます. ここではそのやり方についてざっくりと解説します.
復号化の設定は以下の手順で行います.

 1. EditメニューからPreferencesを選んでPreferencesウィンドウを表示
 2. ウィンドウ左側のメニューからProtocols→@<em>{IEEE 802.11}を選択
 3. @<b>{Enable decryption}チェックボックスをOnにセット
 4. Decryption Keysの@<b>{Edit}ボタンを押し, @<em>{WEP and WPA Decryptions Keys}ウィンドウを表示
 5. Newボタンで新しい鍵の追加

ここでは@<em>{Key type}に応じて以下の様なKeyを文字列として入力することで
復号化機能を利用できます.

 * wep : WPAキーの16進数文字列
 * wpa-pwd : 8〜63文字のパスフレーズ + ":" + 対象となるSSIDの文字列
 * wpa-psk : 64文字の16進数文字列からなる事前共有鍵(PSK)

wep(WEP)の場合は, 大抵5文字(40/64bit)または13文字(104/128-bit)のWEPキーなるものを
用いていると思われますが,これをASCIIコードで16進数に変換したものを入力します.
たとえばWEPキーが@<em>{abcde}ならASCIIコードでは@<em>{0x61 0x62 0x63 0x64 0x65}
なので入力すべき文字列は@<b>{61:62:63:64:65}になります.
WEPキーが@<em>{abcdefghijklm}の場合, 13文字ですので
@<b>{6162636465666768696a6b6c6d}という26文字の16進数文字列になります.
手計算でも良いですが, Web上にJavascript製のツールがいくつかあるので
簡単にやるならそういったものを用いても良いでしょう.


これらを設定することで復号化が有効になります.
ただしWiresharkではキャプチャを走らせながらのランタイムの復号化はできません.
復号化は基本的に, キャプチャを止めた後または止まっている状態で鍵をセットした
時に走ります@<fn>{airpcap_dec}.
//footnote[airpcap_dec][なお, 先に述べたAirPcapにはランタイムに復号化する機能があります]


=== 覚えておくと便利なフィルタ

Wiresharkで実際に802.11フレームのキャプチャをしてみると分かることですが,
電波というメディアの性質上, ただ単純にダンプをするだけではあまりに流速が速いこと
に気付きます@<fn>{dirtywlan}. また壊れたフレームも多く, 目的のフレームが見つけづらいため
きちんとフィルタをかけてキャプチャを行う必要があります.
ここではいくつか定番(?)のフィルタルールを紹介します.
//footnote[dirtywlan][とある環境では分速20000フレーム程度でした. コミケ会場とかいろいろと楽しそうですね]

なお, Wiresharkには一度設定したフィルタルールをSave(保存)することができ,
次回以降はルールを手で打ち込むことなくボタン一発で呼び出すことができる機能が
あります. 便利なルールを考え出したら積極的に保存していきましょう.

//list[filter_addr][特定のAP/クライアントを対象とした通信のみをキャプチャしたい]{
wlan.addr == (対象となるAP/クライアントのMACアドレス)
//}

もしキャプチャを始める前に「このMACアドレスのAP/クライアントに着目したい」という
ことが決まっているのであれば, 次に挙げるフィルタをかけるのが最適です.

この方法では,キャプチャを始める前に「Captureメニュー」から「Options」を選択します.
Capture Optionsウィンドウの中央, 「Capture Filter」のテキストボックスに以下の様
にMACアドレスを設定します.
//emlist{
ether host (対象のMACアドレス)
//}

この状態でStartボタンを押しキャプチャをスタートすると, そのアドレスをSrc/Dstの
いずれかにもつフレームのみが採取されるようになります.
@<list>{filter_addr}はキャプチャ中でも切り替えが効くため有用ですが, 
長時間に渡って特定のホストの通信をキャプチャしたい場合はこれをやっておくと全体の
キャプチャ量が減らせるため, この後にかけるフィルタの適用時間短縮につながります.
ただし, Src/Dstともに関係ないフレームはすべて見えなくなってしまうため,
上記2つの方法のどちらが適切かは事前によく考えておく必要があります.

//list[filter_fcsgood][壊れたフレームを除外したい]{
wlan.fcs_good == true
//}
電子レンジやBluetoothなどの外部要因やそもそものキャリア検知の失敗等々で,
空気中にはFrame Check Sequenceがおかしなパケットがそこそこ観測される場合が
あります。そのような場合には@<list>[filter_fcsgood]のルールを合わせて記述しておくと良いです。

//list[filter_retry][再送フレームを除外したい]{
wlan.fc.retry == 0
//}
FCSのチェックと同様に, その周波数帯が混雑している場合には各無線LAN機器が
再送フレームを多量に送信する場合があります. 再送に着目したいので無ければ
@<list>{filter_retry}にあるようにFrame ControlフィールドのRetryビットが
立っているフレームを除外しましょう.

//list[filter_mgmt][大量にでるManagementフレームを除外したい]{
!( wlan.fc.type_subtype == 0x04 or
   wlan.fc.type_subtype == 0x05 or
   wlan.fc.type_subtype == 0x08 )
//}
各無線LAN機器ともBeaconやProbe Request/Responseをそこそこ大量に送信します.
これらのフレームは, 接続切断の流れやデータのやりとりを見るに当たっては非常に
邪魔になります. この場合には@<list>{filter_mgmt}にあるようなフィルタにて
定常的に送信されるようなものを除外しましょう.

//list[filter_chan][特定のチャネルに流れているフレームのみ抽出したい]{
radiotap.channel.freq == (そのチャネルに対応する周波数)
//}
前述のように, キャプチャ中にチャネルを変更することができます.
このときに, 特定のチャネルでの流れを見たい場合は@<list>{filter_chan}にある
ようなフィルタでこれを抽出することができます.
このフィルタの難点として「チャネル番号」ではなく「周波数」を入力するという点
があります. なぜかチャネル指定が可能なフィルタがないので, 変換をしましょう.
チャネルと周波数の対応表については後述する@<table>{tbl_chanfreq24}を参照して
ください.

//list[filter_type][特定のタイプのフレームのみ抽出したい]{
wlan.fc.type == (フレームのタイプ)
//}
おおざっぱにフレームのタイプで絞り込みたい場合はこのルールを用います.
フレームのタイプとしては以下の３つの値を指定可能です.

 * 0x0 : Managementフレーム
 * 0x1 : Controlフレーム
 * 0x2 : Dataフレーム

//list[filter_subtype][特定の種類のフレームのみ抽出したい]{
wlan.fc.type_subtype == (フレームの種類)
//}

フレームの種類, つまりタイプとサブタイプまで絞って抽出したい場合はこのルール
を用います. 具体的に指定可能な値については後述する
「Frame Control Type/SubType一覧」を参照してください.

//list[filter_bssid][特定のAPに紐付いた通信のみを抽出したい]{
wlan.bssid == (対象となるAPのBSSID)
//}
電波というメディアの特性上, 着目しているのとは別のAPに関係するフレームも
見えてしまいます. 特定のAPに紐付いた通信(おなじAPの配下にいるクライアント
同士の通信など)に限定したい場合, @<list>{filter_bssid}のフィルタが利用できます.
BSSIDは, 大抵の場合対象のAPがもつMACアドレスです.

//list[filter_eapol][4-Way Handshakeのやりとりを抽出したい]{
eapol
//}
WPA利用時の鍵交換(4-Way Handshake)がうまく行っているかどうかのみに着目したい
場合, @<list>{filter_eapol}にあるフィルタが利用できます. 
EAPOLのフレームだけは, 暗号化されずに中身が見えるためこのように802.11より上の
レイヤのプロトコルを指定しての抽出をすることが可能です.

//list[filter_connect][接続から4-way Handshakeまでのやりとりを抽出したい]{
wlan.fcs_good == true and ( eapol or wlan.fc.type_subtype < 0x04 or
wlan.fc.type_subtype == 0x0a or
wlan.fc.type_subtype == 0x0b or
wlan.fc.type_subtype == 0x0c )
//}
@<list>{filter_eapol}をさらに拡張して,
接続切断および認証関連のフレームとEAPOLのみを観測したい場合,
@<list>{filter_connect}のようなフィルタが有用です.

#@# == 状況把握のためのデバイス
#@# //lead{
#@# 無線LANの状況把握という観点では, 特に運用するにあたってはパケットキャプチャを
#@# するというのはほぼ最終手段というかあまり訳には立ちません.
#@# 実際には
#@# 
#@# //}
#@# 
#@# 
#@# //TODO
#@# 
#@# 前節で紹介したWiresharkにて
#@# 
#@# inSSIDer
#@# 
#@# - Android
#@# - WiSpy
#@# やすいのがある
#@# - Airpcap

== 主要なフレームフォーマット
//lead{
ここでは, 前節までに述べたパケットキャプチャにより得ることができる各種802.11
フレームについて主要かつ良く登場するものについてその中身について解説します.
本稿の内容は802.12-2012を参照して記述しています.
//}

=== 802.11フレームの構成とヘッダについて

802.11フレームは, APがいてクライアントがそこにつながっているような形態の場合,
@<img>{80211_frame}にあるようなフォーマットになっています.
Frame ControlフィールドからFrame Bodyの手前までが, 一般的に802.11ヘッダと
呼ばれる部分になります. Wiresharkでキャプチャした場合には, Radiotapの次に並ぶ
要素になります.

//image[80211_frame][802.11 フレームの典型的な構造][scale=0.5]

#@# FC部分
Frame Controlは, このヘッダをもつ802.11フレーム全体の情報を保持している
フィールドです. その内訳は以下の様になっています.

: Protocol Version
	このフィールドは, 802.11のどのStandardに従っているフレームかを示します.
	現在のところ, @<em>{0}のみが有効です.
: Type, Subtype
	Type/Subtypeフィールドはこのフレームの種類、タイプを示すフィールドです.
	この部分に入っている値に従って802.11スタックはフレームのパースを行い
	ます. 具体的な値については@<table>{tbl_typesubtype_mgmt},
	@<table>{tbl_typesubtype_ctrl}, @<table>{tbl_typesubtype_data}を
	参照してください.
: To DS, From DS
	To DS, From DS@<fn>{ds}は, このフレームの方向について示すビットに
	なります. この2つを合わせて, 無線LAN内の通信なのか, 外行きなのか, 
	外から入ってきたのかなどを表します.
	このフィールドによってAddressフィールドのどれがどの役割を果たすのかが
	変わってきます. これについては@<table>{tbl_todsfromds}に一覧を記載して
	ありますのでそちらを参照してください.
: More Fragments
	802.11にはひとまとまりにして送りたいデータが規定サイズより大きい場合は
	分割して送りつけることができます. この際に, 後続するフラグメントを含む
	データフレームがある場合はこのビットを1にしておきます.
: Retry
	このフレームが再送フレームであることを示すビットです.
	無線という都合上, いわゆるパケロスがそこそこ発生するので受け手で
	重複排除がしやすいようにこのフラグを立ててやります.
: Power Management
	主にDataフレームにおいて使われ, クライアントが自身のパワーセーブモード
	の状態をAPに伝達するために使われます. 0の場合は起きていることを示しますが,
	1になった場合はパワーセーブ機能が有効になったことを示します.
	APはこれを受け取り, そのクライアント宛データフレームのバッファリングを
	開始します.
: More Data
	このビットは, 送り先のクライアントに対してパワーセーブ機能を有効にしてい
	た間にバッファリングしていたフレームが(まだ)あるよ、ということを伝える
	ために使われます. 通常のフレームや, 後続データがない場合は0になります.
: Protected Frame
	特にDataフレームについて、そのデータ部分が暗号化されているかどうかを
	明示するフィールドです. 1の場合, Frame Bodyには暗号化に関する情報(
	たとえばベクタなど)のヘッダと暗号化されたデータが入っているということを
	表します.
: Order
	Dataフレームとその順序制御について表したビットです.
	無線LANのフレームは, 再送が絡んだ場合やマルチキャスト/ユニキャストの差
	等で到着順序がシーケンス番号上前後する可能性があります. この場合, 
	基本的にはそのまま上位レイヤにこのデータが渡されます. VoIPなどこれを
	許容できないアプリケーションのデータについてはこのフィールドを1にして
	おくことで正しいオーダーでの処理を相手に強制することができます.
	これは普通のDataフレームでの話であり, QoS DataフレームではHT Control
	フィールドの有無を明示するために使われます.


//footnote[ds][DSは"Distribution System"の略です. 一般的には無線LANの上位/外にある有線LANのネットワークのことだと思っておけばよいでしょう.]

802.11フレームの, いわゆるDestination Address, Source AddressはTo DSやFrom DS,
はたまたモードによって使われ方が変わるためAddress1, Address2, Address3という
あやふやな表記になっています(また実際にはAddress4もあるのですが, 今回は
メッシュモードは扱わないので省いてます).
APという中継者がいるため, Ethernet的な簡単なDst/Src以外にも中継送信者, 中継
受信者といったものが間に存在しておりいろいろと複雑です.
@<table>{tbl_todsfromds}ではこの
To DS, From DSに基づくAddressフィールドの使われ方の違いについて記載します.
いわゆるEthernetでのDst, Srcという表記を使いつつ,  802.11において一般的な
SA(Source Address), DA(Destination Address), TA(Transmitting Station Address),
RA(Receiving Station  Address), BSSID(Basic Service Set Identifier, いわゆるAP)
という語も並記します.

//table[tbl_todsfromds][To DS, From DSによる方向, Addressフィールドの一覧]{
ToDS	FromDS	意味					Address1		Address2		Address3			Address4
--------------------------------------------------------------------------------
0	0	アドホックモードでのクライアント間通信	Dst@<br>{}(RA, DA)	Src@<br>{}(TA, SA)	BSSID				なし
0	1	DSまたは無線LAN内の別クライアントから	Dst@<br>{}(RA, DA)	BSSID@<br>{}(TA)	Src@<fn>{amsdu}			なし
1	0	無線LANクライアントからDS宛, 外行き	BSSID@<br>{}(RA)	Src@<br>{}(TA)		Dst@<fn>{amsdu}			なし
1	1	メッシュモード				RA			TA			Dst@<br>{}(DA@<fn>{amsdu})	Src@<br>{}(SA@<fn>{amsdu})
//}
//footnote[amsdu][802.11nで導入されたAggregated MSDUを用いている場合, おのおのの集約されたMSDUがアドレスを持っているのでこの部分がBSSIDになります.]

Frame Bodyは, Dataフレームの場合はそれががそのままペイロードとして扱われますが
ManagementフレームやControlフレームおよび一部のDataフレームの扱いではFrame Body
部分にさらに独自のヘッダやフィールド, Information Elementと呼ばれる特別なデータ等が続きます.
このあたりを次の節からざっと見ていきます.


=== 代表的な802.11 Frame

ここからは, パケットキャプチャ中に取れるいくつかの代表的なフレームの中身を
見ていきます. なお, ここで上げたフレームのフォーマットは一例であり,
実環境ではここで記載したのとは微妙に違うフレームが表れる場合もあります.

==== Beacon Frame

Beaconフレームは, APが定期的に送信し自身の存在を告知しつつそのAPに接続するために
必要な設定などを配布するために使われます. クライアントにあたるPCで, 接続先AP一覧
が見えるのもこのBeaconフレームを送信していることによります.
その性質上, Wiresharkでキャプチャしている間, 頻繁に流れているのが見えるでしょう.
一般的なBeaconフレームは@<img>{frame_beacon}にある様な構造を取っています.

//image[frame_beacon][Beacon Frameの例]

Beaconの場合, 802.11ヘッダの次に8バイトのTimestamp Fieldが続きます.
このタイムスタンプはそのAPに接続する無線LANクライアントとの間で時刻同期を
行うために用いられます. AP側はBeaconフレームの中にこれを埋め込んで定期的に送信,
クライアント側はこれを受け取って自分の内部のタイマを修正します.
この仕組みはTSF(Timing Synchronization Function)と呼ばれます.

タイムスタンプの次に, Beacon Interval Fieldが続きます.
このフィールドは, Target Beacon Transmission Timer(TBTT)つまりAPが何Time Units毎
にBeaconフレームを送信するかを表したものになります.一般的にTime Unitsはミリ秒を用います.
Beaconフレームは, 後述するTIMやパワーセーブ機能有効時に復帰のために必要となる
情報を保持しています. クライアントは(レイテンシを下げるためには)これを可能な限り
オンタイムに拾う必要があります. このため「次のビーコンがいつ来るか」ということを
事前に把握して一回ごとのパワーセーブ機能の有効時間を調整しようと試みます.
Beacon Intervalはこのための情報として使われます.

Beacon Intervalの次に, ここではCapatilities Fieldsが続いています.
Capatilitiesには, これを送った無線LAN機器で規定の機能のうち, どれをサポート
しているかが格納されます.

ここで上げたTimestamp, Beacon Interval, Capabilitiesの他にもいくつかの
フィールドが続く場合がありますが, 今回は割愛します.
これらのフィールドが続いた後にInformationElement(IE)が並びます.
この部分に関しては後述する「802.11 Information Element」を参照してください.
APは, この部分に自身の動作情報を格納してクライアントに送ります.

==== Probe Request/Response Frame

前項で説明したBeaconフレームは, APからその情報を送るためのものでした.
Probe Request/Responseフレームはクライアントが主体となって"APから情報を集める"
ためのやりとりにおいて利用されます. フレームは@<img>{frame_probe}にあるような
形式をとっています.

//image[frame_probe][Probe Request/Responseフレームの例]

一般的に, 無線LAN機能をOnにしたクライアント機器は, 接続可能なAPを自分で探す
必要があります. APが送ってくるBeaconフレームをかき集めるのも一つの手です.
アクティブに探す方法としてこの項で説明するProbe Requestフレームを用います.

クライアントはAPを探すためにまず探査対処のチャネル当てにProbe Requestフレームを
ブロードキャスト(FF:FF:FF:FF:FF:FF宛)します, APはこれを受け取り(応答するかどうか
の判断をした後に) Probe Responseフレームをそのクライアント宛にユニキャストで
送信します. クライアントはこの応答を受け取ることで, そのチャネルにおけるAPの存在
と動作状況を収集します.

Probe Request/Probe ResponseフレームでもInformation Elementが並びます.
Probe ResponseフレームにおけるInformation ElementはBeaconのそれと同じものが
並ぶのが一般的です(やりたいことはBeaconと変わらないため).
Probe Requestフレームは, APの存在検知が主目的ではあるためIEは不要そうですが
@<img>{probe_ie}にあるようにいくつかの要素が並びます.

//image[probe_ie][Probe Requestに紐付くInformation Elementsの例]

これは, たとえばSSID隠蔽機能を用いているAPに対して問合せを行うために,
通常はワイルドカード(空)になっているSSIDを特定の文字列にして
送るといったことに用いられます.

==== Authentication Frame

Authenticationフレームは, 無線LANクライアントが無線APに対して接続する際に
802.11上に規定される認証を行うために使われるフレームです.

//image[frame_auth][Authentication Frameのサンプル][scale=0.75]

Authentication Algorithm numberは, このAuthenticationフレームおよび後続の
やりとりで使用したいor使用する認証方式を番号で指定するフィールドです.
@<table>{tbl_authalg}にあるものが利用できます.
Sequence Numberは認証のやりとりにおけるAuthenticationフレームのシーケンス番号
を入れておくフィールドです. 特にWEPのShared Key利用時にはチャレンジレスポンス
に則って何度かAP-クライアント間でこのフレームを送り合うため, その順序を明示
するために用いられます. Status Codeは, 各認証のステップでの成功/失敗等を相手に
伝えるためのフィールドです. 0なら成功, それ以外なら失敗となります
@<fn>{auth_status}.
//footnote[auth_status][個別の番号とその意味については802.11-2012 8.4.1.9 Status Code参照]

//table[tbl_authalg][Authentication Algorithm Number一覧]{
番号	認証方法
----------------
0	Open System認証
1	Shared Key
2	Fast BSS Transition (802.11r)
3	SAE (simultaneous authentication of equals)
65535	ベンダ独自
//}

WPAが主流となった今では, 主な認証処理はいわゆる4-Way Handshakeと呼ばれる
EAP over LAN (EAPOL)を用いたものとなっており元来の認証方式である
Authenticationフレームの役割は小さくなっています.
手順としては必須なので接続時に観測することはできますが, クライアント/APともに
@<img>{auth_ie}にあるようなOpen Systemで常にStatus CodeがSuccess(0)である
フレームを送り合うだけになっています.

//image[auth_ie][WPA/WPA2利用時に観測できるAuthenticationフレームのボディ][scale=0.7]

==== Association/Reassociation Request/Response Frame

Associationフレームは, クライアントがAPに@<b>{接続する}際のやりとりに利用される
フレームです. それぞれクライアントからの接続要求(Association Request)と,
APからの接続応答(Association Response)という二つの方向があります.
また, 無線LANという切り替わりが柔軟なメディアという都合上, 再接続(Reassociation)
用に別の種類のフレームが用意されています. このフレームのフォーマットは
@<img>{frame_assoc}にあるようになっています.

//image[frame_assoc][Association/Reassociation Request/Responseフレーム][scale=0.65]

Listen Intervalフィールドは, クライアントがパワーセーブモードから復帰した時に
「ビーコン間隔のいくつ分を起きたままでいるか」を表したものになります
(パワーセーブ機能についてはTIM IEにて後述).
APは, Beacon Intervalにてビーコンを何msec間隔で送るかを告知します.
このTBTTにListen Intervalを掛けたものが, クライアントが一時的に起きたままでいる
時間になります. APはこの情報に基づいて, パワーセーブモード時のバッファリング
およびそのタイムアウト時間を調整します.

Reassociation Requestフレームの場合, Current AP Addressフィールドが挿入
されます. ここには, 現在このRequestを送ったクライアントが接続中のAPの
MACアドレスが格納されます.

接続の成功, 失敗についてはAssociation/Reassociation Responseフレーム中の
Status Codeにて通知されます. Authenticationフレーム同様, 0が成功, それ以外は
失敗となります.

成功の場合, さらにAssociation ID (AID)フィールドになんらかの数値が入っています.
これは, そのAPにおけるクライアントの識別番号になります.
802.11の各所でこのAIDに基づいた処理が行われるため, クライアント側にも
これを通知するようになっています.

==== Data/QoS Data

実際に, 802.11上にIPパケットなど上位レイヤからのデータを流すにはDataフレームを
用います. ともに802.11ヘッダの後に続くFrame Body部分にデータをそのまま入れ,
アドレスを適切に設定することでこれを行います.

データ通信用のフレームには大きく DataとQoS Dataの二種類があり, 後者の場合は
QoS Controlフィールドが802.11ヘッダに付加されます.
このフィールドは@<img>{qos_control}に示される内容からなるフィールドです.
この中で特に重要なのはPriority(TID)の部分で, ここに適切な優先度を設定してやる
ことでIEEE 802.11eで導入されたQoSに基づいてデバイス/MACでの処理がなされます.
特に制御が必要なデータ, たとえば鍵交換(EAPOL)などのフレームにはNetwork
Control(Void)の優先度をつけてやるなど可能な限り優先度高めに相手に伝達される
ように内部的にスタックが調整を行います.

//image[qos_control][QoS Controlフィールドの内容][scale=0.65]

なお, WEPやWPAで暗号化されている場合Frame Bodyの中身には暗号化されたデータ
とともに, その暗号化用のヘッダが先頭に付加されます.

==== Deauthentication/Disassociation Frame

Deauthentication/Disassociationフレームはそれぞれ, APまたはクライアントが相手
との接続を切断するために用いられます. このフレームは@<img>{frame_dis}に
あるようなフォーマットになっています.

//image[frame_dis][Deauthentication/Disassociationフレームのフォーマット][scale=0.7]

Wireshark等で見ても, @<img>{disassoc_ie}にあるように切断理由を伝える
ためのReason Codeのみが存在します.
DeauthenticationとDisassociationのいずれの場合でも, 切断するという意味合いは
変わらないためどちらが飛んでくる/飛ばすかは実装依存の場合があります.

//image[disassoc_ie][Disassociationフレームのボディの例]

== 接続から切断までの流れ

ここでは, これまでに述べた主要な802.11フレームが接続から通信, 切断までの
ライフサイクルにおいてどういった順序で表れ使われるかを見ていきます.
ざっと図にすると, APとクライアント間では@<img>{connection_life}にあるような
流れでフレームのやりとりが行われます.

//image[connection_life][AP, クライアント間での802.11フレームのやりとり][scale=0.6]

=== WPA/WPA2での4-Way Handshake

802.11的にはAuthenticationとAssociationが済めば接続が完了となります.
WEPの場合, この時点で暗号化についての折衝が終了しているので通信が開始できますが
WPAの場合はここから鍵の折衝をする必要があります.
このやりとりを4-Way Handshakeと言います.
今回はこの折衝部分について, 特にどういったフレームを送っているのかという点に
ついて見ていきます@<fn>{crypt}.
//footnote[crypt][暗号化まわりについて踏み込むともう一冊必要そうなので...]
4-Way Handshake自体の流れはWiresharkにて"eapol"というフィルタを追加すると
@<img>{eapol_filter_cap}にあるように観測することができます.

//image[eapol_filter_cap][Wiresharkで見えるEAPOLのフレーム]

4-Way Handshakeで送られるメッセージは, 802.11的にはQoS Dataフレームとして
送られます. 個々のメッセージは@<img>{eapol_cap}にあるようにLogical Link Control 
(LLC)のヘッダと802.1X Authenticationなるペイロードからなります.
内訳は@<img>{eapol_key}にある通りです.
//image[eapol_cap][Wiresharkで見えるEAPOLフレームの例(Message 1)]
//image[eapol_key][EAPOL-Keyフレームの中身]

それぞれAPとクライアントは, このメッセージのフィールドを変えながら相互に
送りあい鍵の折衝を行います. 例としてWPA2-PSKの場合, @<table>{tbl_eapol4way}
にあるようなフィールドでメッセージが送られます.

//table[tbl_eapol4way][4-Way Handshake中の各メッセージの要素]{
フィールド		Message1	Message2	Message3	Message4	備考
------------------------------------------------------------------------------------
Version			0x02		0x02		0x02		0x02		固定
Type			0x03		0x03		0x03		0x03		固定
Length			*		*		*		*		後続要素の長さ次第
Descriptor Type		0x02		0x02		0x02		0x02		IEEE802.11用@<fn>{8021x_dtype}
Key Info:Version	0x2		0x2		0x2		0x2		HMAC-SHA1-128@<fn>{dver}
Key Info:Key Type	1		1		1		1		PTKの折衝につきPairwise固定@<fn>{dver}
Key Info:Key Index	0		0		0		0		
Key Info:Install	0		0		1		0		
Key Info:Key ACK	1		0		1		0		
Key Info:Key MIC	0		1		1		1		
Key Info:Secure		0		0		1		1		
Key Info:Error		0		0		0		0		問題が無い場合は0固定
Key Info:Request	0		0		0		0		AP始動のため0固定
Key Info:Encrypted	0		0		1		0		
Key Length		16@<fn>{keylen}	0		16@<fn>{keylen}	0		
Replay Counter		0		0		1		1		
Key Nonce		ANonce		SNonce		ANonce		0x00		
Key IV			0x00		0x00		0x00		0x00		PTKなので0x00固定
RSC			0x00		現在の値	現在の値	現在の値	
ID			0x00		0x00		0x00		0x00		今の仕様では0x00固定
MIC			0x00		MIC値		MIC値		MIC値		
Data Length		0x00		キーデータ長	キーデータ長	0x00		
Data			なし		暗号化関連IE	IEまたはGTK	なし		
//}

//footnote[8021x_dtype][802.1X-2010のTable 11-5 Descriptor Type Value assignmentsにてIEEE802.11 Key Descriptorは2となっている]
//footnote[dver][802.11-2012のFigure 11-29 Key Information bit layout以降参照]
//footnote[keylen][802.11-2012のTable 11-4 Cipher suite key lengths]


== Frame Control Type/SubType 一覧

//lead{
この項では802.11フレームを識別するのに使われるTypeおよびSubtypeについてその
一覧を見ていきます
//}

それぞれのフレームは, ヘッダの中にあるFrame ControlフィールドのTypeおよびSubtype
にそのフレームが何であるかを数値として持っており802.11スタックはこれを用いて
処理を変えるようになっています. Wiresharkでも, フレームの種類を出す場合はこれを
参照しておりフィルタでうまく引っかけて問題のフレームを探すにはこれを覚えておくor
メモっておくと便利です.

ここでは主要なものを一覧します.それぞれタイプごとに,
Managementフレームは@<table>{tbl_typesubtype_mgmt},
Controlフレームは@<table>{tbl_typesubtype_ctrl},
Dataフレームは@<table>{tbl_typesubtype_data}
に記載しました@<fn>{typesub}.
//footnote[typesub][大元の表は802.11-2012のTable 8-1 Valid type and subtypecombinationsにあります.]

//table[tbl_typesubtype_mgmt][802.11 Frame Control Type/Subtype : Management編]{
Type/Subtype	種類
-----------------------------------
0x00		Association Request
0x01		Association Response
0x02		Reassociation Request
0x03		Reassociation Response
0x04		Probe Request
0x05		Probe Response
0x08		Beacon
0x0a		Disassociation
0x0b		Authentication
0x0c		Deauthentication
0x0d		Action
//}

//table[tbl_typesubtype_ctrl][802.11 Frame Control Type/Subtype : Control編]{
Type/Subtype	種類
-----------------------------------
0x18		Block Ack Request
0x19		Block Ack
0x1a		PS-Poll
0x1b		RTS
0x1c		CTS
0x1d		ACK
0x1e		CF-End
0x1f		CF-End + CF-Ack
//}

//table[tbl_typesubtype_data][802.11 Frame Control Type/Subtype : Data編]{
Type/Subtype	種類
-----------------------------------
0x20		Data
0x21		Data + CF-Ack
0x22		Data + CF-Poll
0x23		Data + CF-Ack + CF-Poll
0x24		Null (ペイロードなし)
0x25		CF-Ack (ペイロードなし)
0x26		CF-Poll (ペイロードなし)
0x27		CF-Ack + CF-Poll (ペイロードなし)
0x28		QoS Data
0x29		QoS Data + CF-Ack
0x2a		QoS Data + CF-Poll
0x2b		QoS Data + CF-Ack + CF-Poll
0x2c		QoS Null (ペイロードなし)
0x2e		QoS CF-Poll (ペイロードなし)
0x2f		Qos CF-Ack + CF-Poll (ペイロードなし)
//}

//pagebreak


== 802.11 Information Element

//lead{
それぞれBeacon, Probe Request, Probe Responseフレーム等には, ヘッダの後ろに
Information Element (IE)なるものが付加されており, これを参照することでそのAPまたは
クライアントでどういった設定が有効かを知ることができます. ここでは各種存在する
Information Elementの内, よく見かけるものについて解説します.
//}

Information Elementsについては802.11-2012の「8.4.2 Information Elements」に
その一覧および詳細が記載されております.

=== Element ID 一覧

各Information ElementはTLV(tag-length-value)形式を取っており,
Tag(Element ID)の番号を見ることでそれが何かが分かります.
ここでは今回取り上げるもののIDを一覧します.
全体の表については802.11-2012のTabler 8-54 Element IDsを参照してください.

#@# HT Cap/Infoは今回は外し
//table[tbl_elementid][802.11 Information Element IDs]{
Element ID	Information Element
-------------------------------------
0x00		SSID
0x01		Supported Rates
0x03		DSSS Parameter Set
0x05		TIM
0x07		Country
0x2a		ERP
0x30		RSN
0x32		Extended Supported Rates
0xdd		Vendor Specific
//}

=== よく出るInformation Elements

ここでは, BeaconやProbe Responseフレームなどで良く見かけるものを取り上げ,
その中身について見ていきます.
先に述べたように各Information ElementはTLV形式を取っており, @<img>{ie_tlv}に示され
るフォーマットとなっています. 先頭から1バイトのTag (Element ID),
1バイトのLength(長さ), 最後にLengthに格納されている長さ分の
Value(データ)が続きます.

//image[ie_tlv][Information ElementのTLV形式][scale=0.7]

==== SSID IE

Service Set Identifier (SSID) IEは, その無線LAN機器で用いるSSIDを格納するIEです.
Value部分にはいわゆる無線APの人間から見た識別子であるSSIDに当たる文字列が入ります.
主にAPから送信されるBeaconやProbe Responseでこれを用いることで, 無線クライアント
これを伝えユーザに見えるようにします.
無線LANクライアントからのProbe Requestにても利用されますが,
多くの場合は空(ワイルドカード扱い)にして送ります.

LengthにはValueに入っているSSIDのバイト長が入ります.
Valueは基本的にはASCII文字列からなるSSIDが入りますが, 今回紹介しない
Extended Capabilities Element (0x7f)のUTF-8 SSIDフラグをセットすることで
UTF-8文字列を利用可能です.
ただしいずれの場合も0-32バイトに納める必要があります.

==== Supported Rates IE

Supported Rates IEは, その無線LAN機器でサポートするレート(データ送受信速度)を
相手側に伝えるために用いられます.
おもに802.11b時代の1〜11Mbps程度の値を格納します.

// エンコード方法

基本的には下位互換性の観点から, 受付可能なものをすべて並べるのが基本ですが,
公衆無線LAN等を提供する場合あまりに遅いデバイス/レートを受け入れてしまうと
全体の足を引っ張ってしまうため, あえて遅めの値を省略する場合があります.

//image[full_rates][遅めの値から並べてる場合][scale=0.7]
//image[reduced_rates][遅めの値を省略している場合][scale=0.7]

==== Extended Supported Rates

Extended Supported Rates IEは, 前項のSupported Ratesよりさらに上の速度のサポート
状況を伝えるために用いられます. 主に11aや11gの導入にて最大54Mbpsのサポートが可能
となったため使われるようになりました.

// エンコード方法

こちらについてもSupported Rates IE同様, サポートしているものをすべて並べるのが
基本ですが一部のレートを省く場合があります.

==== DSSS Parameter Set IE

Direct Sequence Spread Spectrum (DSSS) Parameter Set IEは,
そのフレームを送信した無線LAN機器が稼働しているチャネルを示すために使われます.
名前自体は直接スペクトラム拡散方式と, 802.11b時代の伝送方式に由来します.
基本的にはBeaconおよびProbe Request/Responseフレームにて使われます.
無線LANクライアントは自身が接続可能なAPを探すために, Probe Requestを
ブロードキャストしAPが返してくるはずのProbe Responseをポーリングします.
この結果を元にあるAPに決めた際に自身の動作チャネルを決定する材料として
このIEに含まれるチャネル番号を利用します.

このIEのLengthは1で固定になっており, Valueにはいわゆるチャネル番号(1〜13)を
16進数1バイトの値として格納されます(つまり0x01〜0x0dのいずれか).

==== TIM IE

Traffic Indication Map (TIM) IEは, 無線LANクライアントに対してペンディングされて
いるData フレームがあることを伝えるために用いられます.
このIEは@<img>{ie_tim}にあるような構造を取っています.
詳細は802.11-2012の8.4.2.7 TIM elementを参照してください.

#@# //table[ie_tim][TIM IEの構造]{
#@# フィールド	長さ(バイト)	意味
#@# -----------------------------
#@# Element ID	1		IEのTag
#@# Length		1		TIM IEのValueの長さ
#@# DTIM Count	1		次に
#@# DTIM Period	1		
#@# Bitmap Control	1		
#@# Partial Virtual Bitmap	1〜251	
#@# //}

//image[ie_tim][TIM IEの構造][scale=0.65]

802.11に存在するいくつかのパワーセーブ機能では, 無線LAN機器が一時的に
@<b>{寝る(Doze)}ことにより消費電力の低減をはかっています.
この間, もちろんAPはその無線LANクライアントにデータフレームを送ることができません.
単純にこれをDROPしてしまうとTCP等の上位レイヤへの影響が大きいので,
パワーセーブ機能ではAPとクライアントが協力することで
@<b>{"一時的にペンディングする"}といったことが可能となっています.
APは, 寝ているクライアントに対してのフレームをバッファリングし起きてくるのを待ちます. 
このバッファリングを行っている間は,"そのクライアントに対して送りたいフレームがある"
旨をTIM IEに記載します.具体的には上図のビットマップフィールドにて,
そのクライアントのAssociation ID番目のビットを立てておきます.

クライアント側は@<b>{起きた(Awake)}時にそのAPから送信されるビーコンを拾います.
これにTIM IEが含まれており, かつ自分宛のフレームがあることを
ビットマップフィールドにて検知した場合, ポーリング(PS-Poll)してAPに
送信要求を出す or APがこちらが起きたことを検知して自発的に送ってくれるのを
待ちます.このような動作によりフレームロスを可能な限り発生させせずに
省電力を実現しています.

より詳しいパワーセーブ機能の中身については802.11-2012の10.2 Power managementを参
照してください.

==== Country IE

Country IEは, 無線LANを使おうとしている国での電波的な制限事項を伝えるために
使われます. APは基本的にこれをBeaconやProbe Responseに付加して送信します.
後述するIEEE 802.11dにもあるように, 使って良いチャネルや送信出力の上限は各国の
法規によって異なります. 無線LANのユーザはAPに適切な設定をすることで, これを
Country IEとして配布し, 接続してくる無線LANクライアントにその制限を
伝え設定を自動的に変える機会を与えます.

このIEの基本的な構造は@<img>{ie_country}にある様になっています.

//image[ie_country][Country IEの構造][scale=0.7]

Valueの先頭には3バイトの国文字列が入ります. 
たとえば, 日本であれば"JP " (最後にスペース)となります.
その後の三バイト分はチャネル/送信出力の制限を表すフィールドです.
それぞれ1バイトごとに「使っていい一番最初のチャネル」「最初のチャネルから,
使っていいチャネル数」「送信出力の上限」の3要素からなり, この組を任意の数だけ
並べることができます. この組の数によって最後にパディングを入れる場合があります.

==== ERP IE

Extended-Rate PHY IEは, 802.11bと802.11gの互換性維持のために用いられるIEです.
802.11bではDSSS(直接拡散方式)と呼ばれる変調技術を用いていましたが, 802.11gでは
速度向上のためにOFDM(直交周波数分割方式)を基本とするように変更されました.
このままでは802.11bな機器と802.11gな機器が通信できない上に, 同じ2.4GHz帯を
使っているにもかかわらず相互にノイズと認識しあうなどメディアの状態把握や
通信に影響がでてしまいます. これを防ぐために802.11gでは互換性のあるDSSS-OFDMと
非互換のERP-OFDMという二種類の方式を選択的に用いることとしています.

ERP IEはAPから送信されるBeaconやProbe Responseにおいて活用されることを
前提として, この判断のために使われます.
@<img>{ie_erp}にあるように, ERP IEにはこれを送ってきたAPの配下にERP非対応な機器
(つまり802.11b onlyなクライアント)がいるかどうか(Non ERP)や,
Protectionが必要かどうかなどの情報を格納します.

//image[ie_erp][ERP IEの構造][scale=0.7]

#@# ==== HT Capabilities IE
#@#
#@# //TODO
#@#
#@# ==== HT Information IE
#@#
#@# //TODO

==== RSN IE

Robust Security Network (RSN) IEは, これを含むBeaconやProbe Responseフレームを
送信しているAPにてサポートするWPA2の具体的なセキュリティ方式について
告知するために利用されます.
たとえば, 事前共有鍵で認証をしている(WPA2-PSK)のかそれとも認証サーバを使って
外部に問い合わせているのか(WPA2-EAP)であったり, ユニキャスト/マルチキャスト
のそれぞれについてどういった暗号化をサポートしているかといった情報がこれに
含まれます.

RSN IEは具体的に@<img>{ie_rsn}に示されるフレーム構造になっています.
Versionフィールド以降は基本的に省略可能なオプショナルフィールドとなっており,
途中までしかない場合があります. この構造の大本の仕様は802.11-2012の
Figure 8-136にあります@<fn>{rsnie}.
//footnote[rsnie][簡単化のためと, 基本的に登場しないことが多いためPMKIDに関する説明を今回は省略しています]

//image[ie_rsn][RSN IEの構造][scale=0.55]
#@# //table[tbl_rsn_ie][RSN IEの構造]{
#@# フィールド名			長さ(バイト)	内容
#@# ----------------------------------------------------
#@# Element ID			1	RSN IEであることのTag(0x30)
#@# Length				1	RSN IEのValue部分の長さ(可変長)
#@# Version				2	RSNのバージョン(802.12-2012では0x0001固定)
#@# Group Data Cipher Suite		4	グループ(マルチキャスト)に用いる暗号方式
#@# Pairwise Cipher Suite Count	2	サポートするペア暗号の数(m)
#@# Pairwise Cipher Suite List	4 * m	利用可能なペア(ユニキャスト)での暗号方式一覧
#@# AKM Suite Count			2	サポートする認証方式の数(n)
#@# AKM Suite List			4 * n	利用可能な認証方式一覧
#@# RSN Capabilities		2	ケーパビリティの設定
#@# //}

図中にも示されているように, グループ暗号スイート(Group Data Cipher Suite)が
一つしか格納できないのに対し, ペア暗号スイート(Pairwise Cipher Suite)と
AKM(Auth Key Management 認証キー管理)スイート(AKM Suite)は
Listとして複数個並べることを前提としています@<fn>{suitelist}.
//footnote[suitelist][もちろん一つだけにしてもかまいません(たいていの場合はひとつ)]

このリストにはそれぞれPairwise Cipher Suite CountおよびAKM Suite Countに
指定されている数の暗号スイートが並びます.
これは, APから複数の方式を並べて提示することでクライアントが
接続時に自分が使える好きな物を選択できる様にしているためです.

グループ/ペアで指定可能な各Cipher Suite(暗号スイート)は4バイトで,
中身はOUI 3バイトとスイートタイプ1バイトから構成されます.
それぞれ暗号化方式に紐付いており, 対応関係は@<table>{tbl_cipher_suite}に一覧
してあるようになっています.

//table[tbl_cipher_suite][主要な暗号スイート一覧]{
OUI(3バイト)	スイートタイプ(1バイト)	暗号方式
------------------------------------------------
00-0F-AC	0			グループのそれと同じ物を使う
00-0F-AC	1			WEP 40/64bit
00-0F-AC	2			TKIP
00-0F-AC	4			CCMP(AES) - これがデフォルト
00-0F-AC	5			WEP 104/128bit
00-0F-AC	7			マルチキャストは禁止
Vendor OUI	*			ベンダ独自
//}

ただし暗号スイートの制限として以下の条件があります

 * 00-0f-AC:0はグループ暗号スイートには指定できません
 * WEP (00-0f-AC:1, 00-0f-AC:5)はペア暗号スイートには指定できません

認証キー管理スイートも, フォーマットは暗号スイートと同様でありOUIとスイートタイ
プからなる4バイトのフィールドです. 主要なものを@<table>{tbl_akm_suite}に一覧
します.

//table[tbl_akm_suite][主要な認証キー管理スイート]{
OUI(3バイト)	スイートタイプ(1バイト)	認証方式
-------------------------------------------------
00-0F-AC	1			IEEE 802.1X認証 (WPA2-EAP)
00-0F-AC	2			PSK(事前共有鍵)
Vendor OUI	*			ベンダ独自
//}

典型的な, WPA2-PSKでCCMP(AES)を暗号化方式として用いている場合, Wiresharkでは
@<img>{rsnie}にあるようなRSN IEが観測できます.

//image[rsnie][WPA2-PSKでAES暗号を用いている場合のRSN IEの例]

==== Vendor Specific IE

Vendor Specific IEは, デバイスのベンダ固有の情報を詰めるために利用されます.
ベンダ各社で, 対応デバイス同士であれば有効になるような特殊な設定や通信方式を
サポート/相手との折衝をするために用いられます.
このため"どのベンダか?"を判別するために, Valueの先頭部分にはOUIを埋め込みます.
OUIは基本的に24bit (3byte) です@<fn>{oui}.
Vendor Specific IEは基本的にベンダで好き勝手に決めているため, Wiresharkで
パースできない場合があります.
//footnote[oui][36bitの場合もあり(OUI-36, IAB)]

よく使われる(?)Vendor Specific IEとして, WPAの情報を詰めたWPA Information
Elementがあります. このIEはMicrosoftのOUI (00-50-f2)を用いておりRSN IEと同様に
サポートしているUnicast/Multicastの暗号スイートを列挙しています.




== Reason Code 一覧
Disassociation/DeauthenticationフレームにはReason Codeなるフィールドが
あります。
これは, なんらか切断を要するような事象が発生した場合に, それがどういった
理由に基づいて行われたのかを伝えるために利用されます.
たとえば「通信が無かったためタイムアウトした」であるとか
「認証情報が食い違っている」といった場合があります.
この際に, 自身から見えている情報を伝えるために使われるのがReason Codeです.
ここではいくつかあるReason Codeの内, 主要な物を@<table>{tbl_reasoncode}に取り上げます.
大元の表は802.11-2012のTable 8-36にあります.

//table[tbl_reasoncode][802.11 Reason Code]{
番号		意味
-------------------------------------------
1		不明な理由
2		以前の認証情報で接続/通信しようとしている
3		電源や無線機能Offにより明示的に切断
4		アクティビティ(通信等)がなくなったためタイムアウト
5		これ以上のクライアントが接続できない場合
6		認証していないクライアントからL2のフレームが送られてきた
7		認証していないクライアントからL3のパケットが来た
8		これを送信した無線LAN機器が去ろうとしている(明示的に切断or無線Offなど)
15		4-Wayハンドシェイクがタイムアウト
16		Group Key ハンドシェイクがタイムアウト
17		4-Wayハンドシェイク時の要件がAssociation Requestや@<br>{}ProbeRequestやBeaconに記載されているものと異なる.@<br>{}(APが複数いる場合にクライアント数のロードバランシングのために使われる場合あり)
18		不正なGroup 暗号方式
19		不正なPairwise 暗号方式
23		IEEE 802.1X認証に失敗
//}
//pagebreak


== チャネルと周波数の対応表

ここではいわゆる@<b>{チャネル番号}とそのチャネルを指定した場合に中心となる
@<b>{周波数}の対応表を記載します.
基本的に2.4GHzは2412MHzをチャネル1, 5GHzは5180MHzをチャネル36であることを基本
としてチャネル番号1ごとに5MHz上がるだけなので手計算できるはずなのですが,
ぱっとみて対応が取れるようにあえて表を上げておきます.
基本的に日本で利用可能なチャネルのみになります.

//table[tbl_chanfreq24][2.4GHz帯のチャネルと周波数の対応表]{
2.4GHz帯チャネル	中心周波数(MHz)
---------------------------------------
1			2412
2			2417
3			2422
4			2427
5			2432
6			2437
7			2442
8			2447
9			2452
10			2457
11			2462
12			2467
13			2472
14			2484 (802.11bのみ)
//}

//table[tbl_chanfreq50][5.0GHz帯のチャネルと周波数の対応表]{
5GHz帯チャネル		中心周波数(MHz)
--------------------------------------
36			5180 (W52 屋内利用のみ)
40			5200 (W52 屋内利用のみ)
44			5220 (W52 屋内利用のみ)
48			5240 (W52 屋内利用のみ)
52			5260 (W53 屋内利用のみ)
56			5280 (W53 屋内利用のみ)
60			5300 (W53 屋内利用のみ)
64			5320 (W53 屋内利用のみ)
100			5500 (W56 屋外利用可)
104			5520 (W56 屋外利用可)
108			5540 (W56 屋外利用可)
112			5560 (W56 屋外利用可)
116			5580 (W56 屋外利用可)
120			5600 (W56 屋外利用可)
124			5620 (W56 屋外利用可)
128			5640 (W56 屋外利用可)
132			5660 (W56 屋外利用可)
136			5680 (W56 屋外利用可)
140			5700 (W56 屋外利用可)
//}

//pagebreak

== IEEE 802.11 一覧

ここでは今のところ世に出ているor出かけているIEEE 802.11のstandard/amendmentsを一覧します.
主要かつよく耳にする802.11, 802.11a, 802.11b, 802.11g, 802.11n そして今のところ
ドラフト段階にある噂の802.11acは言わずもがな感があるので省いています.

: 802.11c
	802.11にて, 802.1Dに記載されるブリッジ規格をサポートするための
	amendmentになります.
: 802.11d
	主に"Country Information Element"を導入するものです。
	国ごとに電波法規は異なり, その国で用いられるデバイスをこれに従う必要
	があります. ハードコーディングされている情報の変更や, ユーザによる
	煩雑な設定なしにこの差異に対処するために, APはこの国別の情報を付加した
	BeaconやProbe Responseを送信します. 各無線LANクライアントは
	これを参照することで自分が動作している国での制限を認識しこれに併せて
	PHYを調整できるようになります。
: 802.11e
	QoSやパワーセーブ機能に対する拡張を定義しています.
	従来の802.11にも電波というメディアにアクセスするための調停といった
	面でのQoSを備えていましたがこれをさらに拡張しフレームごとに優先度を
	付ける, 無線LAN機器ごとに送信機会を分けるといったことが可能になって
	います. 特に前者については802.1DでのUser Priorityと対応するように
	Background, Best Effort, Video, Voiceといった優先度付けを行えるように
	なっています. これらはフレームごとに付加され, QoS Data フレームの
	802.11ヘッダに含まれるQoS Controlフィールドにて確認することができます.
: 802.11F
	アクセスポイント間プロトコル(IAPP)なるものを定義しているものです。
	すでにobsoletedになっていますがhostpadにも実装されており,
	利用することが可能です。OpenBSDのhostapdではこれを用いて無線LANでの
	L3ローミングが可能となっています(hostapd(8)参照).
: 802.11h
	元々はヨーロッパでの5GHz帯利用(802.11a)に当たって, 衛星やレーダーとの
	干渉を回避する仕組みを導入するための仕様でした.
	このために, Dynamic Frequency Selection (DFS)を導入しレーダー波等が検知
	された場合は動作するチャネルを変えたり, 干渉を防ぐために使っている帯域
	全体での送信出力がその国の規定以下に収まるように制限する
	Transmit Power Control (TPC)といったものを導入しています.
	これらは現在ヨーロッパ以外でも5GHz帯での通信を行うにあたっては必須の
	ものとなっています.
: 802.11i
	802.11利用時のセキュリティについて言及したものです.
	いわゆるWPA2にあたるものの仕様を定義し, これを実現するために必要な
	各要素について言及しています. 802.11の頃にはすでにWEP, Wire Equivalent
	Privacyによるセキュリティメカニズムがありましたがいろいろとブロークン
	なのはご存じの通りで, これを置き換えることを目的としたものです.
	なおWPAという名前はWi-Fi Alliance側の名前であるため, 802.11-2012には
	一切出てこないためそこそこに混乱します. 802.11-2012では"Security"という
	章にあたる部分のもとになったものでもあります@<fn>{sec}.
: 802.11j
	802.11aでは5GHz帯の利用について定めていますが, 日本限りとして4.9GHz帯
	の利用について定めたのがこのamendmentになります.
: 802.11k
	電波資源管理(Radio Resource Management)を目的として, これを行うのに
	必要な情報のやりとりについて定めたamendmentになります.
	電波というメディアの特性上, 通信をスムーズに行うためにはあるAPとそれに
	接続する/しようとするクライアントの位置関係や数などを把握しながら
	適切なAPを選択/乗り換えるといったことを行う必要があります.
	APからこの判断の為の情報を配布できるように定めたのがこのamendmentに
	なります. この規格にてMeasurement Request/Report ElementがInformation 
	Elementとして追加されました.
: 802.11m
	仕様ではなく, 802.11のstandard/amendmentのメンテナンスを目的とした
	ワーキンググループになります.
: 802.11p
	主に自動車を対象として, 高度道路交通システム (ITS: Intelligent Transport
	System)を802.11を用いて実現するために, 近距離での車・車間通信を想定した
	拡張を盛り込んだamendmentになります.
: 802.11r
	ローミング性能を強化するためにFast BSS Transition (FT)という規格を
	導入するamendmentになります. 従来のローミングは, 802.11k等で得られる
	情報をクライアントが活用して適切なAPを探し出すといったものになっていまし
	た。このクライアントががんばる方式の問題点として, WPA/WPA2を利用している
	場合には再度4-Way Handshakeをしなければならないという点があります.
	これによるローミング速度低下を改善するために, 802.11rではローミング
	ドメイン内のAPが連携することでセキュリティを担保しつつ4-Way Handshakeを
	省略するといったことを実現しています.
: 802.11s
	いわゆるメッシュネットワークについて策定したものになります.
	従来よりアドホックモードでの通信というものがありましたが, これをさらに
	拡張しネットワーク全体が特定のAPによる中心性を保たない形態を取る
	ことができるようになりました.
: 802.11T
	仕様というよりは, 無線LANのテストやパフォーマンス測定などの手法の標準化
	を目的としたワーキンググループです. 現在はcancelledされており
	有効なものではありません.
: 802.11u
	ローミングはローミングでも, 802.11と携帯網(3GやLTE)と相互運用について
	定めたamendmentになります. いわゆるHotSpotや携帯網から無線LAN網への
	オフロードをスムーズに行うために策定されています.
: 802.11v
	802.11kと同様に, 無線LAN環境の効率化を目指したamendmentになります.
	802.11kがRadio Resource Management, つまり電波資源に着目しその状況に
	応じて切り替えを行うことを目的としていたのに対し, 802.11vでは
	Wireless Network Managementとして複数クライアントからなるネットワーク
	全体を管理することを目的としています.
: 802.11w
	このamendmentにてProtected Management Frameというものを導入しています.
	これまでの802.11ではWEP/WPA等でデータが入っているペイロードの保護や
	改ざん検知を行っていました. 一方でManagementフレームは基本的にそのまま
	送信されるため, うまく偽装すればDoSを行ったりできるためこのamendmentにて
	Managementフレームにも保護の範囲を広げています.
: 802.11y
	米国内で使える, 3650〜3700MHz帯での通信規格について定めたamendmentになり
	ます。
: 802.11aa
	802.11にて音楽, ビデオなどの転送を行うにあたって品質の強化を目的とした
	amendmentになります. 速度面では802.11nが, QoSにおいては802.11eが導入
	されましたが, マルチキャストによるビデオ配信や品質維持といった側面では
	まだまだ不足だとされており, これを改善するために策定されました
	@<fn>{11aa}.
: 802.11ad
	60GHz帯を用いたExtremely High Throughputと呼ばれる伝送規格について
	記述したamendmentになります. 802.11abgnおよび802.11acとはまったく
	異なる周波数帯を用いているため今のところ一般的に試せるデバイスは
	出回っていません.
: 802.11af
	いわゆるTVホワイトスペース, テレビ放送に使われるべく割り当てられている
	周波数帯のうち使われていないものを802.11にて用いることを想定して策定
	されているものになります.
: 802.11ah
	これまでの802.11の規格が高周波数高スループットを目標としていたのに対し
	1GHz以下の周波数帯を用いてセンサやメータなどの省電力な小型デバイスを
	接続することを目的としたamendmentになります.
: 802.11ai
	802.11rよりさらに早くローミングをできるようになることを目指した規格です.
	ハンドオーバ時のInitial Link Setup, つまりAPをさがして認証・鍵交換をして
	IPアドレスの取得を行って通信ができる状態になるまで, の時間を100ms以内に
	するという非常に野心的な目標をもってタスクグループが設置されています.

ここで上げた多くの仕様は, かつてはammendmentという形で個別に存在していましたが
現在は802.11-2007および802.11-2012に統合されており直接名前を目にする機会は
あまりありません.

//footnote[sec][200ページもある長編...]
//footnote[11aa]["IEEE 802.11aa: Improvements on video transmission over Wireless LANs"]

#@# == Nintendo DS検知！
#@# //lead{
#@# ここではちょっと脇道に逸れてNintendo DSの"すれ違い通信"における802.11の使われ方
#@# と, それを元にしたDS検知について紹介します.
#@# //}
#@# 
#@# //TODO
