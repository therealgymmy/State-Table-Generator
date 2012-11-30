#include "../src/StateTableGenerator.h"

enum State {
    LAST_STATE,
};

State next (State from);

int main ()
{
    stg::seq<256>();

    stg::create<10, 256>(LAST_STATE, next);
}
