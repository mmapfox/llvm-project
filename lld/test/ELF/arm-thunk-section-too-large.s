// REQUIRES: arm
// This test intentionally checks for fatal errors, and fatal errors aren't supported for testing when main is run twice.
// XFAIL: main-run-twice
// RUN: llvm-mc %s -triple=armv7a-linux-gnueabihf -arm-add-build-attributes -filetype=obj -o %t.o
// RUN: not ld.lld %t.o -o /dev/null 2>&1 | FileCheck %s

// RUN: llvm-mc %s -triple=armv7aeb-linux-gnueabihf -arm-add-build-attributes -filetype=obj -mcpu=cortex-a8 -o %t.o
// RUN: not ld.lld %t.o -o /dev/null 2>&1 | FileCheck %s
// RUN: not ld.lld --be8 %t.o -o /dev/null 2>&1 | FileCheck %s

// CHECK: InputSection too large for range extension thunk
        .syntax unified
        .thumb
        .text
        .globl _start
        .type _start, %function
_start:
        .space 2 * 1024 * 1024
        // conditional branch has range of 1 Mb expect error as we can't place
        // a thunk in range of the branch.
        beq target
        .space 2 * 1024 * 1024

        .section .text.2, "ax", %progbits
        .globl target
        .type target, %function
target: bx lr
