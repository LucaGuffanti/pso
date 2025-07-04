## modules.sh
# This script loads the necessary modules for different HPC environments.
# call it with source modules.sh <hpc_name> or . modules.sh <hpc_name>

if [ "$1" = "iris" ]; then
    echo "Loading modules for Iris HPC"
    module load devel/CMake
    module load mpi/OpenMPI
    module load compiler/NVHPC/24.11-CUDA-12.6.0
    return 0
fi

if [ "$1" = "vsc" ]; then
    echo "Loading modules for VSC HPC"
    module load cuda
    module load mpi
    module load cmake
    return 0
fi

echo "Unknown HPC: $1"