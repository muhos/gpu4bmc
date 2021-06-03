#!/bin/bash

(return 0 2>/dev/null) && srced=1 || srced=0

ch='|'
lineWidth=90
logdir=logs
mkdir -p $logdir

logfile=bmc_install.log
echo -n > $logfile

# general functions
usage () {
printf "+%${lineWidth}s+\n" |tr ' ' '-'
cat << EOF
$ch usage: install.sh [ <option> ... ]
$ch 
$ch where '<option>' is one of the following
$ch
$ch	-h or --help          	print this usage summary 
$ch	-c or --cpu           	install CBMC + ParaFROST-CPU solver
$ch	-g or --gpu           	install CBMC + ParaFROST-GPU solver
$ch	-n or --less          	print less verbose messages
$ch	-u or --update         	compile existing source code which
$ch                         can only work with GPU compilation
$ch	-q or --quiet         	be quiet
$ch	-d or --download        download only then patch CBMC
$ch	-r or --remove          remove source code and all created folders
$ch	--config=<target>     	set the configuration file location
$ch                        	to <target> (should be a full path)
$ch                       	starting from root
$ch	--clean=<target>      	remove completely old installation of
$ch                       	CBMC <cpu | gpu | all | solvers> builds
$ch                       	all targets cannot be combined with other
$ch                       	options except for <solvers>
EOF
printf "+%${lineWidth}s+\n" |tr ' ' '-'
[ $srced = 1 ] && return 0 || exit 0
}

error () {
printf "$ch error: %-$((lineWidth - 1))s\n" "$1"
printf "$ch %${lineWidth}s+\n" |tr ' ' '-'
[ $srced = 1 ] && return 1 || exit 1
}

#---------
# options
#---------
noverb=0
quiet=0
donly=0
update=0
removeall=0
icpu=0
igpu=0
config=
clean=""

while [ $# -gt 0 ]
do
  case $1 in

    -h|--help) usage;;
	-n|--less) noverb=1;;
	-q|--quiet) quiet=1;;	
	-u|--update) update=1;;	
	-r|--remove) removeall=1;;
	-d|--download) donly=1;;
	-c|--cpu) icpu=1;;
	-g|--gpu) igpu=1;;
	
	--config=*)
	  config=${1#*=}
      ;;

	--clean=*)
	  clean="${1#*=}"
      ;;

    *) error "invalid option '$1' (use '-h' for help)";;

  esac
  shift
done

[ $update = 1 ] && [ $icpu = 1 ] && error "update only works with the GPU compilation"

if [[ "$clean" != "" ]] && [[ "$clean" != "solvers" ]] && [[ "$clean" != "cpu" ]] && [[ "$clean" != "gpu" ]] && [[ "$clean" != "all" ]]; then 
	error "invalid clean target '$clean'"
fi

pfbanner () {
if [ $noverb = 0 ] && [ $quiet = 0 ]; then
	printf "+%${lineWidth}s+\n" |tr ' ' '-'
	printf "$ch %-$((lineWidth - 1))s$ch\n"  \
	" GPU4BMC: GPU-Accelerated CBMC + ParaFROST installer (use -h for options)"
	printf "+%${lineWidth}s+\n" |tr ' ' '-'
fi
}

pffinal () {
if [ $noverb = 0 ] && [ $quiet = 0 ]; then
	log "Notes:"
	log " - logs has CBMC and ParaFROST installation logs"
	log " - check build for installed CBMC binaries" 
	printf "+%${lineWidth}s+\n" |tr ' ' '-'
fi
}

ruler () {
if [ $noverb = 0 ] && [ $quiet = 0 ]; then
	echo -n $ch
	printf "%${lineWidth}s+\n" |tr ' ' '-'
fi
}

log () {
if [ $noverb = 0 ] && [ $quiet = 0 ]; then
	printf "$ch %-$((lineWidth - 1))s\n" "$1"
fi
}

logn () {
if [ $noverb = 0 ] && [ $quiet = 0 ]; then
	printf "$ch %s" "$1"
fi
}

endline () {
if [ $noverb = 0 ] && [ $quiet = 0 ]; then
	echo "done."
fi
}

# banner
pfbanner

bmcdir=cbmc
archdir=archives
builddir=build
bmcsrc=$bmcdir/src
bmc_solvers=$bmcsrc/solvers
bmc_sat=$bmc_solvers/sat
bmc_cbmc=$bmcsrc/cbmc
bmc_gotoin=$bmcsrc/goto-instrument
cbmccpu=$builddir/cbmc_parafrost_cpu
cbmcgpu=$builddir/cbmc_parafrost_gpu

# check removal option first
if [ $removeall = 1 ]; then
	logn "removing all created directory.."
	rm -rf $bmcdir
	rm -rf $builddir
	rm -rf $archdir
	rm -rf $logdir
	rm -f $logfile
	endline
	ruler
fi

movelogs() {
[ -f $logfile ] && mv $logfile $logdir/cbmc_install.log
[ -f $bmcdir/parafrost/install.log ] && cp $bmcdir/parafrost/install.log $logdir/parafrost_install.log
}

cleanup () {
	if [[ "$1" == "cpu" ]]; then 
		rm -rf $cbmccpu
	elif [[ "$1" == "gpu" ]]; then
		rm -rf $cbmcgpu;
	fi
	if [ -d $bmcdir/parafrost ]; then
		cd $bmcdir/parafrost
		./install.sh --clean=$1 -q
		cd ../../
	fi
	[ -d $bmcdir/src ] && make -C $bmcdir/src clean &>> $logfile
}

# cbmc cleaning
if [[ "$clean" = "solvers" ]]; then
	logn "cleaning up CBMC solvers directory.."
	[ -d $bmcdir/src ] && make -C $bmcdir/src/solvers clean &>> $logfile
	endline
	ruler
else 
	cleanCPU=0
	cleanGPU=0
	if [[ "$clean" = "cpu" ]] || [[ "$clean" = "all" ]]; then
		cleanCPU=1; logn "cleaning up CBMC + PFCPU.."; cleanup cpu; endline; ruler
	fi
	if [[ "$clean" = "gpu" ]] || [[ "$clean" = "all" ]]; then
		cleanGPU=1; logn "cleaning up CBMC + PFGPU.."; cleanup gpu; endline; ruler
	fi
	[ $cleanCPU = 1 ] || [ $cleanGPU = 1 ] && [ -f $logfile ] && mv $logfile $logdir/cbmc_install.log
	if [ $icpu = 0 ] && [ $igpu = 0 ] && [ $donly = 0 ]; then [ $srced = 1 ] && return 0 || exit 0; fi
fi

# install
unset PARAFROSTCPU
unset PARAFROSTGPU
mkdir -p $archdir

cbmc_rel=5.31.0
cbmc_zip=cbmc-${cbmc_rel}.zip

if [ ! -f $archdir/$cbmc_zip ]; then
	logn "Downloading CBMC version $cbmc_rel.."
	wget -q https://github.com/diffblue/cbmc/archive/$cbmc_zip
	[ ! -f $cbmc_zip ] && error "could not download $cbmc_zip"
	mv $cbmc_zip $archdir
	endline
fi

if [ ! -d $bmcdir ]; then
	logn "Extracting $cbmc_zip archive.."
	mkdir -p $bmcdir
	unzip -q $archdir/$cbmc_zip -d $bmcdir
	mv $bmcdir/cbmc-*/* $bmcdir
	rm -rf $bmcdir/cbmc-*
	endline
fi

[ ! -d $bmcdir ] && error "cannot find CBMC root directory"

# install functions
configme () {
	log "Setting ParaFROST configuration file to:"
	unset PFROSTCONFIG
	configfile=parafrost_config.ini
	configsrc=$PWD/interface/satcheck-parafrost/$configfile
	[ ! -f $configsrc ] && error "cannot find $configsrc"
	configdest=
	if [[ "$config" != "" ]]; then
		[ ! -d $config ] && error "cannot find $config"
		cp $configsrc $config
		configdest=$config/$configfile	
	else
		configdest=$configsrc
	fi
	export PFROSTCONFIG=$configdest	
	log "  $PFROSTCONFIG"
}

buildme () {
	if [ $update = 0 ]; then
		cleanup $1
		make -C $bmcdir/src parafrost-download &>> $logfile
	fi
	[ ! -d $bmcdir/parafrost ] && error "cannot find parafrost directory in $bmcdir"
	if [[ "$1" == "cpu" ]]; then 
		export PARAFROSTCPU=../../parafrost
		make -C $bmcdir/src &>> $logfile
	elif [[ "$1" == "gpu" ]]; then 
		export PARAFROSTGPU=../../parafrost
		make -C $bmcdir/src &>> $logfile
	fi
	[ ! -f $bmcdir/src/cbmc/cbmc ] && error "CBMC not installed due to previous errors"
	mkdir -p build
	cbmcbin=build/cbmc_parafrost_${1}
	cp $bmcdir/src/cbmc/cbmc $cbmcbin
}
#=====================================

log "Preparing CBMC for ParaFROST integration.."
interfacedir=interface
patdir=make_patches/src
[ ! -d $interfacedir ] && error "cannot find ParaFROST interface directory"
[ ! -d $patdir ] && error "cannot find necessary patches for CBMC install"

logn " Installing ParaFROST interface.."
cp -r $interfacedir/satcheck-parafrost $bmc_solvers/sat
endline
pattest="patch -N --dry-run --silent"
if $pattest $bmcsrc/config.inc < $patdir/config.patch &>/dev/null; then
	logn " " ; patch $bmcsrc/config.inc < $patdir/config.patch
fi
#
if $pattest $bmcsrc/common < $patdir/common.patch &>/dev/null; then
	logn " "; patch $bmcsrc/common < $patdir/common.patch
fi
#
if $pattest $bmcsrc/Makefile < $patdir/Makefile.patch &>/dev/null; then
	logn " "; patch $bmcsrc/Makefile < $patdir/Makefile.patch
fi
#
if $pattest $bmc_solvers/Makefile < $patdir/solvers/Makefile.patch &>/dev/null; then
	logn " "; patch $bmc_solvers/Makefile < $patdir/solvers/Makefile.patch
fi
#
if $pattest $bmc_sat/satcheck.h < $patdir/solvers/sat/satcheck.patch &>/dev/null; then
	logn " "; patch $bmc_sat/satcheck.h < $patdir/solvers/sat/satcheck.patch
fi
#
if $pattest $bmc_cbmc/Makefile < $patdir/cbmc/Makefile.patch &>/dev/null; then
	logn " "; patch $bmc_cbmc/Makefile < $patdir/cbmc/Makefile.patch
fi
#
if $pattest $bmc_gotoin/Makefile < $patdir/goto-instrument/Makefile.patch &>/dev/null; then
	logn " "; patch $bmc_gotoin/Makefile < $patdir/goto-instrument/Makefile.patch
fi
log ""

if [ $donly = 1 ]; then 
	parafrost_release=2.2.4
	parafrost_tag=parafrost-${parafrost_release}
	parafrost_tar=${parafrost_tag}.tar.gz
	logn "Downloading ParaFROST version $parafrost_release.."
	curl -L --remote-name https://github.com/muhos/ParaFROST/archive/${parafrost_tar}  &> /dev/null
	tar xfz $parafrost_tar
	rm -rf $bmcdir/parafrost
	mv ParaFROST-${parafrost_tag} $bmcdir/parafrost
	rm $parafrost_tar
	endline && log "" && configme && rm -rf $logfile $logdir && ruler
	[ $srced = 1 ] && return 0 || exit 0
fi

if [ $icpu = 1 ]; then
	logn "Building CBMC + ParaFROST-CPU [may take +10 min].."; buildme cpu; endline; log ""
fi 
if [ $igpu = 1 ]; then 
	if [ $update = 1 ]; then
		logn "Updating ParaFROST-GPU compilation.."
		cd $bmcdir/parafrost && ./install.sh -g -q && cd ../../
		endline
		logn "Updating CBMC + ParaFROST-GPU [may take +15 min].."
	else
		logn "Building CBMC + ParaFROST-GPU [may take +15 min].."
	fi
	buildme gpu
	endline && log ""
fi 

ruler && configme && movelogs && pffinal