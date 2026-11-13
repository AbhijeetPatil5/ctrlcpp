#include <cmath>
#include <iomanip>
#include <iostream>
#include <format>

int main() {
    const double tau = 2.0, K = 3.0, u = 1.0, dt = 0.1, t_end = 10.0;
    double y = 0.0, t = 0.0;

    std::cout << std::format("{:>12}, {:>12}, {:>12}, {:>12}\n", "t", "y_euler", "y_exact", "error");

    for(; t <= t_end + 1e-12; t +=dt) {
        const double y_exact = K * (1.0 - std::exp(-t / tau));

        // Data rows: numbers use {:12.6f} inside double quotes ""
        std::cout << std::format("{:>12.6f}, {:>12.6f}, {:>12.6f}, {:>12.6f}\n", t, y, y_exact, (y - y_exact));

        const double dydt = (-y + K * u) / tau;
        y += dt * dydt;
    }

    return 0;
}