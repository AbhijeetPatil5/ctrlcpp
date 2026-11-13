#ifndef EULER_HPP
#define EULER_HPP

double eulerStepThermal(double y, double u, double dt, double tau, double K);

double eulerStabilityLimit(double tau);

#endif