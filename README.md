# newt

[![Build Status](https://travis-ci.org/cqcallaw/newt.svg?branch=master)](https://travis-ci.org/cqcallaw/newt)

[![Join the chat at https://gitter.im/cqcallaw/newt](https://badges.gitter.im/cqcallaw/newt.svg)](https://gitter.im/cqcallaw/newt?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

newt is scripting language that aims to be simple and expressive, but also send signals about program correctness issues to the developer as quickly as possible. These goals are born of a dissatisfaction with both the relative verbosity of "system" programming languages such as C/C++, as well as the tendency among scripting languages such as Python or Ruby to defer detection of correctness issues until runtime.

newt is derived from [a compilers class project](http://www.ecst.csuchico.edu/~tyson/classes/515.s15/), and is also a fulfillment of a project requirement for completion of a Bachelor's Degree of Science in Computer Science with Honors from CSU Chico.

newt is [free software](http://www.gnu.org/philosophy/free-sw.en.html), licensed under the [GPLv3](http://www.gnu.org/licenses/gpl-3.0.en.html).

# Design Philosophy

Software is valuable because there exists a class of computations for which machines are faster and more trustworthy than people, that is, software is valuable because automation is valuable. If we truly believe automation is valuable, we automate wherever possible, including the software tools with which we create software (with some [exceptions](https://xkcd.com/1205/)). newt exists because it is believed that software-crafting software can be improved through automation; the language specification and implementation are offered as evidence that an improved solution exists.

Semantic analysis is a class of computations with ample opportunity for automation, and one where automation yields exceptional value; the sooner the programmer knows the program to be incorrect, the sooner the program can be fixed. In other words, semantic analysis helps detect more bugs and helps detect them faster, and the quality of the program is thereby improved. For this reason, newt strives to detect semantic errors as early in the program lifecycle as possible. In support of earliest-possible error detection, newt is statically typed; that is, a variable is associated with a type and this type association constrains the set of values which the variable may legally represent. This allows the language runtime to detect large numbers of semantic errors before the program executes, and automates much of the quality assurance work that must be done with hand-coded unit tests in a dynamically typed language.

Syntatically, newt aims for pragmatism, avoiding "boilerplate" code (e.g. header files) and generally minimizing programmer input as long as readability is not impaired. It is understood that non-astonishing behavior is important, and that "non-astonishing" frequently just means "familiar", so existing conventions and well-known programming idioms are preserved where possible. newt does not adhere slavishly to existing structures, however; for example, the language introduces language constructs to automate use of immutable data structures, which have been observed to mitigate or obviate many classes of defects. Little prior art is known to exist for such language constructs, so new syntax is introduced to achieve this goal.

The reference implementation of newt attempts to embody the language philosophy itself, placing particular priority on following software engineering best practices.

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
### Primitive Types
The following primitive types are supported by the language:
* bool (default value: false)
* byte (default value: 0)
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
newt provides support for [sum types](https://en.wikipedia.org/wiki/Tagged_union), that is, data structures that can hold a value which may be one--and only one--of several different, but fixed, types. For example, one might model a generic color as either RGB _or_ HSL values:

```
color {
	rgb {
		r:int,
		g:int,
		b:int,
	}
	| hsl {
		h:double,
		s:double,
		l:double,
	}
}
```

One might also model the return type of a function that returns a value _or_ an error as follows:

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

The values of unit variants of a sum type may be accessed in the same way as any other variant:

```
color {
    red
    | blue
    | green
    | yellow
    | orange
}

c:color= @color.red
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
    | nil
}
```

The 'nil' variant of the Maybe type is an alias for the universal `nil` unit type, so `int?.nil` is functionally equivalent to `nil`.

### Type Nesting

Complex types may be arbitarily nested. It is thus possible to expand the previous generic color type to include an alpha channel as follows:

```
color {
	value {
		rgb {
			r:int,
			g:int,
			b:int,
		}
		| hsl {
			h:double,
			s:double,
			l:double,
		}
	},
	alpha:int
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

### Built-in Complex Types

newt provides a number of foundational built-in types in support of basic operations such as I/O:

```
error { id:int, message:string }
error_list { next:error_list?, data:error }
byte_read_result { eof | data:byte | errors:error_list }
byte_result { data:byte | errors:error_list }
int_result { data:int | errors:error_list }
stream_mode { read:bool, write:bool, binary:bool, ate:bool, app:bool, trunc:bool }
```

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

### Built-in Functions

The following functions are built-in:

* open (path:string, mode:stream_mode) -> int_result
* close (file_handle:int) -> error_list?
* get (file_handle:int) -> error_list?
* put (file_handle:int, data:byte) -> error_list?

## Resource Management

newt supports automatic resource disposal via `using` expressions:

```
# basic using block
disposable {
	teardown := (this:disposable?) -> error_list? {
		return nil
	}
}

r := using @disposable as value -> int_result {
	return 1
}
```

A disposable type must be a record type that defines a `teardown` member function that takes a reference to a disposable instance as an argument. The teardown function must return type that is compatible with the return type of the `using` block; `error_list` is usually the best choice. This allows teardown errors to propagate to the consumer of the using block's output.

setup is handled in a separate setup function:

```
# basic using block with setup
disposable {
	teardown := (this:disposable?) -> error_list? {
		return nil
	}
}

disposable_setup_result {
	setup_value:disposable
	| setup_errors:error_list
}

get_disposable := () -> disposable_setup_result {
	return @disposable
}

r := using get_disposable() as disposable -> result {
	return 1
}
```
## Interpreter Targets

newt supports specification of a target interpreter version for a given script via the directive `$target major.minor[.optional_patch]`. Compatibility between the interpeter and the script target is determined by the rules of [semantic versioning](https://semver.org/spec/v2.0.0.html); that is, the script target's major version must match the interpeter's, the target minor version must be greater than or equal to the interpreter's, and the target patch version must be greater than or equal to the interpreter's.

The target version is generally not useful unless it is specified at the beginning of the input, but this best-practice is not enforced by the interpreter.
