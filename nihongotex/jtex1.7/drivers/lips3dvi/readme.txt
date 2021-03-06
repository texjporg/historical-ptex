			 【まずお読み下さい】

   lips3dvi は、TEX の出力であるDVI を、キヤノンの新型レーザービームプ
リンタの制御言語であるLIPS3に変換するプログラムです。つまり、このプリ
ンタドライバを使うことで、TEX の出力をLIPS3 を制御言語とするプリンタに
対して出力することができます。また、V2.07 からはエプソンのESC/Page に
対応したプログラムescpdvi も同時に提供されます。 このドキュメントの内
容は、 特別の記述が無い限り、 lips3dviとescpdvi の両方にあてはまります。
   lips3dvi は、 TEX のプリンタドライバとして、 以下のような特徴を持っ
ています。
     ☆ASCII の日本語TEX に対応(新版でpTEX にも対応した)。
     ☆欧文フォントには、 pk ならびにpxl フォントを使用可。 pxl フォン
       トでは、 ID が1002/1003 の圧縮形式も利用できる。
     ☆和文フォントには、 プリンタの内蔵スケーラブルフォント、 ならび
       に、 JXL4 フォーマットの大日本フォントを切り替えて利用できる。
     ☆欧文フォントを完全にプリンタにダウンロードしてしまうことにより、
       和文内蔵フォントとの組み合わせ使用では非常に高速。
     ☆landscape モードに対応。
     ☆印刷時に拡大・縮小の指定が可能。 ただし、 適当な大きさの欧文フォ
       ントが必要(和文フォントはスケーラブルなので無くても可)。
     ☆MS-DOS でも動作可能。
------------------------------------------------------------------------
全体構成

   現在の版のlips3dvi は、 UNIX での動作を主眼としていますので、 UNIX 
の標準的なプリンタスプーラを利用するために、 以下の3 つのプログラムで
構成されています。
     ☆フロントエンド・プログラム〜dvipr
     ☆バックエンド・プログラム〜lipsback
     ☆変換プログラム本体〜lips3dvi(escpdvi)

   これらの処理の流れをまとめますと、 次のようになります。

1.  dvipr が引数を解釈し、正しければ引数とDVI ファイル本体を1 つの出力
    ストリームにまとめて、lprに引き渡す。
2.  lpr は、 必要に応じてネットワークを経由した上で、 プリンタサーバー
    となるマシンの上でlipsbackを起動する。
3.  lipsback は入力ストリームを引数とDVI ファイル本体に分離し、 
    lips3dvi(escpdvi) を起動する。
4.  lips3dvi(escpdvi) は引数に従ってDVI ファイルをLIPS3(ESC/Page) に変
    換して出力する。

つまり、lpr はフィルタプログラムに対して引数を受け渡す方法を提供してい
ない事を回避するために、dviprとlipsback が引数をデータストリームの中に
押し込んで伝達する働きをするわけです。 lips3dvi(escpdvi)は、 それ単体
で全ての変換を行いますので、 lpr を利用する必要がないDOS 版では、 
lips3dvi(escpdvi) だけで動作します。

----------------------------------------------------------------------
配付ファイル一覧

	Makefile.dos	DOS用makefile
	Makefile.unx	UNIX用makefile
	version.h	ヘッダファイル
	config.h
	convert.h
	drive.h
	dvi.h
	font.h
	ktype.h
	lbp.h
	cache.c		lips3dviソースファイル
	doprint.c
	download.c
	fontlist.c
	jxlfont.c
	main.c
	pkfont.c
	pxlfont.c
	readdvi.c
	lbp.c		LIPS3用ルーチン
	lp3000.c(*)	ESC/Page用ルーチン
	dosprn.c	同(DOS版のみ)
	farfont.c
	ems.c(*)
	getopt.c
	dvipr.c		dviprソースファイル
	lipsback.c	lipsbackソースファイル
	patlist.c	おまけプログラムソースファイル
	disp.c		おまけプログラムソースファイル
	lips3dvi.cnf	lips3dviコンフィギュレーションファイル
	manual.tex	lips3dviマニュアル
	readme.txt	今読んでいるファイル
	update.txt	旧版からの変更点
	pageadj.tex	位置合わせ用サンプル
				(*)は岩本氏が著作権を保持しているもの。
----------------------------------------------------------------------
UNIXへのインストール

   lips3dvi をUNIX で利用する場合のインストール方法を以下に述べます。

● config.h の編集

☆マシンの選択
   config.h の先頭付近には、動作を確認したSun, NEWS に対応したシンボル
を定義する部分があります。インストールしたいマシンに対応するシンボルを
"1" に定義して下さい。

☆プリンタの選択
   LIPS3 とESC/Page の選択を行ないます。 先頭付近にあるシンボルLIPS3 
かESCP のいずれか一方を"1" に定義して下さい。

☆各ファイルのパス指定
   以下のファイルのデフォールトパスをサイトの状況に合わせて定義します。
複数のディレクトリやファイルを指定することはできません。

 FONTDIR      フォントファイル(グリフファイル) のあるディレクトリ名
 TFMDIR       TFM ファイルのあるディレクトリ名
 DOWNINFO     ダウンロードインフォファイルのフルパス(ファイル) 名
 CONFFILE     コンフィギュレーションファイルのフルパス(ファイル) 名
 MAINPROG     lips3dvi プログラム本体のフルパス(ファイル) 名
 LPRCMD       lpr コマンド
 LPRDEFOPT    lpr コマンドに必ず与えるオプション(-d)
 TMPFNAME     lipsback が作成する一時ファイルのベース名

☆サポートフォントの選択
   ファイルの中程にサポートするフォントによってシンボルを定義する部分
がありますので、サイトにインストールされているフォントに応じて、
PXLFONT ないしはPKFONT の少なくとも一方を"1" にします。両方を"1" にし
た場合には、先に述べたようにPXLFONT から探してゆきます。なお、UNIX 版
ではFARFONT フォーマットはサポートしていませんので、FARFONT は必ず0 に
しておきます。

☆日本語microTEX のサポート
   使う事はまずないと思いますが、日本語microTEX の出力したDVI ファイル
を印刷する事がある場合には、MICROTEX を"1" にします。

☆キャッシュサイズ
   JXL4 フォントを使って印刷行う場合、lips3dvi がキャッシングする文字
の数をCACHE_SIZE に指定します。SPARCstaion で5000 を指定した場合には、
lips3dvi は約700KB の仮想メモリを使用します。

● コンパイルとインストール
  Makefile.unx を使って、以下のようにmake します。いずれも、ネイティブ
のC コンパイラでしかテストしていません。
    % make -f Makefile.unx lips      (LIPS3 の時)
    % make -f Makefile.unx escp      (ESC/Page の時)

  コンパイルに成功したら、dvipr, lipsback, lips3dvi(escpdvi) を適当な
ディレクトリにインストールします。
    % make -f Makefile.unx install

● フォントのインストール
   lips3dvi(escpdvi) で出力を行なうには、少なくとも欧文のフォントが必
要です。アスキーよりのjTEX のディストリビューションには、欧文フォント
が含まれていませんので、オリジナルTEX のディストリビューションからイン
ストールを行ないます。初めてインストールするのであれば、とりあえず以下
のディレクトリにある全てのファイルを適当なディレクトリ
("/usr/local/lib/tex/pkfont" など) にコピーします。

    tex2.99/cmfonts/pk/pk300/*
    tex2.99/LaTeXfonts/pk/pk300/*
    tex2.99/utilityfont/pk/pk300/*

   色々なフォントを使っているドキュメントを出力するには、これだけのフォ
ントでは不足ですが、とりあえずインストールを完了するには十分でしょう。
フォントに関する詳細は、付録と参考文献を参照してください。

● printcap の設定
   lips3dvi は、フォントをバイナリでダウンロードしますので、lpr は8 ビッ
トをクリーンに出力できるように設定しなくてはなりません。lips3dvi がう
まく動かないとの問い合わせの大部分は、printcap の設定が8 ビットクリー
ンになっていないことが原因でした。
   ここで「8 ビットクリーン」と言うのは、0x00 から0xff までの全ての文
字が「そのまま」プリンタに転送されるということを意味します。LF がCR+LF 
に変換されたり、タブが空白に展開されたりしてはいけません。
   一般論として、printcap が上記のような設定になっていることを確認する
には、次のような手順を取るとよいでしょう。lips3dvi がうまく動かない時
には、まず次の手順でprintcap の設定を確認して下さい。

   1.  0x00 から0xff までの256 バイトから成るファイルを作成する。
   2.  プリンタを16 進ダンプモードにする。
   3.  フィルタを使わずに(printcap のif フィールドを空にして)、そのファ
       イルをプリンタに出力してみる。
   4.  16 進ダンプの結果を見て、すべてのバイトがそのまま出力されている
       ことを確認する。特に0x08(TAB)と0x0a(LF)、0x0d(CR) に注意する。

   8 ビットクリーンになっていることを確認したならば、DVI ファイルの出
力フィルタとして、lipsback を指定します。つまり、LaserShot のエントリ
に次のような項目を追加します。

                   df = /usr/local/lib/tex/lipsback

なお、lipsback はアカウンティングの機能は持っていません。

   SunOS 4.1 において、lips3dvi を利用する際のprintcap の例を示します。
お使いになっている他のフィルタ群とのかねあいもありますので、printcap 
ならびに他のフィルタ群のマニュアルを参照して設定を行なって下さい。

      lp_ls_Cannon  LaserShot2:\
                 :lp=/dev/ttya:sd=/var/spool/lpd:br#19200:\
                 :fs#06020:fc#300:ms=-opost,-parity,ixon:\
                 :df=/usr/local/lib/tex/lipsback:sh:\
                 :if=/usr/local/lib/lips3text:

● 他の機種の場合
   lips3dvi は、System V でUNIX を覚えたプログラマが「MS-DOS でも動か
そう」と考えながら作ったものです。System V のライブラリルーチンが多数
使われています。デバッグ用のものを除く全ての記号定数はconfig.h にあり
ますので、これをよく読んで適当なdefine を切って下さい。健闘をお祈りい
たします。

---------------------------------------------------------------------
DOSへのインストール

   lips3dvi をMS-DOS で利用する場合のインストール方法を以下に述べます。
なお、MS-DOS 版はMicrosoft C compiler V 6.0 を前提としています。他の処
理系でのテストは行っていません。また、Makefile.dosは、C コンパイラに付
属のnmake を想定しています。

● config.h の編集

☆マシンの選択
   config.h の先頭付近には、動作を確認した PC98, J3100, DOS/V に対応し
たシンボルを定義する部分があります。インストールしたいマシンに対応する
シンボルを"1" に定義して下さい。MS-DOS 版のlips3dvi(escpdvi) では、
BIOS を呼び出し、またはハードウェアポートを直接制御することでプリンタ
にデータを送りますので、オブジェクトは機種依存です。

☆プリンタの選択
   LIPS3 とESC/Page の選択を行ないます。 先頭付近にあるシンボルLIPS3 
かESCP のいずれか一方を"1" に定義して下さい。

☆EMS サポートの有無
   V2.07 から、 EMS サポート機能が追加されました。 欧文フォントのビッ
トマップをEMS メモリを使って展開することができます。 この機能を有効に
する場合には、 USEEMS を"1" にします。

☆各ファイルのパス指定

   以下のファイルのデフォールトパスをサイトの状況に合わせて定義します。
複数のディレクトリやファイルを指定することはできません。

 FONTDIR      フォントファイル(グリフファイル) のあるディレクトリ名
 TFMDIR       TFM ファイルのあるディレクトリ名
 DOWNINFO     ダウンロードインフォファイルのフルパス(ファイル) 名
 CONFFILE     コンフィギュレーションファイルのフルパス(ファイル) 名

☆サポートフォントの選択

   ファイルの中程にサポートするフォントによってシンボルを定義する部分
がありますので、マシンにインストールされているフォントに応じて、
PXLFONT ないしはPKFONT の少なくとも一方を"1" にします。FAR フォントアー
カイブを利用する場合には、PXLFONT とFARFONT を"1" にしなくてはなりませ
ん。複数の形式に"1" を指定した場合には、先に述べたようにFARFONT から探
してゆきます。

☆日本語microTEX のサポート
   日本語microTEX の出力したDVI ファイルを印刷する事がある場合には、
MICROTEX を"1" にします。

☆キャッシュサイズ
   JXL4 フォントを使って印刷行う場合、lips3dvi がキャッシングする文字
の数をCACHE_SIZE に指定します。プリンタ側が絶対的に律速となりますので、
大きくしたからと言って早くなるものでもありません。

● コンパイルとインストール
  MS-C 6.0 に付属のnmake.exe とMakefile.dos を使って次のようにmake し
ます。
    c:>nmake -f makefile.dos lips      (LIPS3 の時)
    c:>nmake -f makefile.dos escp      (ESC/Page の時)

nmake は大量のメモリを消費するので、コンパイラの作業領域が取れない場合
があります。Makefile.dosの中にnamke に固有の記述はありませんので、その
ような場合にはUNIX 互換のmake ユーティリティを使用してください。
  コンパイルに成功したら、lips3dvi を適当なディレクトリにコピーします。

● フォントのインストール
   TEX における一般的なフォントの命名規則は、DOS におけるファイル名の
制限(ベース名8 文字+拡張子3 文字) を越えてしまいます。そのためlips3dvi 
では、次のようなフォント命名規則を使っています。

   通常のTEX におけるフォントの名前は、
       <フォント名><デザインサイズ>.<解像度>pk	(PK フォント)
       <フォント名><デザインサイズ>.<拡大率>pxl	(pxl フォント)
となっています。解像度とは、フォントの大きさを示す数値だと考えればよい
でしょう。その解像度を5倍したものを拡大率と呼びます。

   pk フォントの場合、
   1.  "<解像度>pk" を名前とするサブディレクトリを作成する。
   2.  フォントファイルの名前を<解像度>を取り去った名前に変更する。
   3.  1) で作成したサブディレクトリにファイルをコピーする。
例えば、"cmr10.300pk" の場合であれば、"300pk" というサブディレクトリに
"cmr10.pk" という名前のファイルとしてコピーします。また、"cms8.360pk" 
の場合であれば、"360pk" というサブディレクトリに"cms8.pk" という名前で
コピーします。TEX ディストリビューションから、下に記した全てのPK フォ
ントファイルを、名前を変えて適当なサブディレクトリにコピーして下さい。

   pxl フォントの場合、
   1.  "<拡大率>pxl" を名前とするサブディレクトリを作成する。
   2.  フォントファイルの名前を<拡大率>を取り去った名前に変更する。
   3.  1) で作成したサブディレクトリにファイルをコピーする。
例えば、"cmr10.1500pxl" の場合であれば、"1500pxl" というサブディレクト
リに"cmr10.pxl" という名前のファイルとしてコピーします。また、
"cms8.1800pxl" の場合であれば、"1800pxl" というサブディレクトリに
"cms8.pxl" という名前でコピーします。

   lips3dvi で出力を行なうには、少なくとも欧文のフォントが必要です。ア
スキーよりのjTEX のディストリビューションには、欧文フォントが含まれて
いませんので、オリジナルTEX のディストリビューションからインストールを
行ないます。初めてインストールするのであれば、とりあえず以下のディレク
トリにある全てのファイルをインストールすればよいでしょう。

    tex2.99/cmfonts/pk/pk300/*
    tex2.99/LaTeXfonts/pk/pk300/*
    tex2.99/utilityfont/pk/pk300/*

   上記のPK フォントをインストールする場合には、上記のように解像度毎に
サブディレクトリを作成し、フォントファイルの名前を変更してコピーして下
さい。
   色々なフォントを使っているドキュメントを出力するには、これだけのフォ
ントでは不足ですが、とりあえずインストールを完了するには十分でしょう。
フォントに関する詳細は、付録と参考文献を参照してください。

● 他の機種の場合
   機種に依存するBIOS 呼び出しなどは、dosprn.c なるファイルにのみ存在
しています。必要な機能は
    プリンタBIOS の初期化
    プリンタへの1 バイト出力
だけですので、移植は容易なはずです。健闘を祈ります。

----------------------------------------------------------------------
コンフィギュレーションファイルの設定

   同梱のlips3dvi.cnf には、コンフィギュレーションファイルで利用できる
全てのコマンドが書かれています。コメントになっている項目は、通常は指定
しません。lips3dvi の説明をよく読んで、各項目を適当に設定して下さい。

   porth_base, portv_base, landh_base, landv_base の4 つは、ほとんどの
場合、デフォールト値では不適当です。実際に印刷を行ってみて、印字位置の
左上隅が縦横とも1inch になるようにそれぞれの値を設定します。具体的な設
定方法については、"pageadj.tex" を参照してください。

   ゴチックのプロポーショナルフォントカードをお持ちでない場合には、

      FONT           goth          J80W22Y

を指定します。もちろん、この場合はゴチック指定があっても明朝体で印刷さ
れます。また、丸ゴチックカードをお持ちの場合は`J82W2Y' を指定します。
角ゴチックと丸ゴチックの両方をお持ちの場合は好きな方を選んで下さい。本
来の大日本フォントは、どちらかと言えば角ゴチックに近い書体です。

   プリンタのメモリが少ない場合(A406S やA404 にて0.5MB しか増設してい
ない場合や、 A406E でメモリを増設していない場合) には、downloadsize を
指定する必要があるかも知れません。大きな(色々な英文フォントを利用した) 
ファイルを印刷してみて、「ダウンロードオーバー」が表示されないように、
適当な値を設定して下さい。なお、downloadsize に小さな値を設定すると、
プリンタへダウンロードされるデータは減りますが、その分だけlips3dvi が
消費するメモリの量が増えます。DOS では動かなくなるかも知れませんので御
注意下さい。

----------------------------------------------------------------------
Copyrights

   lips3dvi は、 (株) アスキー出版局出版技術部の依頼をうけて(有) エヌ・
エス・プランニングが作成したもので、 著作権は(株) アスキーが保持してい
ます。 また、ESC/Page対応ならびにEMSのサポートは、岩本明氏によって行な
われました。岩本氏による改造部分の著作権は岩本氏が保持していますが、利
用・配付などの条件は以下に準じます。

 ☆ その使用が直接的な営利目的（たとえば、利用者にプログラムの使用料金
    をとる等）でない限り、どのような方法、目的で使用してもかまいません。

 ☆ 他者へのコピーは、その行為が営利目的でない限り、自由に行うことがで
    きます。

 ☆ プログラムの改変については自由に行うことができます。ただし、他者へ
    のコピーに関しては、改変前のもの、あるいは改変前の状態に簡単に戻せ
    る形で行ってください。改変の内容についてはアスキーへの通知義務があ
    るものとし、その公開については事前の承諾を必要とします。

lips3dviならびにその関連プログラムの利用に起因して利用者が何らかの損害
を被った場合であっても、(株)アスキーならびに (有)エヌ・エス・プランニ
ング、岩本明は一切の責を負いません。

----------------------------------------------------------------
lips3dvi に関する問い合わせは、下記までお願いします。なお、お電話によ
るお問い合わせは、業務の円滑な流れを阻害し、結果的にソフトウェアを無償
で提供することができなくなることがありますので、ご遠慮くださいますよう
お願いいたします。

                        株式会社アスキー
                        出版局 電子編集研究統轄部 出版技術部

                        濱野 尚人（hisato-h@ascii.co.jp）
				  (HCG00743  Nifty)

                        〒107-24 港区 南青山 6-12-1 大仁堂ビル 6F
                        FAX: 03 (486) 4988

