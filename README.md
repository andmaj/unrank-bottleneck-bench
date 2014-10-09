# Unrank bottleneck benchmark

*Benchmark unrank operation cost of access using libcds and sdsl library.*

## To make

```make```

## To test libcds

```make cds-bench```

## To test sdsl

```make sdsl-bench```

## Between tests

```make clean```


## Results (Intel Q6600):

Not a bottleneck in libcds (because using a lookup table).

Unrank takes 27-47 % of access time in sdsl.


## Author

Written by Andras Majdan.

License: GNU General Public License Version 3

Report bugs to <majdan.andras@gmail.com>
