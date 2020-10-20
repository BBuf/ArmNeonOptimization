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
|4032x3024|3|Neon Assembly+Diff Predeal|10|181.40ms|1xA53|
|4032x3024|3|Neon AssemblyV2|10|145.92ms|1xA53|
|4032x3024|3|NCNN Origin|10|281.26ms|1xA53|
|4032x3024|3|NCNN Neon Intrinsics|10|236.82ms|1xA53|
|4032x3024|3|NCNN Neon Assembly|10|68.54ms|1xA53|
|4032x3024|3|NCNN Neon AssemblyV2|10|61.63ms|1xA53|

## 2. WinoGrad3x3s1 F(6, 3) Algorithm Version1.0

|inputHeight|inputWidth|inputChannel|KernelSize|outChannel|Optimization Algorithm|Loop Count|Time|核心数|
|--|--|--|--|--|--|--|--|--|
|15|15|512|3x3|1024|手工优化|10|582.67ms|1|
|15|15|512|3x3|1024|WinoGrad Version1.0|10|336.81ms|1|
|56|56|64|3x3|128|手工优化|10|124.63ms|1|
|56|56|64|3x3|128|WinoGrad Version1.0|10|60.41ms|1|
|56|56|64|3x3|128|手工优化|10|61.16ms|2|
|56|56|64|3x3|128|WinoGrad Version1.0|10|32.69ms|2|
|6|6|512|3x3|1024|手工优化|10|74.03ms|1|
|6|6|512|3x3|1024|WinoGrad Version1.0|10|76.30ms|1|
|6|6|512|3x3|1024|手工优化|10|36.49ms|2|
|6|6|512|3x3|1024|WinoGrad Version1.0|10|41.67ms|2|













