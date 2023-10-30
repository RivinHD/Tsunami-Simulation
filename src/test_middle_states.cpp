/**
 * @author Fabian Hofer, Vincent Gerlach (AT uni-jena.de)
 *
 * @section DESCRIPTION
 * Entry-point for middle states tests.
 **/
#include "../include/patches/WavePropagation1d.h"
#include "../include/setups/DamBreak1d.h"
#include "../include/io/Csv.h"
#include "../include/constants.h"
#include <cstdlib>
#include <iostream>
#include <cmath>
#include <fstream>
#include <limits>
#include <string>
#include <filesystem> // requieres C++17 and up

int main()
{
    std::ifstream middle_states("../resources/middle_states.csv");
    
    tsunami_lab::t_real hLeft, hRight, huLeft, huRight, hStar;
    while (tsunami_lab::io::Csv::next_middle_states( middle_states,
                                                     hLeft,
                                                     hRight,
                                                     huLeft,
                                                     huRight,
                                                     hStar))
    {
        // TODO Task 2.0.2
    }

    return 0;
}
