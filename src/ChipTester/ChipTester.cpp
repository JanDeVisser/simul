/*
 * Copyright (c) 2024, Jan de Visser <jan@finiandarcy.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include <thread>

#include "Circuit.h"
#include "IC/LS193.h"

namespace ChipTest {

using namespace Simul;

void main()
{
    auto    quit { false };
    Circuit circuit;

    test_device<JKFlipFlop>();
}

}

int main()
{
    ChipTest::main();
    return 0;
}
