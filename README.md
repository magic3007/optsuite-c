# OptSuite
---

## 安装

框架使用 CMake 构建，仅在 Linux 系统下测试过。Windows 用户可选择 WSL 进行测试。

必要依赖：
- gcc/g++/gfortran
- Eigen3
- fftw
- PROPACK
- BLAS/LAPACK 实现（或 Intel MKL，当使用 MKL 时，Eigen 会将后端替换为 MKL 中的函数）

可选依赖：
- Matlab（用于读取 .mat 文件）

### 依赖的安装过程

gcc/g++/gfortran, Eigen3, fftw, blas/lapack 和 MKL 均可从源里拉取或从官网上安装（MKL）。
PROPACK 可以到[这个网站](http://sun.stanford.edu/~rmunk/PROPACK/)下载源码，
选择最新版 2.1。

常见问题：
- 找不到 `g77` 命令。PROPACK 比较老了，`g77` 是比较老旧的 FORTRAN 编译器，
  可以使用 `gfortran` 替代。具体方法是更改 `make.inc`
- 编译 `second.F` 出错。可参考[这个网站](http://modb.oce.ulg.ac.be/mediawiki/index.php/How_to_compile_ARPACK)做类似修改。

我已经在 `contrib/make.inc` 中写了在现代 Linux 下的常用配置。可以参考使用。
需要注意的是 `BLAS` 和 `LIBPATH` 需要根据使用 BLAS/LAPACK 的位置进行更改，
这里的例子是使用了 MKL sdl 库。

### Configure

使用 `cmake` 命令构建，在 `optsuite-c` 根目录下新建一个名为 `build` 的目录，
并切换到该目录中。
```
mkdir build
cd build
```
构建的命令为
```
cmake [OPTIONS] ..
``` 
比较重要的参数有：
- `-DCMAKE_BUILD_TYPE=Debug|Release` 是否启用优化，默认是 Debug（不启用）
- `-DPROPACK_ROOT_DIR=dir` PROPACK 安装根目录
- `-DPROPACK_LIB_SUFFIX=suffix` PROPACK 在编译时会生成平台相关的后缀，
  例如 Linux 下的后缀可能是 `LINUX_GCC_X86_64`
- `-DBUILD_MATLAB_INTERFACE=ON|OFF` 是否构建 MATLAB 相关接口，默认不构建
- `-DMatlab_ROOT_DIR` 在构建 MATLAB 相关接口条件下，指定 MATLAB 安装根目录

下面是一些例子：
```
# 指定 propack，不使用 matlab，不启用优化
cmake -DPROPACK_ROOT_DIR=/home/user/propack -DPROPACK_LIB_SUFFIX=LINUX_GCC_X86_64 ..

# 指定 propack，使用 matlab，使用优化
cmake -DPROPACK_ROOT_DIR=/home/user/propack -DPROPACK_LIB_SUFFIX=LINUX_GCC_X86_64
      -DCMAKE_BUILD_TYPE=Release -DBUILD_MATLAB_INTERFACE=ON -DMatlab_ROOT_DIR=/opt/MATLAB/R2020b ..
```

目前 cmake 会自动查找环境中的 BLAS/LAPACK 或者是 MKL，使用 MKL 前请设置好相关
环境变量，如 `MKLROOT`

### 编译
Configure 成功后，使用 `make -jN` 编译，其中 `N` 是编译的线程数。

编译结束后，会在 `build` 目录看到如下文件：
- `libOptSuite.so` 主要代码框架库
- `libOptSuite_matlab.so` 若选择构建 matlab，则会出现该库
- `opt` 可执行文件，演示选项类
- `block` 可执行文件，演示变量类
- `fft` 可执行文件，演示 fft
- `svd` 可执行文件，在选择构建 matlab 库时出现，演示 svd
- `readmat` 可执行文件，在选择构建 matlab  库时出现，演示读取 .mat 文件

也可以到 `example` 目录下查看、修改这些代码，并重新编译。
