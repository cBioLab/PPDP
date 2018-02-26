# **PPDP**

### 卒論用リポジトリ

# Installation Requirements

Create a working directory (e.g., work) and clone the following repositories.

       mkdir work
       cd work
       git clone git://github.com/cBioLab/PPDP.git
       git clone git://github.com/iskana/PBWT-sec.git
       git clone git://github.com/herumi/xbyak.git
       git clone git://github.com/herumi/mcl.git
       git clone git://github.com/herumi/cybozulib.git
       git clone git://github.com/herumi/cybozulib_ext.git

* Xbyak is a prerequisite for optimizing the operations in the finite field on Intel CPUs.
* OpenSSL and libgmp-dev are available via apt-get (or other similar commands).

# Installation
       cd mcl
       git checkout b272c9166741926f959666db9ab21915216bf6cf
       cd ..
       cd cybozulib
       git checkout 05a4c2591fd9149ed766a991c2aaeefc07a5854a
       cd ..
       cd cybozulib_ext
       git checkout 4ca1a044eecf18107f93a3c9f25ce0a8e3efe05b
       cd ..
       cd xbyak.git
       git checkout b6133a02dd6b7116bea31d0e6b7142bf97f071aa
       cd ..
       cd PPDP/src
       make

