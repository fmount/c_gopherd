A pure gopherd server implementation
===

This is a simple, minimal implementation of a **Gopher server** written in pure C that follow the reference:

* [RFC1436](https://tools.ietf.org/html/rfc1436)

It's supposed to simply serve the contents you specify defining a root directory in the ~defaults.def.h~ 
file provided to customize the behaviour.

Build from SRC
---
You can simply build it using the provided Makefile.
According to the _$PREFIX_ defined, the default _all_ target generates a bin/ directory containing the result
of the compilation.
The _config_ target is useful to generate a ~lib/defaults.h~ with all the user-defined parameters.

$ make help

will show all the available targets.

$ make config

will generate the defaults.h according to the defaults.def.h

$ make 

will compile and put the result in $(PREFIX) dir.


USAGE
---

    $ ./c_gopherd -h
    Usage ./bin/c_gopherd [-h host] [-p port] [-v]

TODO LIST
---
Here the [todo list](TODO) containing the list of the next feature we MUST have.

