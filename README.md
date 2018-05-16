Process Finder
===
by B062515 Tzu-Hsuan Tseng

## How to use
Just `make` to compile and `./procFinder [OPTION]` to launch the program.

## Acceptable arguments
-a : list processes from all the users.
-x : list processes without an associated terminal.
-p : sort the listed processes by pid(default).
-q : sort the listed processes by ppid.
-r : sort the listed processes by pgid.
-s : sort the listed processes by sid.

## Note
If there are more than one sorting arguments(-p, -q, -r, -s), the first one will be adopted.
If there are some arguments not support, the program will print the error message at the beginning.
like : ./procFinder: invalid option -- 'b'

