# MPI exersices

MPI programs for Message Passing Programming course at uni to practice parallel programming.

## Template

Includes a template in `MPI_template` folder with a header file of useful macros and functions aswell as a directory structure and a Makefile to compile the code. This template is adapted from the one provided by EPCC.

## Usage

First make sure you change `archer2mpi.job` to use your own account and configurations for the slurm job.

Then compile the binaries:
```bash
cd <exersice_you_want_to_run> && make
```

Then launch the script with slurm:
```bash
sbatch archer2mpi.job 
```
