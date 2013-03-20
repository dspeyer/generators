Generators
==========

This library provides python-style generators for C++11.

Examples
--------

It's probably easier to learn this library from examples than from this documentation.  Read range_example first, which is a very simple generator.  Then look at recursive_dir_example for a demonstration of recursion, nesting and destructors.

Defining Generators
-------------------

To define a new type of generator, create a struct that inherits GeneratorHeart<OutputType>.  Then define a method called "run".  Instead of returning a value, the function should call "yield" for its values.  Here's a trivial example:

    struct TagonCountTo : GeneratorHeart<int> {
      void run(int target) {
        yield(1);
        yield(2);
        yield(target);
      }
    }

For a simple but not absurd example, see range_example.cc

You can define whatever additional functions or variables you want.  Run can take whatever arguments it likes.  You can even define several run functions with different arguments, or make run take templated parameters.  However, if run is templated, the template must be deduceable from the parameters.

**Warning: The run function might not be allowed to finish.**

If the generator goes out of scope before the run function finishes, an AbortException will be thrown in the generator.  You generally shouldn't catch this exception -- just be aware that yield might throw instead of returning.  Exceptions unwind the stack c++ style, so all destructors will be called.  If you have cleanup that isn't destructors, put it in the generator's destructor (and make relevant variables struct-scope instead of function-scope), but if you can work it as RAII you will probably have an easier time.

Instantiating Generators
------------------------

Create a generator with

    Gen<TypeYouDefined>(arguments, to, run)

Gen is a type.  You can create them on the heap or the stack or even fill a vector with them if you like.

The type passed to Gen must be derived from GeneratorHeart and the arguments to the constructor must be valid arguments to run.

You can create as many generators as you like.  They won't interfear with each other.

Using Generators
----------------

There are two ways to use a generator.  You can access it directly with * ++ and (bool) or you can use a for loop.

    while(gen) {
      do_something(*gen);
      do_something_else(*gen);  // Same value
      ++gen; // This is when we go to the next value
    }

or

    for (auto i : gen) {
      do_something(i);
      do_something_else(i);
    }

To be precise, here is the api:

* Gen<Foo>::Output is the type that it outputs
* *gen gets the current value
* ++gen goes to the next value
* static_cast<bool>(gen) gets whether or not the generator still has data in it
* gen.begin() and gen.end() return wrappers around gen with the semantics that for loops need

Compiling the Code
------------------

The generators library uses c++11 and pthreads.  If you're using gcc, include

    -std=c++11 -pthread

in your command line.

Wait, pthreads?  What?
----------------------

Yes, the implementation is pthreads based.  This way, the generators get their own first class stacks, which they can use however they like.

The code is not multithreaded.  The generator thread only runs when the thread that spawned it is blocked, waiting for it to run.