TLSS
====

Tiny Little Sudoku Solver
-------------------------

> Note: This currently doesn't solve sudokus yet. It's currently just
> a framework to add solvers to.

I have been getting distracted solving sudokus recently. I don't know
if it's because I'm a programmer, or just odd, but my approach to
solving them left me thinking: I bet I could make a computer solve
these. I then knocked together a quick proof of concept in C++,
following a very naive approach, of basically just checking rows,
columns and boxes, with no frills. It works for simple sudoku, but
any sudoku where you get a
[swordfish](https://sudoku.com/sudoku-rules/swordfish/), or triplets,
or basically anything that needs context of more than one square, and
it will fail.

I did start looking at how I could add more complex solvers to my C++
solver, but decided that this is a perfect excuse for some over
engineering, and to rewrite it in C.

> I feel the need to defend my sudoku-solving honour here. This is not
> in any way intended to be a tool for me to cheat sudokus, or for you
> to use to that end. If you don't enjoy solving them yourself, then
> just don't solve them. Using a tool to do the problem solving
> removes all fun from it. I am only doing this because figuring out
> how to write it is another form of problem solving for me, so this
> is purely for my own enjoyment. As such, the overengineering is part
> of the fun.

As with all my "Tiny Little" libraries, this is not intended to be a
standalone application (I may make a "reference implementation" down
the line, which would read sudokus from the command line or something)
but a library to solve one small problem (solving sudokus) which can
be integrated into a larger project. The design of the library follows
that of its sister libraries. It is intended to have little to no
dependencies, the API should be fairly small and self explanitory, and
it should be heavily unit tested. While effort is made to keep the
executable size small (particularly easy in C of course) the "Tiny" in
the title refers not to the size of the library on disk, either in its
source or compiled form, but its usage. As mentioned, the API is
designed to be minimal and concise, and it does not perform any
additional tricks other than its core functionality. As such
integrating it into a program should be fairly straight forward.

```c
// Setup
tlssContext* context;
tlssInitContext(&context);
tlssDigit data = { 
0, 0, 1,  0, 6, 2,  4, 0, 9
//...
9, 0, 0,  2, 4, 0,  7, 1, 0
};
tlssGrid* grid;
// Load the starting grid
tlssLoad(context, data, &grid);

// Solve the grid
tlssGrid* solution;
tlssSolve(context, grid, &solution); // not currently implemented

// Get the data out
tlssDigit v;
tlssGetValue(context, solution, 0, 0, v);
printf("Value at R1C1: %d\n", v);
tlssReleaseGrid(&solution);
```

There are reasons for this design, while it's a little unlike some
other libraries, I find it makes using them easier and safer once I
get used to them. Each function returns a tlssReturn code (unchecked
in the above example) which means that you can easily check for any
errors at any point, making it far easier to catch errors when they
happen. The concept of a context feels a bit OO, which makes it a bit
odd, this library having been made originally from a C++ one. The
reality is that pretty much any library needs to maintain some form
of state. While I don't necessarily forsee there being situations
where one application runs multiple solvers concurrently, causing
issues with this, I find it a fairly minor inconvenience to use, and
has the benefit that I have a central store of my library's state.
The final potential oddness is that the grids are immutable. If you
call `tlssSetValue` it does not set a value in the grid provided, but
rather returns a new grid with the value set. This might seem
wasteful, but the data set is small to copy (81 bytes) and it will
actually not do many allocations, as, if you release grids, it will
add them to a pool from which they can be re-used without first
allocating them. The reason behind doing it this way is that I intend
to try to make the solver parallel. For this it will run multiple
solvers concurrently, and after every step, merge their results. Does
it need to do this? No, of course not, I doubt that even running this
on a potato, sequentially, it will be slow enough to really worry
about, but I want to experiment, and this is a nice small project to
do that with.

Compiling the library is simple:
```bash
make CONFIG=debug
make CONFIG=release
make # Same as CONFIG=release
make tlss-dynamic
make tlss-static
make test # Same as make
```

If you're wanting to use the library in your project (why??) then just
running `make` will be sufficient. This will build both the static and
dynamic versions of the library, the testsuite, and run the tests.

> To be able to build the tests, first you need to ensure the
> testsuite is cloned, which can be done by passing --recursive when
> cloning the tlss repository.
> Also needed for now is patchelf. I am hoping to drop this
> dependency, but it is used to make the testsuite link to the .so
> I should be able to do this with -Wl,rpath=${OUTDIR} in the Makefile
> but it didn't work. If you don't have patchelf and don't want to get
> it, then it's fairly trivial to link against the static library ---
> or just don't run the test suite.

There are three important targets:
* `tlss-dynamic` - will generate `libtlss.so`
* `tlss-static` - will generate `libtlss.a`
* `test` - will generate `tests` and run it

There are two configurations:
* `debug` - Includes debug symbols (-g)
* `release` - Optimisations enabled (-O3)

In future this may change so that there are more differences, and I
may add more configurations - There is already a define to disable all
NULL checks, but as the only use case right now is the test suite,
which relies on the NULL checks as part of its testing, but this may
be a candidate for a streamlined configuration.