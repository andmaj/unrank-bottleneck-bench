# randbitmap

*Generates a repeatable random bitmap. This is accomplished by using an internal pseudorandom number generator.*

For example creating a bitmap named "bitmap.dat" with 10 % density and 1000 bits length by seed 1234:
```randbitmap 1234 0.1 1000 bitmap.dat```

## Usage

```randbitmap [seed] [density] [bitsnumber] [filename]```


## Build
Program should build on any UNIX like operation system with a standard C compiler and make utility.

To compile:
```
make
```

To run:
```
./bin/randbitmap
```

To clean:
```
make clean
```

## Author

Written by Andras Majdan.

License: GNU General Public License Version 3

Report bugs to <majdan.andras@gmail.com>
