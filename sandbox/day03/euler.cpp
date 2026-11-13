#include "euler.hpp"

double eulerStepThermal(double y, double u, double dt, double tau, double K) {
    return y + dt * ((-y + K * u) / tau);
}

double eulerStabilityLimit(double tau) {
    return 2.0 * tau;
}