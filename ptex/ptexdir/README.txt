===========================================================================
日本語 TeX バージョン 3.14159, p2.1.10
===========================================================================
※ 必要ならば、インストール作業をする前に、
   旧バージョンの TeX システムのバックアップをしてください。

---------------------------------------------------------------------------
● ソースファイル
---------------------------------------------------------------------------
pTeX のインストールには、つぎのファイルが必要です。

 * ptex-src-2.1.10.tar.gz     (pTeX ソースファイル)
 * ptex-texmf-1.8.tar.gz      (pTeX ライブラリファイル)
 * teTeX-src-1.0.7.tar.gz     (teTeX ソースファイル)
 * teTeX-texmf-1.0.2.tar.gz   (teTeX ライブラリファイル)

これらは、以下のサイトなどから入手可能です。

・pTeX 関連
    ftp://ftp.ascii.co.jp/pub/TeX/ascii-ptex/
    ftp://ftp.media.kyoto-u.ac.jp/TeX/ASCII-pTeX/
    ftp://bash.cc.keio.ac.jp/pub/TeX/ascii-ptex/
    ftp://ftp.cs.titech.ac.jp/pub/text/TeX/ASCII-pTeX/
    ftp://ftp.tut.ac.jp/TeX/ASCII-pTeX/
    ftp://ftp.u-aizu.ac.jp/pub/tex/ASCII-pTeX/
    ftp://ftp.foretune.co.jp/pub/TeX/ascii-ptex/

・teTeX 関連
    ftp://ftp.lab.kdd.co.jp/CTAN/
    ftp://ftp.meisei-u.ac.jp/pub/CTAN/
    ftp://ftp.riken.go.jp/pub/tex-archive/
    ftp://ftp.u-aizu.ac.jp/pub/tex/CTAN/
    ftp://sunsite.sut.ac.jp/pub/archives/ctan/
    ftp://ftp.dante.de/tex-archive/
    ftp://ftp.tex.ac.uk/tex-archive/
    ftp://ctan.tug.org/tex-archive/

なお、teTeX-src のかわりに、

 * ftp://ftp.cstug.cz/pub/tex/local/cstug/thanh/pdftex/tetex-supp.tgz

を用いると、src-special 機能を利用できます。

---------------------------------------------------------------------------
● インストール
---------------------------------------------------------------------------
1. teTeX のライブラリファイルを展開します。

     mkdir -p /usr/local/teTeX/share/texmf
     tar zxf teTeX-texmf-1.0.2.tar.gz -C /usr/local/teTeX/share/texmf

2. ptex のライブラリファイルを展開します。

     tar zxf ptex-texmf-1.8.tar.gz -C /usr/local/teTeX/share/texmf

3. teTeX のソースファイルを展開します。

     tar zxf teTeX-src-1.0.7.tar.gz -C /usr/src

4. ptex のソースファイルを展開します。

     tar zxvf ptex-src-2.1.9-1.tar.gz -C /usr/src/teTeX-1.0/texk/web2c

5. ソースファイルを展開したディレクトリに移動し、configure コマンドを
   実行します。prefix オプションには、teTeX ライブラリをインストール
   したディレクトリを指定します。

     cd /usr/src/teTeX-1.0
     ./configure --prefix=/usr/local/teTeX
	 make
	 make install

6. ptex のソースディレクトリに移動し、configure コマンドを実行します。
   このとき、端末に出力する漢字コードを指定してください。

     cd /usr/src/teTeX-1.0/texk/web2c/ptex-2.1.10
     ./configure EUC     (EUCコードの場合)
     ./configure JIS     (JISコードの場合)
     ./configure SJIS    (シフトJISコードの場合)
	 make
	 make install

---------------------------------------------------------------------------
自動生成される ascgrp, ascii10, ascii36 の pk ファイルを
$TEXMF/fonts/pk/<モード>/ascgrp の下に置くようにする場合は、
$TEXMF/fontname/special.map につぎの二行を加えます。

    ascgrp ascgrp
    ascii  ascgrp

---------------------------------------------------------------------------
● 問い合わせ先など
---------------------------------------------------------------------------
pTeX についてのお問い合わせは、電子メールで www-ptex@ascii.co.jp 宛て
にお願いします。

pTeX の WEB ページ（http://www.ascii.co.jp/pb/ptex/）では、バグ情報や
インストールのヒント、pTeX で追加されたプリミティブ一覧などを掲載して
います。こちらもご覧ください。

-------------------------------------
中野 賢 (ken-na@ascii.co.jp)
株式会社 アスキー 出版技術部
=============================================================== EOT =====
