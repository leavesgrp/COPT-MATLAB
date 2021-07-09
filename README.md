# 杉数求解器MATLAB接口安装说明

## 安装

本工具包提供了杉数求解器的MATLAB接口的预编译文件与C源代码文件。对于一般用户使用，打开MATLAB软件，切换用户到工具包所在的目录，执行
`coptmex_install` 命令即可。若正常安装，则输出下述信息：

```none
Installing MATLAB interface for Cardinal Optimizer on Windows system...
Add and save path D:\Tools\copt-mex\lib to MATLAB search path.
MATLAB interface for COPT v2.0.2 was installed.
```

然后切换目录至 `examples` 子目录，执行 `demo_lp`。若已正确配置杉数求解器和本工具包，则输出下述信息：

```none
Cardinal Optimizer v2.0.2. Build date Jun 18 2021
Copyright Cardinal Operations 2021. All Rights Reserved
Setting parameter 'TimeLimit' to 10
Maximizing an LP problem

The original problem has:
    2 rows, 3 columns and 6 non-zero elements
The presolved problem has:
    2 rows, 3 columns and 6 non-zero elements

Starting the simplex solver using up to 8 threads

Method   Iteration           Objective  Primal.NInf   Dual.NInf        Time
Dual             0    6.2083756300e+00            1           0       0.10s
Dual             1    3.5783473899e+00            0           0       0.10s

Solving finished
Status: Optimal  Objective: 3.5783333333e+00  Iterations: 1  Time: 0.12s

Objective value: 3.578333
Variable solution:
  x = 0.100000
  y = 1.500000
  z = 0.361111
Variable basis status:
  x = 0
  y = 2
  z = 1
Constraint basis status:
  r0 = 2
  r1 = 1
```

若未输出上述信息，请检查是否已配置好杉数求解器的许可。

对于高级用户，还可以自行修改杉数求解器的MATLAB接口源代码文件，并编译生成MATLAB接口动态库，分别介绍如下：

**注意**：请确保已配置好 `COPT_HOME` 和 `MATLAB_HOME` 这两个环境变量，分别指向杉数求解器和MATLAB软件的安装路径。

- Windows

  对于Windows系统，要求用户计算机中已安装了Microsoft Visual Studio和MATLAB软件，且已配置好杉数求解器和MATLAB必要的环境变量。
  以Visual Studio 2017版本为例，打开Windows命令行工具，执行命令：

  ```cmd
  "%VS2017INSTALLDIR%\VC\Auxiliary\Build\vcvarsall.bat" amd64
  ```

  配置Visual Studio编译器环境，然后切换目录至 `src` 子目录，执行命令:

  ```cmd
  nmake -f Makefile.win
  ```

  编译生成杉数求解器的MATLAB接口动态库，并复制到上层子目录 `lib` 中。最后，执行命令：

  ```cmd
  nmake -f Makefile.win clean
  ```

  清理编译时产生的临时文件。

- Linux

  对于Linux，分别要求用户计算机中安装了GCC开发工具链和MATLAB软件，且已配置好杉数求解器和MATLAB必要的环境变量。
  打开终端，执行命令：

  ```cmd
  make -f Makefile.unix
  ```

  编译生成杉数求解器的MATLAB接口动态库，并复制到上层子目录 `lib` 中。最后，执行命令：

  ```cmd
  make -f Makefile.unix clean
  ```

  清理编译时产生的临时文件。

- MacOS

  对于MacOS，分别要求用户计算机中安装了Clang开发工具链和MATLAB软件，且已配置好杉数求解器和MATLAB必要的环境变量。
  打开终端，执行命令：

  ```cmd
  make -f Makefile.osx
  ```

  编译生成杉数求解器的MATLAB接口动态库，并复制到上层子目录 `lib` 中。最后，执行命令：

  ```cmd
  make -f Makefile.osx clean
  ```

  清理编译时产生的临时文件。

## 目录说明

本工具包由4个子目录组成，分别介绍如下：

- docs目录

  该目录下存放了杉数求解器MATLAB接口的中英文文档，详细介绍了接口功能。

- examples目录

  该目录下存放了杉数求解器MATLAB接口的调用示例。

- lib目录

  该目录下存放了杉数求解器MATLAB接口的动态库与相关的M文件。

- src目录

  该目录下存放了杉数求解器MATLAB接口的C源代码和头文件，以及适用于Windows/Linux/MacOS的Makefile文件。


# Installation of the COPT MATLAB interface (COPT-MATLAB Toolbox)

## Installation

The `COPT-MATLAB Toolbox` provides the pre-compiled binaries and C source codes for the COPT-MATLAB interface. Users can change to the directory of our toolbox within the MATLAB command window and execute

`coptmex_install` command to install the `COPT-MATLAB Toolbox`. On successful installation, you will see the following output message in the command window

```none
Installing MATLAB interface for Cardinal Optimizer on Windows system...
Add and save path D:\Tools\copt-mex\lib to MATLAB search path.
MATLAB interface for COPT v2.0.2 was installed.
```

Then you can change to the `examples` sub-directory and execute `demo_lp`. With correct setup and configuration of the Cardinal Optimizer (COPT) and our toolbox, it would output

```none
Cardinal Optimizer v2.0.2. Build date Jun 18 2021
Copyright Cardinal Operations 2021. All Rights Reserved
Setting parameter 'TimeLimit' to 10
Maximizing an LP problem

The original problem has:
    2 rows, 3 columns and 6 non-zero elements
The presolved problem has:
    2 rows, 3 columns and 6 non-zero elements

Starting the simplex solver using up to 8 threads

Method   Iteration           Objective  Primal.NInf   Dual.NInf        Time
Dual             0    6.2083756300e+00            1           0       0.10s
Dual             1    3.5783473899e+00            0           0       0.10s

Solving finished
Status: Optimal  Objective: 3.5783333333e+00  Iterations: 1  Time: 0.12s

Objective value: 3.578333
Variable solution:
  x = 0.100000
  y = 1.500000
  z = 0.361111
Variable basis status:
  x = 0
  y = 2
  z = 1
Constraint basis status:
  r0 = 2
  r1 = 1
```
in the command window. Otherwise, please check the COPT license file and ensure that a valid license file has been properly set up.

For the advanced users, a customized MATLAB interface is available by modifying the source code of MATLAB interface and re-compiling the code to generate the MEX files. We provide the instructions for different operating systems below.

**Notice**: Users are required to make sure that both `COPT_HOME` and `MATLAB_HOME` environmental variables are set correctly, which point to the installation direcotory of the Cardinal Optimizer and MATLAB software, respectively.

- Windows

  For the Windows users, Microsoft Visual Studio and MATLAB are needed to customize MATLAB interface. Please also ensure that COPT has been properly installed and that all the necessary environment variables have been set up. 

  After modifying the interface source code, for example, on Visual Studio 2017, you can launch Windows command line tool and execute

  ```cmd
  "%VS2017INSTALLDIR%\VC\Auxiliary\Build\vcvarsall.bat" amd64
  ```

  to configure the Visual Studio compiler environment. Then you can change directory to `src` sub-directory and execute

  ```cmd
  nmake -f Makefile.win
  ```

  to compile the MEX file. Once the MEX file is successfully compiled, it will be automatically copied to the parent directory `lib`. Last, you can execute

  ```cmd
  nmake -f Makefile.win clean
  ```

  to clean up the temporary files.

- Linux
  
  For the Linux users, GCC tool chain and MATLAB are needed to customize MATLAB interface. Please also ensure that COPT has been properly installed and that all the necessary environment variables are set up. 
  
  After modifying the interface source code, you can launch the terminal and execute

  ```cmd
  make -f Makefile.unix
  ```

  to compile the MEX file. Once the MEX file is successfully compiled, it will be automatically copied to the parent directory `lib`. Last, you can execute

  ```cmd
  make -f Makefile.unix clean
  ```

  to clean up the temporary files.

- MacOS

  For the MacOS users, Clang tool chain and MATLAB are needed to customize MATLAB interface. Please also ensure that COPT has been properly installed and that all the necessary environment variables are set up. 

  After modifying the interface source code, you can launch the terminal and execute

  ```cmd
  make -f Makefile.osx
  ```

  to compile the MEX file. Once the MEX file is successfully compiled, it will be automatically copied the parent directory `lib`. Last, you can execute

  ```cmd
  make -f Makefile.osx clean
  ```

  to clean up the temporary files.

## Directories and Contents

COPT toolbox contains 4 sub-directories:

- docs
  
  the directory contains the detailed documentation on the toolbox (both in English and Chinese).

- examples

  the directory contains the examples for the toolbox.

- lib
  
  the directory contains the MEX file and relevant .m files for the toolbox

- src

  the directory contains the C header and source code of the toolbox as well as the Makefile for Windows/Linux/MacOS platform.

