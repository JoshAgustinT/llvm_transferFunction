### Assignment 12: working with LLVM abstract transfer functions 
I chose the integer ranges abstract domain and tested the precision of 'makeMaskNotEqualRange':

https://github.com/llvm/llvm-project/blob/main/llvm/include/llvm/IR/ConstantRange.h#L182
```
  /// Initialize a range containing all values X that satisfy `(X & Mask)
  /// != C`. Note that the range returned may contain values where `(X & Mask)
  /// == C` holds, making it less precise, but still conservative.
  static ConstantRange makeMaskNotEqualRange(const APInt &Mask, const APInt &C);
```
GitHub Repo of Assignment:
https://github.com/JoshAgustinT/llvm_transferFunction/tree/main

Example:

```
    APInt Mask(4, 0b0011); // Example Mask 
    APInt C(4, 0b0001);    // Example C 
    //Calling llvm's tranfser function & printing result
    ConstantRange range = ConstantRange::makeMaskNotEqualRange(Mask, C);
    range.dump();
```
> [2,1)

This means it wraps around and we can interpret our result as:

X values that when & by 'Mask' != C,
ie:
X & 0011 != 0001

Printing the result with helper function: 
> {2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 0}

Which equals:
0010(2), 0011(3), 0100(4), 0101(5), 0110(6), 0111(7),
1000(8), 1001(9), 1010(10), 1011(11), 1100(12), 1101(13),
1110 (14), 1111(15), 0000(0)

In this case we should have values who's lower bits  don't have XX01,
which is proof that it's not very precise ie 5, 9, 13.
    
    
Running my transfer function implementation & printing it out:
```
    vector<APInt> josh_range = josh_transfer_function(Mask, C);
    vector<ConstantRange> josh_CR = abstraction_function(josh_range);
    for (auto token : josh_CR)
        dump_unsigned(token);
```
> [0,1), [2,5), [6,9), [10,13), [14,0)

As it's the naive and slow implementation we get what we expected, printing:
> {0, 2, 3, 4, 6, 7, 8, 10, 11, 12, 14, 15}

<div style="page-break-after: always;"></div>

Report:


=== Precision Comparison Summary ===

Bitwidth tested: 1

Total number of abstract values: 2

LLVM more precise cases: 0

Josh's transfer function more precise cases: 1

Equally precise cases: 3

Finished in: 4.8988e-05 seconds


=== Precision Comparison Summary ===

Bitwidth tested: 2

Total number of abstract values: 4

LLVM more precise cases: 0

Josh's transfer function more precise cases: 3

Equally precise cases: 13

Finished in: 6.4408e-05 seconds

=== Precision Comparison Summary ===

Bitwidth tested: 3

Total number of abstract values: 8

LLVM more precise cases: 0

Josh's transfer function more precise cases: 13

Equally precise cases: 51

Finished in: 0.000465669 seconds

=== Precision Comparison Summary ===

Bitwidth tested: 4

Total number of abstract values: 16

LLVM more precise cases: 0

Josh's transfer function more precise cases: 51

Equally precise cases: 205

Finished in: 0.00305395 seconds

=== Precision Comparison Summary ===

Bitwidth tested: 5

Total number of abstract values: 32

LLVM more precise cases: 0

Josh's transfer function more precise cases: 181

Equally precise cases: 843

Finished in: 0.0130475 seconds


=== Precision Comparison Summary ===

Bitwidth tested: 6

Total number of abstract values: 64

LLVM more precise cases: 0

Josh's transfer function more precise cases: 603

Equally precise cases: 3493

Finished in: 0.0889425 seconds


=== Precision Comparison Summary ===

Bitwidth tested: 7

Total number of abstract values: 128

LLVM more precise cases: 0

Josh's transfer function more precise cases: 1933

Equally precise cases: 14451

Finished in: 0.633192 seconds


=== Precision Comparison Summary ===

Bitwidth tested: 8

Total number of abstract values: 256

LLVM more precise cases: 0

Josh's transfer function more precise cases: 6051

Equally precise cases: 59485

Finished in: 4.70087 seconds


=== Precision Comparison Summary ===

Bitwidth tested: 9

Total number of abstract values: 512

LLVM more precise cases: 0

Josh's transfer function more precise cases: 18661

Equally precise cases: 243483

Finished in: 34.4738 seconds


=== Precision Comparison Summary ===

Bitwidth tested: 10

Total number of abstract values: 1024

LLVM more precise cases: 0

Josh's transfer function more precise cases: 57003

Equally precise cases: 991573

Finished in: 276.043 seconds

=== Precision Comparison Summary ===

Bitwidth tested: 11

Total number of abstract values: 2048

LLVM more precise cases: 0

Josh's transfer function more precise cases: 173053

Equally precise cases: 4021251

Finished in: 2031.73 seconds

