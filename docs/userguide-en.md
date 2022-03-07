# COPT MATLAB Interface Reference Manual

`MATLAB` is a popular engineering software with wide application in both academic research and industrial areas. COPT (Cardinal Optimizer) is a high-performance mathematical programming solver for large-scale optimization problems and currently solves linear programming as well as mixed integer programming problems. To simplify the modeling and solving procedure in MATLAB, this toolbox serves as an implementation of the MATLAB interface of COPT (`COPT-MATLAB Toolbox`).

## Overview

The `COPT-MATLAB Toolbox` provides various functionalities including file I/O, modeling and solving for Linear Programming (LP) and Mixed Integer Programming (MILP) problems. Specially, `COPT-MATLAB Toolbox` provides consistent implementations of `linprog` and `intlinprog` functions to overwrite the same utilities provided by the `MATLAB Optimization Toolbox`.

A detailed introduction to the `COPT-MATLAB Toolbox` is given as follows.

## Input/Output Parameters

`COPT-MATLAB Toolbox` uses the MATLAB `struct` data structure as the input/output parameters. 
These parameters are grouped and explained below by their functionalities.

### Version Information

Version info is of type MATLAB `struct` and stores the version number of COPT solver.
The struct contains 3 fields:

- `major`

  Major version number

- `minor`

  Minor version number

- `technical`

  Technical version number

### Model Information

Model info is of type MATLAB `struct` and stores the data of the model and solution. Model info contains the following fields:

- `objsen`

  Objective sense. This field is of `string` type and takes two candidate values:

  * `'min'`

    Minimization

  * `'max'`

    Maximization
  
  If this field is empty, then the solver will use the default value `'min'`.

- `objcon`

  Constant value in the objective function. If this field is empty, then the solver applies the default value 0.

- `A`

  Constraint coefficient matrix. This field must be a `real sparse matrix` and cannot be empty.

- `obj`

  Objective coefficient vector. If this field is empty, then the solver applies the default value: `zero vector`. If available, this field must be a dense vector of the same length as the optimization variables.

- `lb`

  Variable lower bounds. If this field is empty, then the solver applies the default value 0 for all the variables. If available, this field must be a dense vector of the same length as the optimization variables.

- `ub`

  Variable upper bounds. If this field is empty, then the solver applies the default value `COPT_INFINITY` for all the variables. If available, this field must be a dense vector of the same length as the optimization variables.

- `vtype`

  Variable types. This field is represented by `string` and takes the following candidate values

  * `'C'`

    Continuous variable

  * `'B'`

    Binary variable

  * `'I'`

    Integer variable
  
  If this field is empty, then the solver applies the default value `'C'` for all the variables. If available, the field must be a string vector of the same length as the optimization variables.

- `varnames`

  Variable names. This field is represented by MATLAB `cell` and each cell component contains a string. If this field is empty, then there is no variable name specification. If available, the number of cell components must be the same as that of the optimization variables.

- `sense`

  Constraint senses. This field is represented by `string` and takes the following candidate values

  * `'L'`

    Less than or equal to (<=)

  * `'E'`

    Equal to (=)

  * `'G'`

    Greater than or equal to (>=)

  **Note** If this field is available, the toolbox will build constraints by `sense` and `rhs`. If not, `lhs` and `rhs` will be used instead.

  If this field takes a single scalar, then all the constraints will be of the type specified by the scalar. If the field takes a vector, then its length must be the same as the number of constraints.

- `lhs`

  Constraints lower bounds. If available, it must be a dense vector whose length is the same as the number of constraints. At least one of `'sense'` and `lhs` should be available for a valid model.

- `rhs`

  Constraints upper bounds. This field must be a non-empty dense vector whose length is the same as the number of constraints.

- `constrnames`

  Constraint names. The field is represented by MATLAB `cell` and each cell component contains a string. If this field is empty, then there is no constraint name specification. If available, the number of cell components must be the same as that of the constraints.

Fields on SOS constraints

- `sos`

  SOS constraints in the model represented by MATLAB `struct`. Each struct contains the following 3 fields:

  * `type`

    SOS constraint type. This field must be non-empty. Value `1` specifies SOS-1 constraint and value `2` specified SOS-2 constraint.

  * `vars`

    Indices of the variables in the SOS constraints. This field must be non-empty.

  * `weights`

    Weight list of the variables in SOS constraints. If this field is empty, then an auto-generated list will be used.

Fields on Indicator constraints

- `indicator`

  Indicator constraints in the model represented by MATLAB `struct`. Each struct contains the following 5 fields:

  * `binvar`

    Indices of the indicator variables.
  
  * `binval`

    Values of the indicator variables.
  
  * `a`

    Coefficients of the linear constraints.
  
  * `sense`

    Sense of the linear constraints.
  
  * `rhs`

    Right-hand-side vector of the linear constraints.

Fields on Quadratic Programming 

- `Q`

  Quadratic terms in quadratic programming. This field is a `real sparse matrix`.

Fields on Quadratic Constrained Programming

- `quadcon`

  Quadratic constraints. This field is a MATLAB `struct`, and each consists of 8 fields shown below:

  * `Qc`

    Quadratic terms in quadratic constraint. This field is a `real sparse matrix`.

  * `Qrow`, `Qcol` and `Qval`

    Quadratic terms in quadratic constraint, which represents row indices, column indices
    and nonzero elements respectively.

    **NOTE:** Field `Qc` and `Qrow`, `Qcol`, `Qval` cannot be empty at the same time,
    field `Qc` will be used if they are both non-empty.

  * `q`

    Linear terms in quadratic constraint. This field is a sparse vector. Can be empty.

  * `sense`

    Type of quadratic constraint. Must not be empty.

  * `rhs`

    Right hand side of quadratic constraint. Must not be empty.

  * `name`

    Name of quadratic constraint. Can be empty.

Fileds on Conic Programming

- `cone`

  Conic constraints. This field is a MATLAB `struct`, and each consists of 2 fields shown below:

  * `type`

    Type of conic constraint. Options values are: 1 means standard cone, 2 means rotated cone.
    Must not be empty.

  * `vars`

    Index of variables in conic constraint. Must not be empty.

Fields on Initial solution

- `varbasis`

  Column (variable) basis status for LP. The solver uses the field as the initial column basis status when it is available.

- `constrbasis`

  Row (constraint) basis status for LP. The solver uses the field as the initial row basis status when it is available.

- `x`

  Optimal solution for LP. When `x`, `rc`, `slack` and `pi` are non-empty, setting parameter `lpmethod` to 3 initiates Crossover in COPT.

- `rc`

  Reduced costs for LP.

- `slack`

  Slack variables for LP.

- `pi`

  Dual variables for LP.

- `start`

  Initial solution for the model. For MILP, the solver will check whether the field is valid when it is available and a valid solution will be used by the MILP solver. 

  If the field is a dense vector, then each component specifies the value of a variable. Variables with uncertain values can be specified by `nan`.

  If the field is a sparse vector, then only the corresponding part of the variables are specified.

### Parameter Information

Parameter info is of type MATLAB `struct` and stores the parameters for optimization. The fields in the struct can be referred from the COPT reference manual.

### Result Information

Result Info is of type MATLAB `struct` and stores the result and status of solution after optimization. Result Info struct contains the following fields:

- `status`

  Solution status information represented as `string`. There are following cases:

  * `'unstarted'`

    Solution not yet started.

  * `'optimal'`

    Optimal solution found.

  * `'infeasible'`

    Model is infeasible.

  * `'unbounded'`

    Model is unbounded.

  * `'inf_or_unb'`

    Model is either infeasible or unbounded.

  * `'nodelimit'`

    Fail to solve the problem within given number of nodes.

  * `'timeout'`

    Fail to solve the problem within given time.

  * `'unfinished'`

    Solution finishes but no result is given due to unexpected behaviors.

  * `'interrupted'`

    Solution is interrupted by the user.

- `simplexiter`

  Number of the simplex iterations.

- `barrieriter`

  Number of the barrier iterations.

- `nodecnt`

  Number of nodes searched by branch and bound.

- `bestgap`

  Best relative MIP gap for MILP.

- `solvingtime`

  Elapsed time for solution (in seconds).

- `objval`

  Optimal objective value for LP or the current best primal bound (objective value) for MILP.

- `bestbnd`

  Best dual bound for MILP.

- `varbasis`

  Optimal column basis status for LP.

- `constrbasis`

  Optimal row basis status for LP.

- `x`
  
  Optimal solution for LP or the current best solution for MILP.

- `rc`

  Reduced costs for LP.

- `slack`

  Slack variables for LP.

- `pi`

  Dual variables for LP.

- `qcslack`

  Activities for quadratic constraints.

- `pool`

  Solutions from solution pool of MIP problem. This field is represented by a MATLAB ``struct`` variable,
  and consists of two sub-fields:

  - `objval`

    Objective values for solution pool.

  - `xn`

    Values of columns for solution pool.

IIS result information, includes:

- `isminiis`

  Whether the computed IIS is minimal.

- `varlb`

  IIS status for lower bounds of variables.

- `varub`

  IIS status for upper bounds of variables.

- `constrlb`

  IIS status for lower bounds of constraints.

- `construb`

  IIS status for upper bounds of constraints.

- `sos`

  IIS status for SOS constraints.

- `indicator`

  IIS status for indicator constraints.

### File I/O

- `copt_read` function

  - **Synopsis**

    `problem = copt_read(probfile)`

    `problem = copt_read(probfile, basfile)`

  - **Description**

    Import (read) model from the specified file and return a model info struct. Basis status will be stored in the returned struct if a basic solution file is provided.

  - **Arguments**

    `probfile`

      File name of the model to import. Currently support MPS, LP and COPT binary format. Automatically identified by the solver.

    `basfile`

      File name of the basic solution file.

    `problem`

      Model info struct. Type of MATLAB `struct`.

  - **Examples**

    ```matlab
    mip_problem = copt_read('testmip.mps')
    lp_problem = copt_read('testlp.lp', 'testlp.bas')
    ```

- `copt_write` function

  - **Synopsis**

    `copt_write(problem, probfile)`

  - **Description**

    Export (Write) a model to the specified file.

  - **Arguments**

    `problem`

      Model info struct to export. Type of MATLAB `struct`.

    `probfile`

      File name of the exported model. Currently support MPS, LP and COPT binary format. Automatically identified by the solver.

  - **Examples**

    ```matlab
    problem = copt_read('testmip.mps')
    copt_write(problem, 'testmip.lp')
    ```

### Modeling and Solving

- `copt_solve` function

  - **Synopsis**

    `version = copt_solve()`

    `solution = copt_solve(probfile)`

    `solution = copt_solve(probfile, parameter)`

    `solution = copt_solve(problem)`

    `solution = copt_solve(problem, parameter)`

  - **Description**

    The function has multiple uses given different inputs. 

    If the input is empty, the function returns a version info struct.
    If the input is a model filename with a parameter info struct, the function reads the model and parameters from the input, solves the problem and returns a result info struct.
    If the input is a model info struct with a parameter info struct, the function extracts the relevant information from the input, constructs the model, solves the problem and returns a result info struct.

  - **Arguments**

    `version`

      Version info struct. Type of MATLAB `struct`.

    `solution`

      Result info struct. Type of MATLAB `struct`.

    `probfile`

      File name of the model to import. Currently support MPS, LP and COPT binary format. Automatically identified by the solver.

    `parameter`

      Parameter info struct. Type of MATLAB `struct`.

    `problem`

      Model info struct. Type of MATLAB `struct`.

  - **Example**

    ```matlab
    version = copt_solve();

    mip_solution = copt_solve('testmip.mps');

    lpparam.TimeLimit = 10;
    lp_solution = copt_solve('testlp.lp', lpparam);
    ```

- `copt_computeiis` function

  - **Synopsis**

    `iisinfo = copt_computeiis(probfile)`

    `iisinfo = copt_computeiis(probfile, parameter)`

    `iisinfo = copt_computeiis(problem)`

    `iisinfo = copt_computeiis(problem, parameter)`

  - **Description**

    The function has multiple uses given different inputs. 

    If the input is a model filename with a parameter info struct, the function reads the model and parameters from the input, computes IIS for the problem and returns an IIS result info struct.
    If the input is a model info struct with a parameter info struct, the function extracts the relevant information from the input, constructs the model, computes IIS for the problem and returns an IIS result info struct.

  - **Arguments**

    `iisinfo`

      IIS result info struct. Type of MATLAB `struct`.

    `probfile`

      File name of the model to import. Currently support MPS, LP and COPT binary format. Automatically identified by the solver.

    `parameter`

      Parameter info struct. Type of MATLAB `struct`.

    `problem`

      Model info struct. Type of MATLAB `struct`.

  - **Example**

    ```matlab
    iisinfo = copt_computeiis('testmip.mps');

    lpparam.TimeLimit = 10;
    iisinfo = copt_computeiis('testlp.lp', lpparam);
    ```

### Other functions

The `COPT-MATLAB Toolbox` provides  `copt_linprog` and `copt_intlinprog` to overwrite the corresponding `linprog` and `intlinprog` functions in the `MATLAB Optimization Toolbox`.
