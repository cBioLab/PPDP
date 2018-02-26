# **PPDP**

## 概要
二者が互いの文字列を秘匿したまま類似度を計算することができる。

## 準備
作業用ディレクトリを作成し、以下のリポジトリをダウンロードしてください。

       mkdir work
       cd work
       git clone git://github.com/cBioLab/PPDP.git
       git clone git://github.com/iskana/PBWT-sec.git
       git clone git://github.com/herumi/xbyak.git
       git clone git://github.com/herumi/mcl.git
       git clone git://github.com/herumi/cybozulib.git
       git clone git://github.com/herumi/cybozulib_ext.git

* OpenSSL と libgmp-dev を別途インストールする必要があります。 

## インストール方法
ライブラリの最新版には対応していないので、下記のバージョンを利用してください。

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

## 作成者情報など
