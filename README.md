# Warpcore

![{master}](https://github.com/zeroflag/warpcore/actions/workflows/c-cpp.yml/badge.svg) 

A compact stack-based virtual machine designed for simplicity.

* Stacks: 16-bit cell size for both data and return stacks.
* Instruction Set: 8-bit opcodes and 16-bit addresses.
* Memory: 32 KB of RAM for programs and runtime data.
* Compiler: Self-hosted Forth compiler running directly on the VM.

## Language

```Forth
ENTRY

s" Hello World!" PRINT

BYE
```
