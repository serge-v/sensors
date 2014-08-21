#set -x

# based on instructions from
# http://www.nongnu.org/avr-libc/user-manual/install_tools.html

function atexit()
{
    echo -e '\a' # flash putty window
}

trap atexit EXIT

VER=4.9.1
PREFIX=$HOME/local/avr-gcc

function build_dep()
{
	fname=$(basename $1)
	dname=${fname%.tar.*}
	[[ ! -f $fname ]] && wget $1
	[[ ! -d $dname ]] && tar -xf ${fname}
	[[ ! -d $dname ]] && { echo cannot find $dname; exit; }
	cd $dname
	if [[ ! -f Makefile ]] ; then
		shift
		./configure $*
	fi
	make
	make install
	cd -
}

function build_deps()
{
	pushd ~/srcr

	build_dep ftp://ftp.gmplib.org/pub/gmp-5.0.5/gmp-5.0.5.tar.bz2 \
		--prefix=$(pwd)/gcc-deps \
		--disable-shared

	build_dep http://www.mpfr.org/mpfr-current/mpfr-3.1.2.tar.gz \
		--prefix=$(pwd)/gcc-deps \
		--disable-shared \
		--with-gmp=$(pwd)/gcc-deps

	build_dep http://www.multiprecision.org/mpc/download/mpc-1.0.tar.gz \
		--disable-shared \
		--prefix=$(pwd)/gcc-deps \
		--with-gmp=$(pwd)/gcc-deps \
		--with-mpfr=$(pwd)/gcc-deps
	
	popd
}

function build_binutils()
{
	pushd ~/srcr

	BINUTILS=binutils-2.24

	[[ ! -f $BINUTILS.tar.gz ]] && wget http://ftp.gnu.org/gnu/binutils/$BINUTILS.tar.gz
	[[ ! -d $BINUTILS ]] && tar -xf $BINUTILS.tar.gz
	[[ ! -d binutils-build ]]  && mkdir binutils-build
	cd binutils-build
	../$BINUTILS/configure --prefix=$PREFIX --disable-nls --target=avr
	make
	make install
	cd -
	
	popd
}

function build_avr_gcc()
{
	pushd ~/srcr/

	[[ ! -f gcc-$VER.tar.bz2 ]] && wget http://mirror.rit.edu/gnu/gcc/gcc-$VER/gcc-$VER.tar.bz2
	[[ ! -d gcc-$VER ]] && tar -xf gcc-$VER.tar.bz2
	[[ ! -d avr-gcc-$VER ]] && mkdir avr-gcc-$VER

	cd avr-gcc-$VER

	PATH=$PATH:$PREFIX/bin # path for avr bintools
	export PATH

	# --build: the machine you are building on
	# --host: the machine you are building for
	# --target: the machine that GCC will produce binary for

	../gcc-$VER/configure --target=avr --prefix=$PREFIX \
	--enable-languages=c \
	--disable-nls --disable-shared \
	--with-dwarf2 --with-avrlibc=yes \
	--with-mpfr=$(pwd)/../gcc-deps \
	--with-gmp=$(pwd)/../gcc-deps  \
	--with-mpc=$(pwd)/../gcc-deps
	make
	make install
	popd
}

function build_avr_libc()
{
	PATH=$PATH:$PREFIX/bin # path for avr bintools
	export PATH
	AVRLIBC=avr-libc-1.8.1
	pushd ~/srcr/
	[[ ! -f $AVRLIBC.tar.bz2 ]] && wget http://download.savannah.gnu.org/releases/avr-libc/$AVRLIBC.tar.bz2
	[[ ! -d $AVRLIBC ]] && tar -xf $AVRLIBC.tar.bz2
	cd $AVRLIBC
	./configure --prefix=$PREFIX --host=avr --with-debug-info=dwarf2
	make
	make install
	popd
}

#build_deps
#build_binutils
#build_avr_gcc
#build_avr_libc

echo update path with: PATH=\$PATH:$PREFIX/bin

