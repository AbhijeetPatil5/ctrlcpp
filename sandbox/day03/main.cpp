#include <cmath>
#include <iostream>
#include <iomanip>
#include <format>
#include "euler.hpp"

int main () {
    const double tau = 2.0, K = 3.0, u = 1.0, dt = 0.1, t_end = 10.0;
    double y = 0.0;

    std::cout << "Euler stable for dt < " << eulerStabilityLimit(tau) << " sec\n";

    std::cout << std::setprecision(6);

    for (double t = 0.0; t <= t_end + 1e-12; t += dt) {
        std::cout << t << "," << y << "," << K * (1 - std::exp( -t / tau)) << "\n";
        y = eulerStepThermal(y, u, dt, tau, K);
    }

    return 0;
}