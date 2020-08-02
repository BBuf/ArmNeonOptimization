# ArmNeonOptimization

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
|4032x3024|3|Origin Algorithm|10|3850.83ms|1xA53|
|4032x3024|3|RowAndCol Split|10|1041.75ms|1xA53|
|4032x3024|3|RowAndCol Split && Reduce Repeated Computations|10|2599.71ms|1xA53|
|4032x3024|3|Reduce Cache Miss|10|338.97ms|1xA53|

