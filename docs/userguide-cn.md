# 杉数求解器MATLAB接口参考手册

`MATLAB`是一款流行的工程软件，广泛应用于学术研究与工业实践中。`杉数求解器` 是一款高性能大规模数学规划求解器，
目前可以求解线性规划和混合整数规划问题。为了方便广大科研人员与企业工程师在MATLAB工具下建模并使用杉数求解器求解优化问题，
本工具包实现了杉数求解器的MATLAB接口。下面对杉数求解器的MATLAB接口功能进行阐述。

## 功能概述

杉数求解器的MATLAB接口提供了文件读写、建模与求解功能，支持求解线性规划、混合整数线性规划问题。同时，还提供了与MATLAB的优化工具箱中
`linprog` 和 `intlinprog` 函数具有相同功能的函数。下面将依次阐述MATLAB接口的各项规范与功能。

## 输入输出参数说明

杉数求解器的MATLAB接口的输入与输出参数大量使用了MATLAB语言的 `struct` 数据类型。按照功能属性主要包括以下几种参数：

### 版本信息

版本信息是MATLAB的 `struct` 类型的变量，用于存储杉数求解器的版本号，包括以下3个域：

- `major`

  软件大版本号。

- `minor`

  软件小版本号。

- `technical`

  软件修复版本号。

### 模型信息

模型信息是MATLAB的 `struct` 类型的变量，用于存储待求解的问题的数据信息，包括以下域：

- `objsen`

  模型的优化方向。以字符串形式表示，可取值包括：

  * `'min'`

    最小化。

  * `'max'`

    最大化。
  
  该域可为空，则取其默认值：`'min'`。

- `objcon`

  目标函数的常数部分。该域可为空，则取其默认值：0。

- `A`

  模型的系数矩阵。该域以实数稀疏矩阵表示，不可为空。

- `obj`

  模型的目标函数系数。该域可为空，则取其默认值：零向量。若该域非空，则其长度为模型的变量数。

- `lb`

  模型中变量的下界。该域可为空，则取其默认值：0。若该域非空，则其长度为模型的变量数。

- `ub`

  模型中变量的上界。该域可为空，则取其默认值：`COPT_INFINITY`。若该域非空，则其长度为模型的变量数。

- `vtype`

  模型中变量的类型，支持以下变量类型：

  * `'C'`

    连续变量

  * `'B'`

    二进制变量

  * `'I'`

    整数变量
  
  该域可为空，则取其默认值：`'C'`。若该域非空，且值为标量，则所有变量类型为标量值指定的类型。若该域非空，且值为向量，则其长度为模型的变量数。

- `varnames`

  模型中变量的名字。以MATLAB的 `cell` 类型表示，每个元胞内容为字符串。该域可为空，表示不指定变量的名字。若该域非空，则其元素数目为模型的变量数。

- `sense`

  模型中约束的方向，支持以下约束方向：

  * `'L'`

    小于等于

  * `'E'`

    等于

  * `'G'`

    大于等于

  **注意** 若该域非空，则接口函数使用 `sense` 和 `rhs` 域中的内容构建约束。若该域为空，则接口函数使用 `lhs` 和 `rhs` 域中的内容构建约束。

  若该域非空，且值为标量，则所有约束类型为标量值指定的类型。若该域非空，且值为向量，则其长度为模型的约束数。

- `lhs`

  模型中约束的下界。当域 `'sense'` 不为空时，该域可为空，否则该域不可为空，其长度为模型的约束数。

- `rhs`

  模型中约束的上界。该域不可为空，其长度为模型的约束数。

- `constrnames`

  模型中约束的名字。以MATLAB的 `cell` 类型表示，每个元胞内容为字符串。该域可为空，表示不指定约束的名字。若该域非空，则其元素数目为模型的约束数。

SOS约束相关的域：

- `sos`

  模型中的SOS约束。以MATLAB的 `struct` 类型表示，每个结构体包括以下3个域：

  * `type`

    SOS约束的类型。该域不可为空，其取值 `1` 表示SOS-1约束，`2` 表示SOS-2约束。

  * `vars`

    SOS约束的变量的下标列表。该域不可为空。

  * `weights`

    SOS约束的变量的权重列表。该域可为空，此时权重由杉数求解器自动生成。

Indicator约束相关的域：

- `indicator`

  模型中的Indicator约束。以MATLAB的 `struct` 类型表示，每个结构体包括以下5个域：

  * `binvar`

    Indicator变量对应的下标。
  
  * `binval`

    Indicator变量的取值。
  
  * `a`

    线性约束的系数。
  
  * `sense`

    线性约束的方向。
  
  * `rhs`

    线性约束的右端项。

二次规划相关的域：

- `Q`

  二次规划目标函数中的二次项。该域以实数稀疏矩阵表示。

二次约束规划相关的域：

- `quadcon`

  模型中的二次约束。以MATLAB的 `struct` 类型表示，每个结构体包括以下8个域：

  * `Qc`

    二次约束中的二次项。该域以实数稀疏矩阵表示。

  * `Qrow`、`Qcol` 和 `Qval`

    二次约束中的二次项。分别表示二次项非零元的行索引、列索引和非零元素值。

    **注意:** 域 `Qc` 和域 `Qrow` 、 `Qcol`、 `Qval` 不可同时为空，优先选取域 `Qc` 作为二次项。

  * `q`

    二次约束中的线性项。该域以稀疏实数向量表示，可为空。

  * `sense`

    二次约束的类型。不可为空。

  * `rhs`

    二次约束的右端项。不可为空。

  * `name`

    二次约束的名字。可为空。

锥约束相关的域：

- `cone`

  模型中的二阶锥约束。以MATLAB的 `struct` 类型表示，每个结构体包括以下2个域：

  * `type`

    锥约束的类型。可取值为：1 表示标准锥，2 表示旋转锥。不可为空。

  * `vars`

    锥约束中变量的下标。不可为空。

初始解信息相关的域：

- `varbasis`

  模型中的变量的基解信息。对于线性规划模型，当该域非空时，则以域中的值作为初始变量基状态进行优化求解。

- `constrbasis`

  模型中的约束的基解信息。对于线性规划模型，当该域非空时，则以域中的值作为初始约束基状态进行优化求解。

- `x`

  对于线性规划，表示最优变量解值。当域 `x`、`rc`、`slack` 和 `pi` 均为非空时，设置求解参数 `lpmethod` 为3，即可直接调用杉数求解器进行Crossover。

- `rc`

  线性规划中Reduced cost的取值。

- `slack`

  线性规划中松弛变量的取值。

- `pi`

  线性规划中对偶变量的取值。

- `start`

  模型的初始解信息。对于整数规划模型，当该域非空时，则将判断该域中的值是否有效，若是则利用该信息作为整数规划的初始解。

  若该域是致密向量，则应对每个变量指定初始解信息，若某些变量取值不确定，则指定其值为 `nan` ；若该域是稀疏向量，则指定部分变量的初始解即可。

### 参数信息

参数信息是MATLAB的 `struct` 类型的变量，用于存储优化求解的参数设置。该变量中的域名与其含义
详见杉数求解器中参数相关章节的内容。

### 结果信息

结果信息是MATLAB的 `struct` 类型的变量，用于存储优化求解后的结果与状态，包括以下域：

- `status`

  解状态信息，以字符串形式表示，包括以下几种情形：

  * `'unstarted'`

    尚未开始求解。

  * `'optimal'`

    找到了最优解。

  * `'infeasible'`

    模型是无解的。

  * `'unbounded'`

    目标函数在优化方向没有边界。

  * `'inf_or_unb'`

    模型无解或目标函数在优化方向没有边界。

  * `'nodelimit'`

    在节点限制达到前未能完成求解。

  * `'timeout'`

    在时间限制到达前未能完成求解。

  * `'unfinished'`

    求解终止。但是由于数值问题求解器无法给出结果。

  * `'interrupted'`

    用户中止。

- `simplexiter`

  单纯形迭代循环数。

- `barrieriter`

  内点法迭代循环数。

- `nodecnt`

  分支定界搜索的节点数。

- `bestgap`

  整数规划求解结束时最好的相对容差。

- `solvingtime`

  求解所使用的时间（秒）。

- `objval`

  对于线性规划，表示最优目标值。对于整数线性规划，表示求解结束时最好的目标函数值。

- `bestbnd`

  整数规划求解结束时最好的下界。

- `varbasis`

  线性规划中变量的最优基状态信息。

- `constrbasis`

  线性规划中约束的最优基状态信息。

- `x`

  对于线性规划，表示最优变量解值。对于整数线性规划，表示求解结束时最好的变量解值。

- `rc`

  线性规划中Reduced cost的取值。

- `slack`

  线性规划中松弛变量的取值。

- `pi`

  线性规划中对偶变量的取值。

- `qcslack`

  二次约束规划中二次约束的取值。

- `pool`

  对于整数规划模型，表示解池中的解。以MATLAB的 `struct` 类型表示，每个结构体中包括以下2个域：

  - `objval`

    解池中解的目标函数值。

  - `xn`

    解池中解的变量取值。

IIS结果相关信息，包括以下域：

- `isminiis`

  是否为极小IIS。

- `varlb`

  变量上界的IIS状态。

- `varub`

  变量下界的IIS状态。

- `constrlb`

  约束上界的IIS状态。

- `construb`

  约束下界的IIS状态。

- `sos`

  SOS约束的IIS状态。

- `indicator`

  Indicator约束的IIS状态。

### 文件读写

- `copt_read` 函数

  - **概要**

    `problem = copt_read(probfile)`

    `problem = copt_read(probfile, basfile)`

  - **描述**

    读取指定的模型文件并返回模型对象。对于线性规划问题，若提供了基解文件，则将基状态信息也存储在返回的模型对象相应的域中。

  - **参量**

    `probfile`

      模型文件名。目前支持MPS格式、LP格式和COPT二进制格式的模型，根据文件后缀名自动识别。

    `basfile`

      基解文件名。

    `problem`

      模型对象。该变量类型为MATLAB的 `struct` 类型。

  - **示例**

    ```matlab
    mip_problem = copt_read('testmip.mps')
    lp_problem = copt_read('testlp.lp', 'testlp.bas')
    ```

- `copt_write` 函数

  - **概要**

    `copt_write(problem, probfile)`

  - **描述**

    将模型对象表示的模型输出到指定文件中。

  - **参量**

    `problem`

      模型对象。该变量类型为MATLAB的 `struct` 类型。

    `probfile`

      待输出模型文件名。目前支持MPS格式、LP格式和COPT二进制格式的模型文件，根据文件后缀名进行自动识别。

  - **示例**

    ```matlab
    problem = copt_read('testmip.mps')
    copt_write(problem, 'testmip.lp')
    ```

### 建模与求解

- `copt_solve` 函数

  - **概要**

    `version = copt_solve()`

    `solution = copt_solve(probfile)`

    `solution = copt_solve(probfile, parameter)`

    `solution = copt_solve(problem)`

    `solution = copt_solve(problem, parameter)`

  - **描述**

    该函数有多种用法。若输入参数为空，则返回版本信息对象。若输入函数为模型文件及参数信息对象，
    则直接读取模型文件及参数信息对象中的设置并求解指定的模型，求解完成后返回结果信息对象。
    若输入函数为模型信息对象和参数信息对象，则提取模型信息对象和参数信息对象中的相关信息，
    在内部构建模型并求解，求解完成后返回结果信息对象。

  - **参量**

    `version`

      版本信息对象。该变量类型为MATLAB的 `struct` 类型。

    `solution`

      结果信息对象。该变量类型为MATLAB的 `struct` 类型。

    `probfile`

      模型文件名。目前支持MPS格式、LP格式和COPT二进制格式的模型，根据文件后缀名自动识别。

    `parameter`

      参数信息对象。该变量类型为MATLAB的 `struct` 类型。

    `problem`

      模型信息对象。该变量类型为MATLAB的 `struct` 类型。

  - **示例**

    ```matlab
    version = copt_solve();

    mip_solution = copt_solve('testmip.mps');

    lpparam.TimeLimit = 10;
    lp_solution = copt_solve('testlp.lp', lpparam);
    ```

- `copt_computeiis` 函数

  - **概要**

    `iisinfo = copt_computeiis(probfile)`

    `iisinfo = copt_computeiis(probfile, parameter)`

    `iisinfo = copt_computeiis(problem)`

    `iisinfo = copt_computeiis(problem, parameter)`

  - **描述**

    若输入函数为模型文件及参数信息对象，则直接读取模型文件及参数信息对象中的设置并计算指定模型的IIS，
    计算完成后返回IIS结果信息对象。
    若输入函数为模型信息对象和参数信息对象，则提取模型信息对象和参数信息对象中的相关信息，
    在内部构建模型并计算IIS，计算完成后返回IIS结果信息对象。

  - **参量**

    `iisinfo`

      IIS结果信息对象。该变量类型为MATLAB的 `struct` 类型。

    `probfile`

      模型文件名。目前支持MPS格式、LP格式和COPT二进制格式的模型，根据文件后缀名自动识别。

    `parameter`

      参数信息对象。该变量类型为MATLAB的 `struct` 类型。

    `problem`

      模型信息对象。该变量类型为MATLAB的 `struct` 类型。

  - **示例**

    ```matlab
    iisinfo = copt_computeiis('testmip.mps');

    lpparam.TimeLimit = 10;
    iisinfo = copt_computeiis('testlp.lp', lpparam);
    ```

### 其它函数

杉数求解器的MATLAB接口还提供了与MATLAB的优化工具箱函数 `linprog` 和 `intlinprog` 具有
相同功能的函数封装，分别名为 `copt_linprog` 和 `copt_intlinprog` 函数，
使用方法与MATLAB优化工具箱提供的函数用法相同。
