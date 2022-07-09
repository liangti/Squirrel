# Namespaces

This document is to define some policies for namespaces in Sqirrel library.

## sqrl

Is the main namespace to have all APIs directly provides to user.

User could use directly and could see example of usages in `t/`.

## sqrl::details

Is the namespace for internal implementation details, that user should not use
directly.

## sqrl::compiler

Is new added namespace for hooking the compiler. User not should invoke unless they
clearly understand the language implementation(such as ABI)

In directory `include/compiler` there are few APIs that is used to provide
functionality that compiler should provide, such as class inheritance relationship,
object memory layout.

*May consider codegen in the future*

## Caveat

Currently the implmentation of `dyn_cast` highly relies on extra compiler informations.

It may have undefined behavior without those `sqrl::compiler` APIs calls.