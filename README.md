Generators
==========

This library provides python-style generators for C++11.

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

You can define whatever additional functions or variables you want.  Run can take whatever arguments it likes.  You can even define several run functions with different arguments, or make run take templated parameters.

**Warning: The run function might not be allowed to finish.**

If the generator is destroyed, the function will be rudely interrupted.  So rudely that destructors for local variables will not run.  Destructors for struct-level variables will run, as will the struct's own destructor.

The following code might leak memory:

    struct foo : GeneratorHeart<int> {
      void run() {
        vector<int> scratch(1000);

The safe way to write this is:

    struct foo : GeneratorHeart<int> {
      vector<int> scratch;
      void run() {
        scratch.resize(1000);

Or

    struct foo : GeneratorHeart<int> {
      vector<int> scratch;
      foo() : scratch(1000) {}
      void run() {

Similarly, this also might leak memory:

    struct foo : GeneratorHeart<int> {
      void run() {
        char* scratch = new char[1000];
        // do stuff
        delete scratch;
      }
    }

The safe version is:

    struct foo : GeneratorHeart<int> {
      char* scratch;
      void run() {
        scratch = new char[1000];
        // do stuff
      }
      ~foo() {
        delete scratch;
      }
    }

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