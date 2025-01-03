[![INFORMS Journal on Computing Logo](https://INFORMSJoC.github.io/logos/INFORMS_Journal_on_Computing_Header.jpg)](https://pubsonline.informs.org/journal/ijoc)


# Iterated Inside Out Algorithm

This archive is distributed in association with the [INFORMS Journal on
Computing](https://pubsonline.informs.org/journal/ijoc) under the [CC BY-NC-SA 4.0 License](LICENSE).

The software and data in this repository are a snapshot of the software and data
that were used in the research reported on in the paper [Iterated Inside Out: a new exact algorithm for the transportation problem](https://doi.org/10.1287/ijoc.2024.0642) by R. Bargetto, F. Della Croce, and R. Scatamacchia.

The snapshot is based on this [GitHub repository](https://github.com/robertobarg/IteratedInsideOut).


## Cite
To cite the contents of this repository, please cite both the paper and this repo, using their respective DOIs.

https://doi.org/10.1287/ijoc.2024.0642

https://doi.org/10.1287/ijoc.2024.0642.cd

Below is the BibTex for citing this snapshot of the repository.

```
@misc{iio2024,
  author =        {Roberto Bargetto and Federico Della Croce and Rosario Scatamacchia},
  publisher =     {INFORMS Journal on Computing},
  title =         {Iterated Inside Out: a new exact algorithm for the transportation problem},
  year =          {2024},
  doi =           {10.1287/ijoc.2024.0642.cd},
  url =           {https://github.com/INFORMSJoC/2024.0642},
  note =          {Available for download at https://github.com/INFORMSJoC/2024.0642},
}
```

## Description
This software provides the implementation of Iterated Inside Out, a new exact algorithm for solving the transportation problem.
The software also implements the [Shielding Method](https://doi.org/10.1007/s10851-016-0653-9) by B. Schmitzer and a standard transportation simplex.


## Content of the repository
This repository includes

* the C++ source code of the algorithm, directory [src](src/),
* the complete results of the paper experiments, directory [results](results/),
* the program configuration files to replicate the paper experiments, directory [cfgs](cfgs/),
* an SQL script for creating a relational database of the optimization results, directory [sql](sql/), and
* a [makefile](Makefile) to compile the C++ code and generate the executable file.


## Problem instances
All the instances we generated for the paper experiments are available at this [link](https://www.dropbox.com/scl/fo/e9y2yb1h09r7ygv32luia/ACa-Lhzjmpti8UIAwbSrj3c?rlkey=d3t016b3jagfmazv1ij6zo05e&st=o4vb12o3&dl=0).
Problem instance are text files.

Each instance file contains a list of space-separated values fully describing a transportation problem.
The first line of a file contains the number of sources `M`, the number of destinations `N`, and a dummy value.
The second line reports the quantities at sources and the third line the quantities at destinations.
From the fourth line on, the file contains the `M x N` matrix of the transportation costs.


## Instruction to run the program
If you compile the source code with the makefile we provide, you will find the new executable file in the subdirectory [bin](bin/).
In the Linux command line, use the following command to run the program.
```
./bin/iio instancefile.txt cfgs/iio.cfg

```
The command above runs the program silently (no message is sent to standard output) and all the messages are written to a file with extension `.log` created in the program execution directory.
If you want the program to write messages to standard output, add a third argument in the command line.
As an example
```
./bin/iio instancefile.txt cfgs/iio.cfg stdo

```

The program writes the optimization results to a file with extension `.optres` created in the execution directory.
The file contains a single line of space-separated values.
Comments in the SQL file [sql/result.sql](sql/result.sql) describe the space-separated values as they are written into the `.optres` file by the program.

Compilation and sample instance solution tests have been run also on a machine running Windows operating system.


## Ongoing development and support
This code is being developed on an ongoing basis.
You may want to check out the code main developer's [GitHub site](https://github.com/robertobarg).

For support in using this software, submit an
[issue](https://github.com/robertobarg/IteratedInsideOut/issues/new).

To be in touch, [send us an email](mailto:roberto.bargetto@polito.it;federico.dellacroce@polito.it;rosario.scatamacchia@polito.it?cc=roberto.bargetto@gmail.com&subject=IIO%20Repo%20-%20Question).


## License
Repository license file [LICENSE](LICENSE).

Shield: [![CC BY-NC-SA 4.0][cc-by-nc-sa-shield]][cc-by-nc-sa]

This work is licensed under a
[Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License][cc-by-nc-sa].

[![CC BY-NC-SA 4.0][cc-by-nc-sa-image]][cc-by-nc-sa]

[cc-by-nc-sa]: http://creativecommons.org/licenses/by-nc-sa/4.0/
[cc-by-nc-sa-image]: https://licensebuttons.net/l/by-nc-sa/4.0/88x31.png
[cc-by-nc-sa-shield]: https://img.shields.io/badge/License-CC%20BY--NC--SA%204.0-lightgrey.svg