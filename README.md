# aaArmNeonOptimization

## Environment

Hisi 3519A-> (2 x A53)

## Build And Run

1. Remove all localized settings

run command

```
export LC_ALL=C
```

2. Make

run command

```
make clean; make -j4
```

## Speed Test

### 1. Box Filter Algorithm

|Image Resolution|Radius|Optimization Algorithm|Loop Count|Time|核心数|
|--|--|--|--|--|---|
|4032x3024|3|Origin Algorithm|10|2313.40ms|1xA53|
|4032x3024|3|RowAndCol Split|10|784.98ms|1xA53|
|4032x3024|3|RowAndCol Split && Reduce Repeated Computations|10|2496.55ms|1xA53|
|4032x3024|3|Reduce Cache Miss|10|302.00ms|1xA53|
|4032x3024|3|Neon Intrinsics|10|188.37ms|1xA53|
|4032x3024|3|Neon Assembly|10|187.7ms|1xA53|
|4032x3024|3|Neon Assembly+pld|10|158.70ms|1xA53|


