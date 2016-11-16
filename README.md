# newt

[![Build Status](https://travis-ci.org/cqcallaw/newt.svg?branch=master)](https://travis-ci.org/cqcallaw/newt)

[![Join the chat at https://gitter.im/cqcallaw/newt](https://badges.gitter.im/cqcallaw/newt.svg)](https://gitter.im/cqcallaw/newt?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

newt is an exploration of type safety and immutability, aiming to combine the succinctness and expressiveness of languages such as Python and Ruby with the type safety and semantic analysis found in other languages such as C and C++.

newt is derived from [a compilers class project](http://www.ecst.csuchico.edu/~tyson/classes/515.s15/), and is also a fulfillment of a project requirement for completion of a Bachelor's Degree of Science in Computer Science with Honors from CSU Chico.

newt is [free software](http://www.gnu.org/philosophy/free-sw.en.html), licensed under the [GPLv3](http://www.gnu.org/licenses/gpl-3.0.en.html).

# Philosophy
As creators of software, automation is what we do. We automate because we have the idea that this automation yields value. If we truly believe the idea that automation is valuable, we will apply it wherever possible, including the software tools with which we create software (with some [exceptions](https://xkcd.com/1205/)).

Automation tightens feedback loops, and newt is built on the principle that tighter feedback loops yield solutions faster, and the solutions so yielded are qualitatively better. Here "feedback loop" encompasses everything necessary to create a solution and be confident that it works as intended, and includes authoring code, compilation (if necessary), and all forms of testing.

"boilerplate" code and the manually synchronized declaration and definition exemplified by header files are examples of things that lack automation, and newt's creators are inspired by the succinctness and expressiveness found in languages such as [Python](https://www.python.org/) to avoid such constructs. However, newt is statically typed; that is, variables are associated with a type and may only be assigned values of that type (or its subtypes). This is because dynamic variable typing has been observed to lead to subtle defects, and also requires a great deal of non-automated unit testing to achieve the same correctness guarantees that are more succinctly and precisely expressed in a static type system.

newt also aims to automate immutability, as immutable data structures have been observed to avoid or mitigate many classes of defects. Little prior art is known to exist for such language constructs, so new syntax is introduced to achieve this goal. newt is also authored with the understanding that non-astonishing behavior is important, and that "non-astonishing" frequently just means "familiar." Where possible, existing conventions and well-known programming idioms are preserved.

newt exists because it was believed that the state of the art of software-authoring software could be improved. The language specification and implementation are offered as evidence that an improved solution exists.

# Building
newt requires flex (tested with version 2.6.0), Bison (tested with version 3.0.4), and g++ (tested with 5.4.0) to build. To build a release binary, type the following from the commandline:

```
$ make -C Release all
```
The build process will generate an executable, `newt` in the Release subdirectory.

To run the test suite:
```
$ make -C Release test
```

# Program Execution
The `newt` executable generated the build system is a program interpreter that takes newt script input files as an arguments, and executes them on behalf of the user:

```
$ Release/newt hello.nwt
Welcome to newt!
$
```

# Syntax
newt's syntax is a blend of C-style language constructs and notation from more succinct grammars. The grammar does not include semi-colon statement terminators. Whitespace is not significant; blocks are surrounded by curly braces.

## Hello, World!
```
print("Hello, World!")
```

## Comments
```
print("Hello, World!") # prints "Hello, World!"
```

## Variable Declaration
Variable declaration is a departure from the C-style syntax. Variables are declared by first specifying the variable name, followed by a colon, followed by the variable's type specifier. Variables may be initialized using an appropriate (that is, type-compatible) value. If the variable initializer is specified, the type specifier may be omitted, allowing the variable's type to be inferred.

```
a_lovely_var:int = 42
a_type_inferred_var:= "danny boy" #the type is inferred from the type of the initialization value
a_var:double #every type in newt has an associated default value.
```
### Built-in types
The following types are built-in:
* boolean (default value: false)
* int (default value: 0)
* double (default value: 0.0)
* string (default value: "")
* nil (default unit type)

## Variable Assignment
```
a_lovely_var:int = 42 #variables must be declared before assignment
a_lovely_var = 10
```
## Arrays
Arrays are dynamically sized and multiple dimensions are permitted.

```
a_lovely_array:int[]
a_lovely_multidimensional_array:string[][]

a_lovely_array[5] = 1
a_lovely_multidimensional_array[3][1] = "n"
```

## Default Values
Every type--both built-in and programmer-defined--has a default value. The default value of any type can be accessed using the `@` operator:
```
a_lovely_var:int = @int 
```
The default value of a type is fixed and constant for the lifetime of a program's execution.

## Compound Types

### Record Types
newt supports compound types analagous to C's structs, though unlike C's structs, data is not stored contiguously in memory. In newt, these are called _record types_.

```
person {
	age:int,
	name:string = "Jojo" # default member values allowed
}
```

#### Instantiation
New instances of record types are created by applying a transformation to an existing type instance. The `with` operator followed by a list transformations yields the new instance, as follows:

```
	person2 = person1 with { age = 26 }
```

If no instances of the record type exist, the default value of the record type may be used:

```
	p1:person = @person with { age = 25, name = "Joe" }
```

#### Member Mutation
Records are immutable by default:

```
point {
	x:int,
	y:int
}

point:point = @point with { x = 30, y = 40 }
point.x = 50 # semantic error
```

To make a record mutable, use the `mutable` type modifier 

```
mutable point {
	x:int,
	y:int
}
point:point = @point
point.x = 50
```

### Sum Types
newt provides support for [sum types](https://en.wikipedia.org/wiki/Tagged_union), that is, data structures that can hold a value which may be one--and only one--of several different, but fixed, types. For example, one might model the return type of a function that returns a value _or_ an error as follows:

```
result {
    value:double
    | error {
       int:code,
       string:message
    }
}

f:= (...) -> result { ... }
```

The previous example also serves to demonstrate the manner in which complex types may be declared as members of a containing complex type. `result.error` is a discrete type definition, and is _not_ considered equivalent to any other definition named `error`, even if the structure of the other type is identical. `result.value`, on the other hand, is an _alias_ for the built-in `double` type, and variables of type `result.value` may be assigned to values of type `double`.

#### Instantiating Sum Types
Sum type instantiation is similar to record type instantiation, with the important caveat that type inference can be less intuitive for variables of sum types. Here is an example of a valid sum type instantiation:

```
res:result= @result.error with { code = 3, message = "No more room on disk!" }
```

Note that the type of the variable is specified. If we had allowed the variable type to be inferred, the variable type would have been the narrower `result.error`.

Variables that are of a sum type's member type can automatically be assigned to the wider sum type:

```
res:result
error:= @result.error with { code = 3, message = "No more room on disk!" }
res = error

# widening works for return values too
f:= (...) -> result {
   # ...
   return @result.error with { code = 3, message = "No more room on disk!" }   
}
```

#### Matching
A value that is of a sum type may be any of the sum type's member types, so before the value can be used, the member type of the value must be identified. This is done with a `match` statement, as follows:

```
result {
    value:double
    | error {
       int:code,
       string:message
    }
}

res := get_result_from_function()

match (res)
    value {
        print("value: " + value)
    }
    | error {
        print("error: " + error.message)
    }
```

A match statement must provide match blocks that cover all the members of the sum type. If explicit coverage is not appropriate, a catch-all default block may be used:

```
match (res)
    value {
        print("value: " + value)
    }
    | _ {
        print("default block.")
    }
```

Match blocks may optionally alias the name of discrete matches:
```
match (res)
    value as v {
        print("value: " + v)
    }
    | _ {
        print("default block.")
    }
```

Match aliasing is particularly useful for nested match blocks.

#### Unit Types and Enumerations

Sum types may contain members that are declared without a type. These type members are unit types (that is, types capable of taking on a single value). Thus, the semantic concept of an enumeration can be expressed as a sum type of unit types:

```
color {
    red
    | blue
    | green
    | yellow
    | orange
}
```

#### Maybe Types

One sum type is common enough to warrent special syntax, that of the "Maybe" type. This type is expressed as type indentifier followed by a question mark ('?'):

```
a:int? = 3
```

This type captures the semantic concept of a nullable value, and is semantically equivalent to the following sum type:

```
maybe_int {
    int:value
    | empty
}
```

### Recursive Types

Complex (e.g. record or sum) types may contain members that reference the containing type, provided the reference is wrapped in a Maybe type:

```
list {
    head:int
    tail:list?
}
```

The Maybe-wrapping constraint exists because all types in newt must have a default value, and a clean method for computing the default value of the containing type is not yet known.

## Functions

Functions are first-class citizens in newt, assignable to variables. The syntax for declaring functions is one of the most notable departures from C-style syntax:

```
a:= (a: int, b:int) -> int {
	return a + b
}
```

Higher order functions are also supported; that is, functions may be passed as function arguments:

```
f:= (a:int, b:(int) -> int) -> int {
	return b(a)
}

g:= (d:int) -> int {
	return d * d
}

x:= f(3, g)
y:= f(3, (e:int) -> int { return e * e * e }) # anonymous functions are supported too
```

Functions need not be declared with a body, as every type has a default value. The default value of a function type is a placeholder function that returns the default value of its return type.

```
f:(int) -> int  # will return the default value of the int type if invoked
```
