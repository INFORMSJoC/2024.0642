# Iterated Inside Out Algorithm

This repository provides the original implementation of the Iterated Inside Out (IIO) algorithm as described in the paper [Iterated Inside Out: a new exact algorithm for the transportation problem](https://pubsonline.informs.org/journal/IJOC).
If you use the material of this repository, please consider citing our paper.

The repository includes the source C++ code of the algorithm, directory [src](src/), the complete results of the paper experiments, directory [results](results/), and a `Makefile` in the root directory.
The repository also includes a Linux executable file in the directory [bin](bin/).

All the instances we generated for the paper experiments are available at [instances](https://www.dropbox.com/scl/fo/e9y2yb1h09r7ygv32luia/ACa-Lhzjmpti8UIAwbSrj3c?rlkey=d3t016b3jagfmazv1ij6zo05e&st=o4vb12o3&dl=0).

The directory [cfgs](cfgs/) contains the configuration files to pass as second argument to the command line program.
The directory contains the program configuration files required to replicate the paper experiments.

If you compile the source code with the provided makefile, you will find the new executable file in the directory [bin](bin/).
In the Linux command line, use the following command to run the program.
```
./bin/iio instancefile.txt cfgs/iio.cfg

```
The command above runs the program silently (no message is sent to std output) and all the messages are written to a file with extension `.log` created in the program execution directory.
If you want the program to write messages to std output, add a third argument to the command.
As an example
```
./bin/iio instancefile.txt cfgs/iio.cfg stdo

```

The result of the optimization is written to a file with extension `.optres` created in the program execution directory.
The file contains a single line of space-separated values.
A `.sql` file for creating a relational database of the optimization results is in the directory [sql](sql/).
Comments in the SQL file [sql/result.sql](sql/result.sql) describe the space-separated values as they are written into the `.optres` file by the program.

Compilation and sample instance solution tests have been run on a machine running Windows operating system.

For questions on this repository, [send us an email](mailto:roberto.bargetto@polito.it?cc=roberto.bargetto@gmail.com;rosario.scatamacchia@polito.it;federico.dellacroce@polito.it&subject=IIO%20Repo%20-%20Question).


Shield: [![CC BY-NC-SA 4.0][cc-by-nc-sa-shield]][cc-by-nc-sa]

This work is licensed under a
[Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License][cc-by-nc-sa].

[![CC BY-NC-SA 4.0][cc-by-nc-sa-image]][cc-by-nc-sa]

[cc-by-nc-sa]: http://creativecommons.org/licenses/by-nc-sa/4.0/
[cc-by-nc-sa-image]: https://licensebuttons.net/l/by-nc-sa/4.0/88x31.png
[cc-by-nc-sa-shield]: https://img.shields.io/badge/License-CC%20BY--NC--SA%204.0-lightgrey.svg
