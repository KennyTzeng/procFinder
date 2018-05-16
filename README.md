Process Finder
===
by B062515 Tzu-Hsuan Tseng

## How to use
Just `make` to compile and `./procFinder [OPTION]` to launch the program.

## Acceptable arguments
-a : list processes from all the users.</br>
-x : list processes without an associated terminal.</br>
-p : sort the listed processes by pid(default).</br>
-q : sort the listed processes by ppid.</br>
-r : sort the listed processes by pgid.</br>
-s : sort the listed processes by sid.</br>

## Note
If there are more than one sorting arguments(-p, -q, -r, -s), the first one will be adopted.</br>
If there are some arguments not support, the program will print the error message at the beginning.</br>
like : ./procFinder: invalid option -- 'b'</br>

