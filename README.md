A pure gopherd server implementation
===

This is a simple, minimal implementation of a **Gopher server** written in pure C that follow the reference:

* [RFC1436](https://tools.ietf.org/html/rfc1436)

It's supposed to simply serve the content you specify defining a root directory in the _defaults.def.h_
file.

Build from SRC
---

You can simply build it using the provided Makefile.<br>
According to the defined _$PREFIX_, the default _all_ target generates a bin/ directory containing the result
of the compilation.<br>
The _config_ target is useful to generate a _lib/defaults.h_ with all the user-defined parameters.


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


THE GOPHER PROTOCOL AND DATA TYPES
---
In the /example folder there is a little Gophermap file.<br>
By customizing the **GOPHER_ROOT** directory (aka *GROOT*)
you can serve it as an *HTML equivalent index*.<br>
In this way you can display your custom Gopher menu instead of just a directory listing.<br>
A good approach enabled by the use of the gophermap is represented by the ability to create
nested [gopherholes](https://gopher.zone/posts/tutorial-for-absolute-beginners) indexed with
their own gophermap in a structure like the following:

    example/
      gophermap
      user1/
        gophermap
        user_1_content/
      user2
        gophermap
        user_2_content/
      content1/
      content2.txt
      ...
      ...


How the gophermap is built?
---

According to the rfc and the evolution of the protocol, the following table specifies the format and the content types
allowed.<br>
The technical specification for Gopher, RFC 1436, defines 14 item types. A one-character code indicates what kind of
content the client should expect.<br>
Item type 3 is an error code for exception handling. Gopher client authors improvised item types h (HTML),
i (informational message), and s (sound file) after the publication of RFC 1436, and I think the informational message
is very useful to organize better a gophermap and present the resources in a cleaner way.<br>

| Type   |      Description      |
|:----------:|:-------------|
| 0 | text file |
| 1 | Gopher submenu   |
| 2 | CCSO Nameserver |
| 3 | Error code returned by a Gopher server to indicate failure |
| 4 | BinHex-encoded file (primarily for Macintosh computers)   |
| 5 | DOS Files |
| 6 | uuencoded file |
| 7 | Gopher full-text search   |
| 8 | Telnet |
| 9 | Binary file |
| + | Mirror or alternate server (for load balancing or in case of primary server downtime)   |
| g | GIF file |
| I | Image file |
| T | Telnet 3270 |
| i | informational message |
| h | HTML file |
| s | Sound file |

An example of gophermap can be [found here](example/gophermap).


TODO LIST
---
Here the [todo list](TODO) containing the list of the next features we MUST have.

