#ifndef STATE_TABLE_GENERATOR_H
#define STATE_TABLE_GENERATOR_H

namespace StateTableGenerator {

//  <-- Number Sequence Generator Module --> //

template <unsigned... Is>
struct BaseSequence {};

template <unsigned N, unsigned... Is>
struct GenerateSequence : GenerateSequence <N - 1, N - 1, Is...> {};

template <unsigned... Is>
struct GenerateSequence <0, Is...> : BaseSequence <Is...> {};

template <unsigned N>
constexpr
GenerateSequence<N> seq () { return GenerateSequence<N>{}; }



//  <-- Table Module --> //

template <unsigned X, unsigned Y, typename S>
class Table {
    class Row {
        S states_[X];
    public:
        const S& operator[] (unsigned i) const { return states_[i]; }
        S&       operator[] (unsigned i)       { return states_[i]; }
    };

    Row rows_[Y];

public:
    const Row& operator[] (unsigned i) const { return rows_[i]; }
    Row&       operator[] (unsigned i)       { return rows_[i]; }

    typedef S StateType;
};



// <-- Table Generation Module -->

template <unsigned X, unsigned Y, typename S, typename F>
constexpr
Table<X, Y, S> create (S lastState, F next)
{
    return Table<X, Y, S>{};
}



}//StateTableGenerator

namespace stg = StateTableGenerator;

#endif//STATE_TABLE_GENERATOR_H
