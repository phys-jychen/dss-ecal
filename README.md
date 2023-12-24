# DSS-ECAL, a Stand-Alone Simulation Framework for DarkSHINE ECAL

## Author
Ji-Yuan CHEN (SJTU; <jy_chen@sjtu.edu.cn>)

## Description
This program was designed for generating Monte Carlo samples of DarkSHINE ECAL. With the help of the MC samples, further studies, including event reconstruction and structure optimisation, can be carried out.

### Structure of ECAL
- **Crystal:** LYSO; cross-section 2.5 × 2.5 cm<sup>2</sup> in general, while the length can be set in the YAML file.
- **Wrapper:** ESR, 0.3 mm thick.
- **SiPM:** Silicon, 0.6-mm thick, while the length and width can be set in the configuration file. Locates between the wrapped crystal and PCB.
- **PCB:** FR4, 2.5-mm thick. Considering the merits of $x$–$y$ crossing structure, the PCBs are placed on the sides of ECAL.
- **Gap:** Carbon structure; both between the neighbouring crystals in the same layer and two consecutive layers.

## Environment Set-up
This project requires CMake version >= 3.11. If you are working on the cluster of INPAC, IHEP, etc., the environment can be easily set up by executing
```shell
source /cvmfs/sft.cern.ch/lcg/views/LCG_97rc4python3/x86_64-centos7-gcc9-opt/setup.sh
```
which has been included in `setup.sh`.

Then, the environment with CMake 3.14.3, Geant4-10.06-patch-01, and ROOT 6.20/00 is configured. As long as no compilation errors are raised and the CMake version requirement is met, other versions on the LCG are also acceptable (mind your version of Geant4!). :stuck_out_tongue:

## Installation & Compilation
Having set up the environment, this project can be cloned from GitHub and compiled normally:
```shell
git clone git@github.com:phys-jychen/dss-ecal.git
cd dss-ecal
mkdir build
cd build
cmake ..
make -j100    # Just do it!
source setup.sh
```

## Usage
Execute
```shell
ecal -p
```
to create default configuration YAML file, `default.yaml`, in your current directory.

Then, you can modify the configuration file and execute
```shell
ecal -c default.yaml
```
to generate MC samples.

While necessary, you can also print help message by executing
```shell
ecal -h
```

Every time you log in to the cluster, before the first time of running this program, remember to execute
```shell
source [build]/setup.sh
```

By now, the compilation has been finished. Generate your YAML file, and have fun! :relaxed:

## Change Log
### 19 December 2023
- Creation of the program. The electronics are placed behind the crystals in $z$ direction, instead of on the ends.
- More options for the locations of the electronics will be available in the following versions. The arrangements of the crystals will also be updated.

### 20 December 2023
Added an interface for deciding the length and width of PCB and SiPM.

### 24 December 2023
- Moved the PCBs to the sides of ECAL, instead of after each layer.
- Added an interface for deciding the read-out mode — single or dual read-out.
- Added optical adhesives between the ends of the crystals. In this version, the material is epoxy (the same as that used in FR4) which should be modified in future versions. No ideal material (with specific chemical formula) has been found for this version...
- Added exception handling section, which checks whether the length and width of ECAL match, before construction.