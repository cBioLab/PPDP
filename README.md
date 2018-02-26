# **PPDP**

**English →　README_EN.md**

## 概要
"加法準同型暗号を用いた編集距離の秘匿計算" (2016年度卒論[神保])の実装。サーバとクライアントが互いに秘密の文字列を所持している時、互いの文字列を公開することなく二つの文字列の編集距離を計算することができる。

## 動作環境
+ Intel 64-bit CPUs
+ 64-bit Linux (tested on ubuntu 14.04 LTS)

## 準備
作業用ディレクトリを作成し、以下のリポジトリをダウンロードする。

       mkdir work
       cd work
       git clone git://github.com/cBioLab/PPDP.git
       git clone git://github.com/iskana/PBWT-sec.git
       git clone git://github.com/herumi/xbyak.git
       git clone git://github.com/herumi/mcl.git
       git clone git://github.com/herumi/cybozulib.git
       git clone git://github.com/herumi/cybozulib_ext.git

OpenSSL と libgmp-dev を別途インストールする必要がある。  
       sudo apt-get install libssl-dev      
       sudo apt-get install libgmp-dev

## インストール方法
ライブラリの最新版には対応していないので、下記のバージョンを利用する必要がある。

       cd mcl
       git checkout b272c9166741926f959666db9ab21915216bf6cf
       cd ..
       cd cybozulib
       git checkout 05a4c2591fd9149ed766a991c2aaeefc07a5854a
       cd ..
       cd cybozulib_ext
       git checkout 4ca1a044eecf18107f93a3c9f25ce0a8e3efe05b
       cd ..
       cd xbyak
       git checkout b6133a02dd6b7116bea31d0e6b7142bf97f071aa
       cd ../PPDP/src
       make

## サンプルの実行
文字種数2の同じ長さの文字列同士の編集距離を計算するサンプルの実行コマンド。先にサーバを起動し、後からクライアントを起動する必要がある。

### サーバ側
    cd bin
    ./servertest.sh

### クライアント側
    cd bin
    ./clienttest.sh

## 使い方
先にサーバを起動し、後からクライアントを起動する。サーバ、クライアントの起動コマンドは以下の通り。

### サーバ側
    cd bin
    ./server -p <ポート番号> -c <スレッド数> -f <文字列ファイル> -s <文字種数>

### クライアント側
    cd bin
    ./client -h <サーバのipアドレス>  -p <ポート番号> -c <スレッド数> -f <文字列ファイル> -s <文字種数>

具体例はサンプルの実行の項目を参照

# 著作権表示
Copyright (C) 2018, cBioLab
All rights reserved.

# 連絡先
* 神保 元脩 (jimwase@asagi.waseda.jp)
