[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)

# gpu4bmc

This repository contains all necessary patches, interface and a script to install the C Bounded Model Checker [CBMC](https://github.com/diffblue/cbmc) 5.31.0 with our SAT solver [ParaFROST](https://github.com/muhos/ParaFROST).

ParaFROST is a parallel SAT solver with GPU-accelerated inprocessing capable of harnessing NIVIDA CUDA-enabled GPUs in applying modern simplification tecnhiques in parallel. New compact data structure and memory-aware Variable-Clause Eliminations (VCE) are designed specifically for BMC problems in the new version of our tool. Furthermore, the decision making heuristics are further improved. Check our paper in [CAV'21](https://gears.win.tue.nl/papers/cbmc_gpu.pdf) for more information.

# Configuration
CBMC is patched to read a configuration file before starting the search. This file contains all options supported by ParaFROST in the format `<option>=<value>`. The configurable interface allows the user to explore all ParaFROST capabilities while running the verifier. 
The file is located by default in `interface/satcheck-parafrost/parafrost_config.ini` and its path is set via an enviromental variable called `PFROSTCONFIG`.<br>

# Install

To install either CBMC + ParaFROST (CPU) or CBMC + ParaFROST (GPU), use the `install.sh` script which has the following usage:

&nbsp; usage: `install.sh [ <option> ... ]`<br>
&nbsp; where `<option>` is one of the following

       -h or --help            print this usage summary
       -c or --cpu             install CBMC + ParaFROST-CPU solver
       -g or --gpu             install CBMC + ParaFROST-GPU solver
       -n or --less            print less verbose messages
       -q or --quiet           be quiet
       -d or --download        download only then patch CBMC
       -r or --remove          remove source code and all created folders
       --config=<target>       set the configuration file location
                               to <target> (should be a full path)
                               starting from root
       --clean=<target>        remove completely old installation of
                               CBMC <cpu | gpu | all | solvers> builds
                               all targets cannot be combined with other
                               options except for <solvers>


## GPU
To build the GPU accelerated CBMC, make sure you have a CUDA-capable GPU with pre-installed NVIDIA driver and CUDA toolkit.

For installing the driver + CUDA, run the following commands:<br>

`wget https://developer.download.nvidia.com/compute/cuda/repos/ubuntu1804/x86_64/cuda-ubuntu1804.pin`<br>
`sudo mv cuda-ubuntu1804.pin /etc/apt/preferences.d/cuda-repository-pin-600`<br>
`sudo apt-key adv --fetch-keys https://developer.download.nvidia.com/compute/cuda/repos/ubuntu1804/x86_64/7fa2af80.pub`<br>
`sudo add-apt-repository "deb https://developer.download.nvidia.com/compute/cuda/repos/ubuntu1804/x86_64/ /"`<br>
`sudo apt-get update`<br>
`sudo apt-get -y install cuda`<br>

Now the GPU checker is ready to install by running the install script via the command `. install.sh -g`. 
Mind the space after the dot to force the script to run under the current shell, thus keeping the value of the enviromental variable `PFROSTCONFIG` intact.<br>

The `CBMC_paraforst_gpu` binary will be created by default in the build directory.<br>

To build a CPU-only version of CBMC + ParaFROST, run `. install.sh -c`.<br>

# Test
The GPU-accelerated version of CBMC with ParaFROST has a complete artifact we created to experimentally evaluate the effectiveness of accelerating Boumded Model Checking (BMC) using GPUs on verifying AWS C99 package. The artifact can be downloaded [here](https://gears.win.tue.nl/software/gpu4bmc/).<br>
