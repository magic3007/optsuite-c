### CPU specific flags for gcc/g77.
#CPUOPT =  -march=pentium4 -mfpmath=sse 
#CPUOPT =  -march=pentium3 -mfpmath=sse
#CPUOPT =  -march=pentium2
#CPUOPT =  -march=athlon-xp -mfpmath=sse
# Older versions of gcc may not support these flags.
# Check your local man pages or use:
#CPUOPT = 
CPUOPT = -march=native


# Optimization flags
OPT = -O3 -funroll-all-loops -ffast-math $(CPUOPT)
OPT_ACCURATE = -O3 -funroll-all-loops $(CPUOPT)
#OPT = -O0 -fbounds-check
#OPT_ACCURATE = -O0 -fbounds-check
NO_OPT = -O0

# Compiler flags
FFLAGS = -g -fno-second-underscore  -W -Wall $(OPT) -fPIC
FFLAGS_NOOPT = -g  -fno-second-underscore -W -Wall $(NOOPT) -fPIC
FFLAGS_ACCURATE = -g  -fno-second-underscore -W -Wall $(OPT_ACCURATE) -fPIC
CFLAGS = -g  -W -Wall  $(OPT) -fPIC
LINKFLAGS = -g  -W -Wall  $(OPT) 

# Which compiler and linker to use
CC = gcc
F77 = gfortran -I.
LINKER = gfortran
MAKELIB = ar rc

# Linker paths and flags
#LIBPATH =  -L$/usr/local/lib
#BLAS = -lmkl_p4 -lguide
#LIBPATH =  -L$(HOME)/lib/$(ARCH)
#BLAS = -llapack -lf77blas -latlas
#BLAS = -lgoto_p4-64_1024-r0.97
#BLAS = -lgoto_p4_512-r0.9
#BLAS = -lgoto_p3_512-r0.9
LIBPATH = -L${MKLROOT}/lib/intel64
BLAS = -Wl,--no-as-needed -lmkl_rt -lpthread -lm -ldl
RANLIB = ranlib
#MGS = mgs.pentium
MGS = mgs.risc


#  Platform postfix to use in naming executables and library files
PLAT = LINUX_GCC_X86_64

