Copyright (C) 2012 Jimmy Lu

State Table Generator
=====================

Introduction
------------

**State Table Generator** is a header only library that is
capable of building state tables at compile time (or run-time).

The biggest advantage is performance.
Two common approaches to initialize state tables are to
delegate the initialization of a state table to the run-time
or using a third party tool to populate the state table initialization before compiling.
This library allows you to code in C++ only and saves the cost of run time initialization.

A C++11 conforming compiler is required to use this library.

How to Use
----------

User will need to create their own State enum's.

Something like the following will do.

    enum State {
        ST_NUL,
        ST_START,
        ST_ID_TRANSITION,
        ST_ID,
        ST_NUMBER,
        // ...
        NUMBER_OF_STATES,
    };

A state traversal function that takes in input and current state,
and returns the next state is required to compute the state table.
Note that this function **must** be a constexpr function.

For example:

    constexpr
    State next (State from, char input) {
        return from == ST_START && (input >= '0' && input =< '9') ? ST_NUMBER :
               from == ST_START && (input == '@') ? ST_ID_TRANSITION :
               // ...
               ST_NUL;
    }

Finally, to create a global compile time initialized table, simply do the following.

    // This table can be used as a part of a tokenizer on ASCII input (0 - 127).
    auto table = stg::create<128, NUMBER_OF_STATES, State>(next);

For more examples, refer to the `test` directory.

Future Plans
------------

* Make it easier to create a state traversal function.
* Make it easier to create a State enum.
