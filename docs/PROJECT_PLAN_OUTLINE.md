# Control Systems Library (csl) — Project Plan Outline

| Field | Value |
|---|---|
| Document | Project Plan Outline, Version 1.0 |
| Project | Control Systems Library (`csl`) |
| Derived from | Design Document V4 (`docs/DESIGN_V4.md`), which remains the detailed implementation reference |
| Companion | Project Plan Change Log (`docs/CHANGELOG_PROJECT_PLAN.md`) |
| Language standard | C++17 |
| Dependencies | Eigen 3.4 (linear algebra), Google Test 1.14 (unit testing) |
| Platforms | Windows 11 / MSVC 14.44 (primary); Linux / GCC; macOS / Clang |
| Date | 2026-11-01 |

---

## Contents

1. Purpose and Scope
2. Final Product
3. Architecture
4. Standards
5. Reference Models and Notation
6. Learning Map
7. Work Breakdown Structure
8. Function Inventory
9. Algorithm Specifications
10. Testing Strategy
11. Extensions
Appendix A — Glossary
Appendix B — References

**Figures:** 3-1 Repository layout · 3-2 Module dependency graph · 7-1 Stage sequence, core track and branches
**Tables:** 2-1 Library identity · 2-2 Deliverables · 2-3 Acceptance criteria · 3-1 Modules · 3-2 Semantic contracts · 3-3 Data flow · 3-4 Build-versus-buy · 4-1 Toolchain · 4-2 Coding standard · 4-3 Tolerance policy · 5-1 Notation · 5-2 Reference models · 5-3 Reference values · 6-1 C++ learning map · 6-2 Control and DSP learning map · 7-1 Stages · 8-1 to 8-11 Function inventory by module · 10-1 Test oracles · 11-1 Extensions

---

## 1. Purpose and Scope

### 1.1 The Project

The Control Systems Library (`csl`) is a C++17 library implementing the core of a control-systems and signal-processing toolbox: system representations and conversions, time- and frequency-domain analysis, classical and state-space controller design, discretization and digital filtering, optimal and predictive control, state estimation, and system identification. It is built from scratch by one developer as a structured learning project: every component is chosen so that implementing it teaches a C++ capability and a control or signal-processing concept at the same time.

This outline describes what the project builds, how it is structured, what is learned, and in what sequence the work is performed. It is a guide to the whole, divided into self-contained work packages; it is not a calendar. The Design Document V4 holds the full per-package derivations, listings and verification tables and is referenced where detail is needed.

### 1.2 C++ Objectives

On completion the developer can: compile and link multi-file programs by hand and through CMake; explain translation units, headers, include guards and the One Definition Rule; use value, reference and pointer semantics; distinguish stack and heap storage, apply RAII and smart pointers, and justify the absence of `new`/`delete` in the code base; design classes with constructors, invariants, `const` correctness and operator overloading; use inheritance, virtual dispatch and abstract base classes; use lambdas, `std::function`, templates, STL containers, iterators and algorithms, exceptions, `std::optional`, structured bindings, `enum class`, move semantics and the Rule of Zero; write Google Test suites; use a debugger, compiler warnings and sanitizers; package and install a library for an external consumer.

### 1.3 Control and Signal-Processing Objectives

On completion the developer can derive, implement and test: dynamic systems as ODEs and numerical integration; state-space, transfer-function and zero-pole-gain representations and their conversions; poles, zeros, stability, minimum phase and time delay; time-domain responses and specifications; frequency response, Bode and Nyquist analysis, stability margins and sensitivity functions; root locus and Routh–Hurwitz; PID, tuning rules, lead/lag and loop shaping; sampling, discretization and the z-domain; DFT/FFT, spectral estimation and FIR/IIR filter design; controllability, observability, pole placement, observers, Lyapunov equations and Gramians; LQR and Riccati equations; nonlinear systems and linearization; dynamic programming; Kalman, extended and unscented Kalman filters and LQG; ARX, recursive least squares, frequency-domain and subspace identification; uncertainty models, small-gain, H2/H∞ norms; quadratic programming and MPC; iterative LQR; H∞ synthesis.

### 1.4 Design Principles

- P1 **Delegate generic numerics.** Matrices, decompositions, matrix functions, complex arithmetic, elementary functions, random numbers, containers and algorithms come from Eigen, `std::complex`, `<cmath>`, `<random>` and the STL (Table 3-4).
- P2 **Hand-write domain algorithms.** Every algorithm that constitutes control theory or signal processing is implemented in the library.
- P3 **Admit a component only if it teaches.** A component exists only if implementing it teaches C++ or control.
- P4 **Sequential dependency.** Each work package uses only components built by its prerequisites and produces a component used later.
- P5 **Cohesion by contract.** One set of types and conventions (Section 3.4–3.5) is used by every module, so the output of any function is a valid input to any function accepting that type.
- P6 **Verification per concept.** A concept is complete when its implementation passes a test with a known answer (Chapter 10).
- P7 **Visualisation inside the library.** The library writes self-contained HTML/SVG plots and CSV files with no external dependency.
- P8 **Numerical honesty.** Every solver reports status, verifies its result and documents its limits (Section 3.8).

### 1.5 Out of Scope

Hardware interfaces, real-time scheduling, MIMO transfer-function matrices, and the extensions of Table 11-1, which are specified only by title and prerequisites.

### 1.6 Relationship to the Design Document

| Content | This outline | Design Document V4 |
|---|---|---|
| Architecture, contracts, standards | Complete | Complete |
| Work packages: scope, products, prerequisites, acceptance | Summary (Chapter 7) | Full: background, equations, listings, verification tables, exit criteria |
| Function inventory | Complete (Chapter 8) | Distributed across listings |
| Algorithms | Specifications (Chapter 9) | Derivations and C++ implementations |
| Reading | Roles and sources (Appendix B) | Citations per package |

---

## 2. Final Product

### 2.1 Library Identity

**Table 2-1 — Library identity**

| Property | Value |
|---|---|
| Namespace | `csl`, one sub-namespace per module |
| Build | CMake ≥ 3.20; one static library target `csl::csl`; `tests/`; `examples/` |
| Dependencies | Eigen 3.4 and Google Test 1.14, both fetched by CMake; nothing else |
| Scalar | `csl::Real` = `double` |
| Linear algebra types | `csl::Vector` = `Eigen::VectorXd`, `csl::Matrix` = `Eigen::MatrixXd`, complex counterparts |
| Ownership | Value semantics for numeric objects; `std::unique_ptr` for polymorphic objects owned by the simulator |
| Errors | Exceptions derived from `csl::Error`; absent-but-valid results as `std::optional` |
| Version | 2.0.0 at release |

### 2.2 Deliverables

**Table 2-2 — Deliverables**

| Deliverable | Content |
|---|---|
| Library | Eleven modules, about 52 headers with matching sources (Figure 3-1) |
| Test suite | Approximately 350–450 Google Test cases, one test file per header, run by CTest in Debug and Release on Windows and Linux |
| Examples | Ten programs `ex01`–`ex10`, one per stage, culminating in an end-to-end pipeline |
| Reference models | `examples/plants.hpp`: the five systems of Table 5-2, shared by examples and tests |
| Package | Installable CMake package (`find_package(csl)`), self-contained with vendored Eigen headers, verified by a consumer project |
| Documentation | README with gallery, documentation comment on every public function, five tutorials, this outline, the design document, the change log |
| Continuous integration | GitHub Actions: Windows and Linux builds, sanitizers, formatting check, install test |

### 2.3 Acceptance Criteria for the Final Product

**Table 2-3 — Acceptance criteria**

| ID | Criterion |
|---|---|
| F1 | Every function in Chapter 8 exists with the stated signature and a documentation comment stating units, conventions and exceptions |
| F2 | The test suite passes in Debug and Release on Windows (MSVC) and Linux (GCC with address and undefined-behaviour sanitizers), with zero compiler warnings at `/W4` or `-Wall -Wextra` |
| F3 | Every reference value of Table 5-3 is reproduced by a test to the tolerance of Table 4-3 |
| F4 | The cross-method identities of Section 10.1 (T4) hold: the stability limit `K = 6` of `1/(s(s+1)(s+2))` from margins, root locus and Routh; LQR cost versus simulated integral; Kalman covariance versus DARE; unconstrained MPC versus `dlqr`; iLQR versus finite-horizon LQR on a linear problem; H∞ controller versus LQG as `γ → ∞` |
| F5 | `ex10_full_pipeline` runs to completion and its identified models, designed controllers and estimators agree across methods within the tolerances of their work packages |
| F6 | A consumer project built against the installed package compiles and runs using only `find_package(csl)` and `csl::csl` |
| F7 | Continuous integration is green on the release tag |

---
## 3. Architecture

### 3.1 Modules

**Table 3-1 — Modules**

| Module | Namespace | Responsibility | Stage |
|---|---|---|---|
| core | `csl` | Type aliases, constants, exceptions, math utilities, polynomials, controllability, Lyapunov equations, Riccati equations, Jacobians, statistics, root finding, quadratic programming | 1 |
| systems | `csl::systems` | `LTISystem` base, `StateSpace`, `TransferFunction`, conversions, discretization, interconnections, `NonlinearSystem` | 1 |
| interfaces | `csl::control`, `csl::estimation`, `csl::dsp` | Abstract bases `Controller`, `Estimator`, `DigitalFilter` in `include/csl/interfaces/`; depends only on core and systems | 4 |
| simulation | `csl::simulation` | ODE integrators, `SimResult`, time responses and specifications, closed-loop `Simulator`, Monte Carlo | 1 |
| analysis | `csl::analysis` | Frequency response, Bode, Nyquist, margins, sensitivity, root locus, Routh–Hurwitz, Lyapunov stability, norms, robustness | 3 |
| control | `csl::control` | PID, tuning, lead/lag, transfer-function controller, state feedback, LQR, dynamic programming, LQG, MPC, iLQR, H∞ | 4 |
| estimation | `csl::estimation` | Luenberger observer, Kalman, extended and unscented Kalman filters | 6 |
| dsp | `csl::dsp` | FFT, windows, spectral estimation, filter structures and design | 5 |
| sysid | `csl::sysid` | ARX, RLS, frequency-domain identification, ERA, validation | 9 |
| signals | `csl::signals` | Deterministic test signals, white noise, PRBS | 2 |
| plot | `csl::plot` | CSV I/O, SVG/HTML figures, control-specific plot helpers | 1 |

### 3.2 Repository Layout

**Figure 3-1 — Repository layout at completion (annotated with the work package that creates each file)**

```
controlSystemCPPLibrary/
├── CMakeLists.txt                        WP 1.4; Eigen WP 1.6; tests WP 1.10; install WP 11.1
├── README.md / CHANGELOG.md              WP 11.2 / WP 11.3
├── .gitignore / .clang-format            WP 1.1 / WP 11.3
├── .github/workflows/ci.yml              WP 1.10 (minimal); WP 11.3 (full)
├── cmake/cslConfig.cmake.in              WP 11.1
├── docs/                                 design documents, this outline, change log, tutorials (WP 11.2), retrospective (WP 11.4)
├── sandbox/                              pre-library programs (WP 1.1–1.3), stage review notes
├── scripts/test_install.{ps1,sh}         WP 11.1
├── include/csl/
│   ├── csl.hpp                           WP 3.6   umbrella header
│   ├── core/
│   │   ├── Types.hpp                     WP 1.6
│   │   ├── Exceptions.hpp                WP 1.8
│   │   ├── MathUtils.hpp                 WP 3.1   dB, angles, linspace/logspace, unwrap; templates WP 5.6
│   │   ├── Polynomial.hpp                WP 2.1
│   │   ├── RootFinding.hpp               WP 4.5
│   │   ├── Controllability.hpp           WP 6.1
│   │   ├── Lyapunov.hpp                  WP 6.4
│   │   ├── Riccati.hpp                   WP 7.1   solveARE; solveDARE WP 7.2
│   │   ├── Jacobian.hpp                  WP 7.4
│   │   ├── Statistics.hpp                WP 8.1   sampleCovariance, discretizeNoise; unscentedTransform WP 8.5
│   │   └── QP.hpp                        WP 10.3
│   ├── systems/
│   │   ├── LTISystem.hpp                 WP 2.3
│   │   ├── StateSpace.hpp                WP 1.8
│   │   ├── TransferFunction.hpp          WP 2.2
│   │   ├── Conversions.hpp               WP 2.4   tf2ss, ss2tf; c2d/d2c WP 5.1–5.2
│   │   ├── Interconnect.hpp              WP 3.3
│   │   └── NonlinearSystem.hpp           WP 7.4
│   ├── interfaces/
│   │   ├── Controller.hpp                WP 4.1
│   │   ├── DigitalFilter.hpp             WP 5.5
│   │   └── Estimator.hpp                 WP 6.3
│   ├── simulation/
│   │   ├── SimResult.hpp                 WP 1.7
│   │   ├── ODESolver.hpp                 WP 1.4 (scalar); WP 1.7 (vector)
│   │   ├── TimeResponse.hpp              WP 2.6; specifications WP 2.7
│   │   ├── Simulator.hpp                 WP 4.2; estimator hook WP 6.3; nonlinear plant WP 7.4; noise WP 8.1
│   │   └── MonteCarlo.hpp                WP 8.1
│   ├── analysis/
│   │   ├── FrequencyResponse.hpp         WP 3.1; margins WP 3.2; sensitivity WP 4.6
│   │   ├── RootLocus.hpp                 WP 3.5
│   │   ├── Stability.hpp                 WP 3.4 (minimum phase); WP 3.6 (Routh); WP 6.4 (Lyapunov)
│   │   ├── Norms.hpp                     WP 6.4 (H2); WP 10.2 (H∞)
│   │   └── Robustness.hpp                WP 10.1
│   ├── control/
│   │   ├── PID.hpp                       WP 4.1; anti-windup WP 4.3
│   │   ├── Tuning.hpp, Relay.hpp         WP 4.4
│   │   ├── LeadLag.hpp                   WP 4.5
│   │   ├── TransferFunctionController.hpp WP 5.2
│   │   ├── StateFeedback.hpp             WP 6.2; integral action WP 6.5
│   │   ├── LQR.hpp                       WP 7.1–7.3
│   │   ├── DynamicProgramming.hpp        WP 7.5
│   │   ├── LQG.hpp                       WP 8.3
│   │   ├── MPC.hpp                       WP 10.4–10.5
│   │   ├── iLQR.hpp                      WP 10.6
│   │   └── HInfinity.hpp                 WP 10.7
│   ├── estimation/
│   │   ├── LuenbergerObserver.hpp        WP 6.3
│   │   ├── KalmanFilter.hpp              WP 8.2; steady state WP 8.3
│   │   ├── ExtendedKalmanFilter.hpp      WP 8.4
│   │   └── UnscentedKalmanFilter.hpp     WP 8.5
│   ├── dsp/
│   │   ├── FFT.hpp                       WP 5.3
│   │   ├── Spectral.hpp                  WP 5.4; etfe WP 9.3
│   │   ├── Filters.hpp                   WP 5.5
│   │   └── FilterDesign.hpp              WP 5.6
│   ├── sysid/
│   │   ├── ARX.hpp, Validation.hpp       WP 9.1; residual tests WP 9.4
│   │   ├── RLS.hpp                       WP 9.2
│   │   ├── FrequencyDomainID.hpp         WP 9.3
│   │   └── Subspace.hpp                  WP 9.4
│   ├── signals/Signals.hpp               WP 2.7; noise and PRBS WP 8.1
│   └── plot/
│       ├── CSV.hpp                       WP 1.5
│       ├── Figure.hpp                    WP 2.5
│       └── Plots.hpp                     WP 2.5; extended by stages 3 and 5
├── src/                                  one .cpp per header requiring one, mirroring include/csl
├── tests/                                tests/<module>/test_<Header>.cpp; tests/consumer/ (WP 11.1)
└── examples/
    ├── plants.hpp                        WP 1.6   reference models (Table 5-2)
    ├── ex01_first_order_sim.cpp          Stage 1
    ├── ex02_representations.cpp          Stage 3
    ├── ex03_classical_design.cpp         Stage 4
    ├── ex04_dsp_and_filters.cpp          Stage 5
    ├── ex05_state_space_design.cpp       Stage 6
    ├── ex06_lqr_pendulum.cpp             Stage 7
    ├── ex07_estimation.cpp               Stage 8
    ├── ex08_sysid.cpp                    Stage 9
    ├── ex09_mpc.cpp                      Stage 10
    └── ex10_full_pipeline.cpp            Stage 11
```

### 3.3 Dependencies

**Figure 3-2 — Module dependency graph (an arrow from X to Y: headers of X may include headers of Y; acyclic)**

```
                         ┌──────────┐
                         │   core   │
                         └────▲─────┘
                              │
                         ┌────┴─────┐
                         │ systems  │
                         └────▲─────┘
          ┌───────────┬───────┼────────────┬──────────────┐
    ┌─────┴─────┐ ┌───┴────┐  │     ┌──────┴─────┐  ┌─────┴─────┐
    │ interfaces│ │signals │  │     │    dsp     │  │ plot:     │
    └─────▲─────┘ └───▲────┘  │     └──────▲─────┘  │ CSV,Figure│
          │           │       │            │        └─────▲─────┘
    ┌─────┴───────────┴───────┴──┐         │              │
    │         simulation         │         │              │
    └─────▲──────────────────────┘         │              │
          │                                │              │
    ┌─────┴────────────────────────────────┴───┐          │
    │               analysis                   ├──────────┘   plot/Plots.hpp depends on analysis and simulation
    └─────▲───────────────▲──────────────▲─────┘
          │               │              │
    ┌─────┴─────┐   ┌─────┴──────┐  ┌────┴─────┐
    │ estimation│◀──│  control   │  │  sysid   │──▶ control::tuning::FOPDT
    └───────────┘   └────────────┘  └──────────┘
```

`simulation` includes `interfaces` (not `control` or `estimation`), so the simulator can own controllers and estimators without a cycle; `control` includes `simulation` (relay tuning experiment) and `estimation` (LQG); `estimation` includes `simulation` (EKF discretization).

### 3.4 Type Contract

**Listing 3-1 — `include/csl/core/Types.hpp`**

```cpp
#pragma once
#include <Eigen/Dense>
#include <complex>
#include <limits>
namespace csl {
using Real          = double;
using Index         = Eigen::Index;
using Vector        = Eigen::VectorXd;          // column vector
using RowVector     = Eigen::RowVectorXd;
using Matrix        = Eigen::MatrixXd;          // column-major
using Complex       = std::complex<Real>;
using ComplexVector = Eigen::VectorXcd;
using ComplexMatrix = Eigen::MatrixXcd;
inline constexpr Real PI     = 3.141592653589793238462643383279502884;
inline constexpr Real TWO_PI = 2.0 * PI;
inline constexpr Real EPS    = std::numeric_limits<Real>::epsilon();
inline constexpr Real INF    = std::numeric_limits<Real>::infinity();
}
```

Conventions:
- C1 All numeric code uses these aliases.
- C2 A sampled multi-channel signal is a `Matrix` with one row per time sample and one column per channel; a sampled scalar signal is a `Vector`.
- C3 Frequencies are angular (rad/s) and named `w`; parameters in hertz carry the suffix `_hz`.
- C4 `Polynomial` coefficients are stored in descending powers; the denominator of a `TransferFunction` is monic.

**Listing 3-2 — `LTISystem` interface**

```cpp
namespace csl::systems {
class LTISystem {
public:
    virtual ~LTISystem() = default;
    virtual Index order()   const = 0;
    virtual Index inputs()  const = 0;
    virtual Index outputs() const = 0;
    virtual Real  Ts()      const = 0;                       // 0: continuous; > 0: discrete with that sample time [s]
    virtual std::vector<Complex> poles() const = 0;
    virtual std::vector<Complex> zeros() const = 0;          // SISO transmission zeros
    virtual ComplexMatrix evaluate(Complex s) const = 0;     // H(s) or H(z), outputs x inputs
    virtual Matrix dcGain() const = 0;
    bool isContinuous() const { return Ts() == 0.0; }
    bool isDiscrete()   const { return Ts() >  0.0; }
    bool isSISO()       const { return inputs() == 1 && outputs() == 1; }
    bool isStable()     const;                               // continuous: Re p < 0; discrete: |p| < 1
};
class StateSpace       : public LTISystem { /* A, B, C, D, Ts; MIMO; n = 0 permitted (static gain) */ };
class TransferFunction : public LTISystem { /* num, den, Ts; SISO */ };
}
```

- C5 `Ts() == 0` denotes continuous time; `Ts() > 0` discrete time. There are no separate discrete-time classes; `c2d` and `d2c` change `Ts`.
- C6 `evaluate` is the single frequency-domain primitive, called at `s = jω` or `s = e^{jωT_s}`.
- C7 `StateSpace` is MIMO; `TransferFunction` is SISO; SISO-only functions test `isSISO()` and throw `NotSupported`.
- C8 Functions requiring matrices accept `const StateSpace&`; conversion from a transfer function is explicit (`tf2ss`, `toStateSpace`).

**Listing 3-3 — Result types**

```cpp
namespace csl::simulation {
struct SimResult {                 // produced by integrate, step, impulse, lsim, Simulator::run
    Vector t;  Matrix x, y, u;     // N; N x n; N x p; N x m
    Index  samples() const { return t.size(); }
    Real   sampleRate() const;     // 1/(t(1) - t(0)); throws for non-uniform t
    Vector channel(Index j) const; // y.col(j)
};
}
namespace csl::analysis {
struct FrequencyResponse {         // produced by frequencyResponse(sys, w) and by dsp::etfe(u, y, fs)
    Vector w;  ComplexVector H;  Real Ts = 0.0;
};
}
```

- C9 All time-domain results are `SimResult`; all SISO frequency-domain results are `FrequencyResponse`, whether computed from a model or estimated from data.

**Listing 3-4 — `Controller` interface**

```cpp
namespace csl::control {
enum class MeasurementKind { Output, State };
class Controller {
public:
    virtual ~Controller() = default;
    virtual Vector compute(const Vector& measurement, const Vector& reference, Real t) = 0;
    virtual void   reset() = 0;
    virtual Real   Ts() const = 0;
    virtual Index  inputs()  const = 0;        // size of measurement
    virtual Index  outputs() const = 0;        // size of the control vector
    virtual Index  referenceSize() const = 0;
    virtual MeasurementKind measurementKind() const = 0;
};
}
```

**Listing 3-5 — `Estimator` and `DigitalFilter` interfaces**

```cpp
namespace csl::estimation {
class Estimator {
public:
    virtual ~Estimator() = default;
    virtual void   predict(const Vector& u) = 0;
    virtual void   update (const Vector& y) = 0;
    virtual Vector state()      const = 0;     // x̂[k|k] after update(); x̂[k+1|k] after predict()
    virtual Matrix covariance() const = 0;     // zero for deterministic observers
    virtual void   reset(const Vector& x0) = 0;
    virtual Real   Ts() const = 0;
};
}
namespace csl::dsp {
class DigitalFilter {
public:
    virtual ~DigitalFilter() = default;
    virtual Real process(Real x) = 0;
    Vector       process(const Vector& x);     // derived classes re-expose with `using DigitalFilter::process;`
    virtual void reset() = 0;
    virtual Real Ts() const = 0;
    virtual systems::TransferFunction toTransferFunction() const = 0;   // discrete
    virtual std::unique_ptr<DigitalFilter> clone() const = 0;
};
}
```

- C10 A `Controller` operates on vectors even when SISO.
- C11 A controller declares through `measurementKind()` whether it receives the output or the state; the `Simulator` validates the combination of controller, estimator and plant before running.
- C12 A `DigitalFilter` is convertible to a discrete `TransferFunction` and is therefore analysed with the same functions as systems.

**Listing 3-6 — `include/csl/core/Exceptions.hpp`**

```cpp
#pragma once
#include <stdexcept>
namespace csl {
class Error             : public std::runtime_error { using std::runtime_error::runtime_error; };
class DimensionMismatch : public Error { using Error::Error; };
class InvalidArgument   : public Error { using Error::Error; };
class NumericalError    : public Error { using Error::Error; };
class NotSupported      : public Error { using Error::Error; };
}
```

- C13 Precondition violations throw one of these with a message naming the function and the offending values; absent-but-valid results are `std::optional`.

### 3.5 Semantic Contracts

**Table 3-2 — Semantic contracts**

| Contract | Rule | Enforced by |
|---|---|---|
| Measurement kind | `Controller::measurementKind()` declares `Output` or `State`; a `State`-kind controller requires an `Estimator` or a plant with `C = I` | `Simulator::run()` |
| Reference dimension | `Controller::referenceSize()` is the length of `reference`; the reference signal is validated against it | `Simulator::run()` |
| Estimator timing | `state()` is the posterior after `update()` and the prior after `predict()`; `covariance()` refers to the same quantity; `Estimator::Ts()` equals the controller's sample time; per tick: `update(y)` → `compute` → `predict(u)` | WP 6.3, 8.3 |
| Noise units | Continuous process noise is an intensity `Q_c`; discrete process noise is a covariance per sample `Q_d = discretizeNoise(A, Q_c, T_s)`; measurement noise `R` is a covariance per sample; noise is added once per controller period | WP 8.1 |
| Sampled signals | Data-consuming functions take `(const Vector& x, Real fs_hz)`; `SimResult::sampleRate()` and `channel(j)` extract them; non-uniform `t` is rejected | WP 5.4 |
| State coordinates | Gains and estimators are tied to the realization they were designed for; after `similarityTransform(sys, T)` the gain is `KT⁻¹` and the estimate `Tx̂`; no implicit realization changes | WP 2.4, 6.2 |
| Internal versus input–output stability | `StateSpace::isStable()` includes hidden modes; `TransferFunction::isStable()` tests poles after cancellation; `ss2tf` does not cancel, `minreal` does | WP 2.4 |
| Unknown stability from data | `Margins::closedLoopStable` is `std::optional<bool>`, absent for measured responses | WP 3.2 |
| Result-type consumers | Functions documented as taking `SimResult` or `FrequencyResponse` take the struct; single-channel consumers take `(Vector, fs)` | Table 3-3 |

### 3.6 Data Flow

**Table 3-3 — Data flow between modules**

| Type | Produced by | Consumed by |
|---|---|---|
| `Polynomial` | constructors, arithmetic, `fromRoots`, `charPoly` | `TransferFunction`, `routhArray`, `place`, Tustin substitution |
| `LTISystem` | constructors, `tf2ss`, `ss2tf`, `c2d`, `d2c`, `series`/`parallel`/`feedback`, `DigitalFilter::toTransferFunction`, `arx`, `era`, `fitRational`, `lqgCompensator`, `hinfsyn` | `analysis`, `simulation`, `control`, `estimation`, `plot` |
| `SimResult` | `integrate`, `step`, `impulse`, `lsim`, `Simulator::run` | `stepInfo`, `fitFOPDT`, `plotOutputs`, `welch` (via `channel`), `arx`, `markovParameters` |
| `FrequencyResponse` | `frequencyResponse`, `etfe` | `bode`, `nyquist`, `margins`, `plotBode`, `fitRational`, `relativeErrorEnvelope` |
| `Vector` signal | `signals::*`, `DigitalFilter::process`, `PRBS`, `WhiteNoise` | `lsim`, `Simulator`, `arx`, `etfe`, `welch`, filters |
| `ComplexVector` spectrum | `fft`, `rfft` | `periodogram`, `welch`, `csd` |
| Gain `Matrix K` | `place`, `lqr`, `dlqr`, `lqrIntegral` | `StateFeedbackController`, `IntegralStateFeedbackController`, `LQGController`, `iLQR` |
| Riccati `Matrix P` | `solveARE`, `solveDARE` | `lqr`, `dlqr`, `steadyStateKalman`, `MPC` terminal weight, `hinfnorm`, `hinfsyn` |
| `Estimator` | `LuenbergerObserver`, `KalmanFilter`, `ExtendedKalmanFilter`, `UnscentedKalmanFilter` | `Simulator`, `LQGController` |
| `QPResult` | `solveQP` | `MPC` |

### 3.7 Build-Versus-Buy Decisions

**Table 3-4 — Build-versus-buy decisions**

| Capability | Source | Rationale |
|---|---|---|
| Dense matrices, arithmetic, blocks, norms; LU, QR, SVD, Cholesky, Schur, eigenvalue decompositions; least squares; matrix exponential and logarithm | Eigen (incl. `unsupported/MatrixFunctions`) | Generic numerical infrastructure |
| Complex arithmetic; elementary functions; random engines and distributions; containers and algorithms | `std::complex`, `<cmath>`, `<random>`, STL | Standard |
| Unit testing | Google Test | Not a learning objective |
| Polynomials and companion-matrix roots; representations, conversions, interconnections, discretization; ODE integrators; frequency-domain analysis; root locus; Routh–Hurwitz; sensitivity | Library | Control theory |
| DFT/FFT, windows, PSD, ETFE; FIR/IIR structures; Butterworth, windowed FIR, notch design | Library | Signal processing is a stated objective |
| Controllability, observability, Lyapunov, Gramians, Riccati (CARE, DARE) | Library | Not in Eigen; the core of state-space control; SLICOT and SciPy serve as independent references |
| PID, tuning, lead/lag, pole placement, LQR, LQG, MPC, iLQR, H∞; observers and Kalman filters; ARX, RLS, ETFE fit, ERA | Library | Subject matter |
| QP solver (Hildreth's method with null-space elimination of equalities) | Library | The optimisation inside MPC; `solveQP` is the seam for an external solver |
| SVG/HTML plotting; CSV | Library | Dependency-free visualisation (P7) |
| Logging, registries, container wrappers | Not implemented | Principle P3 |

### 3.8 Numerical Policy

- N1 Solve, do not invert: linear systems are solved by factorization; `inverse()` only where the inverse is the result.
- N2 Report status: iterative solvers return a status in their result struct (converged, iteration limit, infeasible); factorization results are checked; `NumericalError` is thrown only when no result is meaningful.
- N3 Verify solutions: Riccati solvers check residual, symmetry and the stabilizing property; Lyapunov solvers check the residual; QP solvers report the KKT residual and maximum violation.
- N4 Scale: residuals are relative to input magnitudes; rank thresholds are relative to the largest singular value; physical models are scaled by Bryson-type weights.
- N5 Known sensitive cases are named with their robust alternative: high-degree polynomial roots, controllability matrices for `n > 10` (use Gramians), Hamiltonian eigenvectors with nearly coincident eigenvalues (ordered Schur), nearly uncontrollable pairs, high-order direct-form IIR filters (second-order sections).
- N6 Compare with references: SLICOT, SciPy and MATLAB implementations are independent oracles and the replacements of choice when an educational solver reaches its limits.

---
## 4. Standards

### 4.1 Toolchain

**Table 4-1 — Toolchain**

| Tool | Version | Role |
|---|---|---|
| Visual Studio 2022 Community, MSVC toolset | 14.44 | Compiler and linker (primary platform) |
| CMake | 4.3.3 | Build-system generator |
| Ninja | bundled with Visual Studio | Build driver |
| Visual Studio Code with *C/C++* and *CMake Tools* | 1.143 | Editor and debugger front end |
| Git | 2.54 | Version control |
| GCC ≥ 11 or Clang ≥ 14 | — | Alternative compilers (Linux, macOS, CI) |
| Python with NumPy/SciPy | 3.13 | Optional independent oracle; never a dependency |

### 4.2 Build Commands

**Listing 4-1 — Build, test and run (Developer PowerShell for VS 2022)**

```powershell
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug      # configure
cmake --build build                                         # build library, tests, examples
ctest --test-dir build --output-on-failure                  # run tests
.\build\examples\ex01_first_order_sim.exe                   # run an example
cmake -S . -B build-release -G Ninja -DCMAKE_BUILD_TYPE=Release && cmake --build build-release
cmake --install build --prefix <dir>                        # install (Stage 11)
```

With the Visual Studio generator, append `--config Debug` to `cmake --build` and `-C Debug` to `ctest`. Debugging: VS Code CMake Tools debug target plus `eigen.natvis` from `build/_deps/eigen-src/debug/msvc/`.

### 4.3 Coding Standard

**Table 4-2 — Coding standard**

| Item | Rule |
|---|---|
| Files | Headers `.hpp`, sources `.cpp`; `#pragma once`; includes from the include root (`"csl/systems/StateSpace.hpp"`) |
| Namespaces | `csl::<module>`; no `using namespace` in headers |
| Names | Types `PascalCase`; functions and variables `camelCase`; private members trailing underscore; namespace-scope constants `UPPER_CASE` |
| Parameters | Eigen and class objects by `const&`; results by value; in-place algorithms (`fft`) take non-`const` references and say so in their documentation |
| Prohibited | raw `new`/`delete`; C-style casts; global mutable state; `float`; `using namespace` in headers |
| Errors | Preconditions throw `csl::Error` subtypes; absent results use `std::optional`; solver outcomes use status enums |
| Documentation | A `///` comment on every public declaration: purpose, units and conventions, exceptions |
| Tests | `tests/<module>/test_<Header>.cpp`; `TEST(HeaderName, Property)`; floating-point comparisons with `EXPECT_NEAR`; test kind (correctness, numerical, experiment) in the test name |
| Formatting | `.clang-format`, 4-space indentation, 110 columns; warnings as errors on library and tests |
| Commits | One per completed work package, `WP n.m: <summary>`; one tag per stage |

### 4.4 Numerical Tolerance Policy

**Table 4-3 — Tolerance policy**

| Result class | Tolerance | Examples |
|---|---|---|
| Closed-form algebra | `1e-9` | polynomial products, `tf2ss` matrices, dc gain |
| Decomposition-based | `1e-8` | poles, Lyapunov solutions |
| Iterative solvers | `1e-6` | ARE, DARE, QP, RLS |
| Simulation versus analytic solution | stated per test | RK4 at `Δt = 0.01` versus `e^{−t}`: `1e-6` |
| Statistical | bound with fixed seed, stated per test | "RMSE < 0.45 with seed 7" |

Where an acceptance value is given as a closed form, the test evaluates the closed form in code; where it is given as a decimal, the decimal is correct to the digits shown and the tolerance exceeds the rounding.

### 4.5 Testing Policy

- T1 Every work package adds tests; the suite never shrinks.
- T2 Only the hand-written layer is tested; Eigen and the standard library are not.
- T3 Every test has a known answer: closed form, reference value (Table 5-3), cross-representation identity, invariant, convergence law, or independent computation.
- T4 Cross-method identities are tested deliberately (Table 2-3, F4).
- T5 Stochastic tests use fixed seeds and assert bounds with margin.
- T6 Tests are labelled *correctness*, *numerical* or *experiment*; experiments record values and assert an ordering only where theory guarantees it.
- T7 The suite runs in Debug and Release on Windows and Linux, with sanitizers on Linux.

### 4.6 Version Control

One commit per completed work package; one tag per stage (`stage-n`); release tag `v2.0.0`. Generated artefacts (`build/`, plots, CSV, object files, IDE state) are excluded by `.gitignore`.

---

## 5. Reference Models and Notation

### 5.1 Notation

**Table 5-1 — Notation**

| Symbol | Meaning | Code |
|---|---|---|
| `x ∈ ℝⁿ`, `u ∈ ℝᵐ`, `y ∈ ℝᵖ` | state, input, output | `Vector x, u, y` |
| `A, B, C, D` | state-space matrices (`n×n`, `n×m`, `p×n`, `p×m`) | `sys.A()` … |
| `T_s` | sample time [s]; `0` for continuous time | `Ts` |
| `s`, `z` | Laplace and z-transform variables | `Complex s` |
| `ω` | angular frequency [rad/s] | `w` |
| `H(s) = N(s)/D(s)` | transfer function | `num()`, `den()` |
| `p_i`, `z_i` | poles, zeros | `poles()`, `zeros()` |
| `ω_n`, `ζ`, `τ` | natural frequency, damping ratio, time constant | `wn`, `zeta`, `tau` |
| `L`, `S`, `T` | loop, sensitivity, complementary sensitivity | `L`, `S`, `T` |
| `K`, `L_o` | state-feedback gain, observer gain | `K`, `L` |
| `Q`, `R` | LQR weights; or noise covariances (context) | `Q`, `R` |
| `P` | Riccati solution; error covariance (context) | `P` |
| `‖G‖₂`, `‖G‖∞` | H2 and H∞ norms | `h2norm`, `hinfnorm` |
| `N`, `N_u` | prediction and control horizons | `N`, `Nu` |
| `λ` | eigenvalue; forgetting factor; dual variable (context) | `lambda` |

### 5.2 Reference Models

**Table 5-2 — Reference models (`examples/plants.hpp`)**

| ID | System | n / m / p | Parameters | Factory |
|---|---|---|---|---|
| E1 | First-order thermal system, `τẏ = −y + Ku` | 1 / 1 / 1 | `τ = 2 s`, `K = 3` | `plants::thermal()` |
| E2 | Mass–spring–damper, `mẍ + cẋ + kx = F`, position output | 2 / 1 / 1 | `m = 1`, `c = 0.5`, `k = 4` | `plants::massSpringDamper()` |
| E3 | DC motor, voltage to speed: `L di/dt = −Ri − K_bω + V`, `J dω/dt = K_t i − bω` | 2 / 1 / 1 | `R = 1`, `L = 0.5`, `J = 0.01`, `b = 0.1`, `K_t = K_b = 0.01` | `plants::dcMotor()` |
| E4 | Double integrator, `ẍ = u` | 2 / 1 / 1 | — | `plants::doubleIntegrator()` |
| E5 | Cart–pole, Eq. (5-1), outputs `[p, θ]` | 4 / 1 / 2 | `M = 1`, `m = 0.1`, `l = 0.5`, `g = 9.81` | `plants::cartPole()` |
| E6 | Pendulum, `θ̈ = −(g/l) sin θ − cθ̇ + u`, output `sin θ` | 2 / 1 / 1 | `l = 1`, `c = 0.1` | `plants::pendulum()` |

Cart–pole equations of motion (state `[p, ṗ, θ, θ̇]`, `θ = 0` upright, force `F` on the cart):

```
p̈ = [F + m l θ̇² sin θ − m g sin θ cos θ] / [M + m sin²θ]
θ̈ = [−F cos θ − m l θ̇² sin θ cos θ + (M + m) g sin θ] / [l (M + m sin²θ)]                              (5-1)
```

State-space forms (states E1 `[y]`, E2 `[x, ẋ]`, E3 `[i, ω]`, E4 `[x, ẋ]`):

```
E1: A = [−1/τ], B = [K/τ], C = [1], D = [0]
E2: A = [[0, 1], [−k/m, −c/m]], B = [[0], [1/m]], C = [1, 0], D = [0]
E3: A = [[−R/L, −K_b/L], [K_t/J, −b/J]], B = [[1/L], [0]], C = [0, 1], D = [0]
E4: A = [[0, 1], [0, 0]], B = [[0], [1]], C = [1, 0], D = [0]
E5 (upright linearization): A = [[0,1,0,0], [0,0,−mg/M,0], [0,0,0,1], [0,0,(M+m)g/(Ml),0]], B = [0, 1/M, 0, −1/(Ml)]ᵀ
```

### 5.3 Reference Values

**Table 5-3 — Reference values used in acceptance tests**

| Model or loop | Quantity | Value |
|---|---|---|
| E1 | pole; dc gain; step response at `t = τ`; 10–90 % rise time; 2 % settling time | `−0.5`; `3`; `3(1 − e^{−1}) = 1.8963617`; `τ ln 9 = 4.394 s`; `τ ln 50 = 7.824 s` |
| E1, `T_s = 0.1` | `A_d`; `B_d` | `e^{−0.05} = 0.9512294245`; `3(1 − e^{−0.05}) = 0.1463117265` |
| E2 | poles; `ω_n`; `ζ`; dc gain; overshoot; peak time; peak value; resonance peak; H2 norm | `−0.25 ± 1.9843j`; `2`; `0.125`; `0.25`; `67.3 %`; `1.583 s`; `0.418`; `1.0079` at `1.968 rad/s`; `0.5` |
| E3 | poles; dc gain; transfer function | `−2.0025`, `−9.9975`; `2/20.02 = 0.0999`; `2/(s² + 12s + 20.02)` |
| E4, LQR `Q = I`, `R = 1` | `P`; `K`; closed-loop poles | `[[√3, 1], [1, √3]]`; `[1, √3]`; `−0.866 ± 0.5j` |
| E5 | open-loop poles | `0, 0, ±4.646` |
| `L = 1/(s(s+1)(s+2))` | gain margin; phase crossover; phase margin; gain crossover; delay margin; `M_s`; stability limit of `K` | `6 (15.56 dB)`; `1.414`; `53.4°`; `0.446`; `2.09 s`; `1.57` at `0.73 rad/s`; `K < 6` |
| `L = 10/(s(s+1))` | phase margin; gain crossover; closed-loop bandwidth | `18.0°`; `3.08`; `4.83 rad/s` |
| `1/(s+1)`, Tustin `T_s = 0.1` | discrete transfer function | `(z/21 + 1/21)/(z − 19/21)` |
| Butterworth 4th order, `f_c = 100`, `f_s = 1000` | gain at 100 Hz; at 200 Hz | `−3.01 dB`; `−27.97 dB` |
| Hildreth test QP | `min (x₁−1)² + (x₂−2)²` s.t. `x₁ + x₂ ≤ 2` | `(0.5, 1.5)`, `λ = 1` |

---

## 6. Learning Map

### 6.1 C++ Topics

**Table 6-1 — C++ learning map (in order of first use)**

| Topic | First applied in | Applied again in |
|---|---|---|
| Preprocessor, compiler, linker; object files | WP 1.1 | WP 1.3, 1.4 |
| Fundamental types, `const`, loops, `<cmath>`, formatted output | WP 1.2 | throughout |
| Declarations versus definitions, headers, `#pragma once`, One Definition Rule | WP 1.3 | WP 3.6 (umbrella header) |
| CMake targets, scopes, build types, static libraries | WP 1.4 | WP 1.6, 1.10, 11.1 |
| `std::vector`, range-for, references, `std::string`, `std::ofstream` | WP 1.5 | WP 2.5 |
| Namespaces, type aliases, `inline constexpr`, Eigen basics, overloading, `FetchContent` | WP 1.6 | throughout |
| Lambdas, `std::function`, `auto`, `struct`, `enum class` | WP 1.7 | WP 4.2, 7.4 |
| Classes, constructors, invariants, `const` members, exceptions, anonymous namespaces | WP 1.8 | all classes |
| `std::complex`, `Eigen::EigenSolver`, iterator-range conversion | WP 1.9 | WP 2.2, 7.1 |
| Google Test, CTest, debugger, `assert`, minimal CI | WP 1.10 | every package |
| Operator overloading, `std::initializer_list`, call operator, `friend`, static factories | WP 2.1 | WP 2.2, 3.3 |
| Composition, `std::ostringstream`, `std::sort` with comparators | WP 2.2 | WP 3.5 |
| Inheritance, virtual and pure virtual functions, `override`, virtual destructors, polymorphic references | WP 2.3 | WP 4.1, 5.5, 6.3 |
| Eigen block operations, numerical identities as assertions | WP 2.4 | WP 3.3, 6.1 |
| Class API design with method chaining, SVG generation, layering | WP 2.5 | — |
| Iterators, `<algorithm>`, reverse iterators | WP 2.7 | WP 3.5 |
| Inline header utilities; `std::optional`; overload sets across types | WP 3.1–3.3 | WP 4.4, 8.4 |
| Header hygiene, `inline` versus out-of-line | WP 3.6 | — |
| Configuration structs with default member initialisers, `std::clamp`, stateful objects | WP 4.1 | WP 6.5, 10.4 |
| Stack versus heap, RAII, `std::unique_ptr`, `std::make_unique`, move-only types, incomplete-type rules | WP 4.2 | WP 6.3 |
| Strategy selection by `enum class` and `switch`; refactoring under test | WP 4.3 | WP 5.1 |
| `std::pair`, `std::tuple`, structured bindings | WP 4.4 | — |
| Numerical root finding over `std::function`; aggregate initialisation | WP 4.5 | WP 10.2 |
| Eigen matrix functions (`exp`, `log`) | WP 5.1 | WP 8.1 |
| Polynomial composition; generic adapters | WP 5.2 | — |
| Bitwise operators, `std::size_t`, in-place algorithms, `std::chrono` | WP 5.3 | — |
| Name hiding and `using` declarations; `final`; `clone` | WP 5.5 | — |
| Function templates, `Eigen::MatrixBase<Derived>` parameters | WP 5.6 | — |
| Numerical rank, SVD thresholds | WP 6.1 | WP 9.4 |
| Horner evaluation at a matrix; solving instead of inverting | WP 6.2 | WP 7.1 |
| Kronecker products, `Eigen::Map`, Cholesky as definiteness test | WP 6.4 | WP 8.5 |
| Special member functions, Rule of Zero, deleted copies and defaulted moves, `noexcept`, RVO, benchmarking | WP 6.6 | — |
| Block matrices with `<<`, complex eigenvectors, residual self-checks | WP 7.1 | WP 10.2, 10.7 |
| Backward and convergence loops over `std::vector<Matrix>` | WP 7.2 | WP 10.6 |
| Numerical differentiation, Newton's method, class refactoring without behaviour change | WP 7.4 | WP 8.4 |
| Flattened multi-dimensional grids, interpolation | WP 7.5 | — |
| `<random>` engines and distributions, seeding, factory lambdas | WP 8.1 | WP 8.2–8.5 |
| Joseph-form updates, diagnostic accessors, statistical tests | WP 8.2 | WP 8.4 |
| Optional callbacks (`std::optional<std::function>`) | WP 8.4 | — |
| Least squares by QR, condition numbers | WP 9.1 | WP 9.3 |
| Weighted complex least squares by real stacking | WP 9.3 | — |
| Hankel matrices, `Eigen::BDCSVD` | WP 9.4 | — |
| Bisection on a matrix-spectrum property | WP 10.2 | WP 10.7 |
| Solver status enums, warm starts, null-space elimination | WP 10.3 | WP 10.4 |
| Block-Toeplitz assembly; slack variables | WP 10.4–10.5 | — |
| Line search, regularisation, index bookkeeping in large algorithms | WP 10.6 | WP 10.7 |
| CMake install/export, package config, consumer test | WP 11.1 | — |
| CI pipelines, sanitizers, formatting enforcement | WP 11.3 | — |

### 6.2 Control and Signal-Processing Topics

**Table 6-2 — Control and DSP learning map (in order of first use)**

| Topic | First applied in | Applied again in |
|---|---|---|
| Dynamic systems; state, input, output; open and closed loop | WP 1.1 | throughout |
| First-order response, time constant; Euler integration, order and stability of integrators | WP 1.2–1.3 | WP 1.7 |
| Second-order response, `ω_n`, `ζ`, damping regimes | WP 1.4–1.5 | WP 2.7 |
| State-space form | WP 1.6 | throughout |
| Runge–Kutta integration, step-size selection | WP 1.7 | WP 2.6, 4.2 |
| Dimensions, feedthrough, dc gain | WP 1.8 | WP 2.2 |
| Eigenvalues as poles; modes; stability; dominant poles | WP 1.9 | WP 2.2, 5.1 |
| Laplace transform, transfer functions, characteristic polynomial | WP 2.1 | — |
| Poles, zeros, properness, zero-pole-gain form, cancellation | WP 2.2 | WP 3.4 |
| `H(s) = C(sI−A)⁻¹B + D`; transmission zeros | WP 2.3 | WP 3.1 |
| Realizations, canonical forms, Faddeev–LeVerrier, minimality, internal versus I/O stability | WP 2.4 | WP 6.1 |
| Step, impulse, arbitrary-input responses; zero-order hold | WP 2.6 | WP 5.1 |
| Time-domain specifications and second-order formulas; test signals | WP 2.7 | WP 6.2 |
| Sinusoidal steady state, Bode plot, asymptotes, phase unwrapping | WP 3.1 | WP 5.5 |
| Nyquist criterion, gain/phase/delay margins | WP 3.2 | WP 4.4, 9.3 |
| Interconnections, closed-loop poles, gang of four | WP 3.3 | WP 4.6 |
| Minimum phase, RHP zeros, all-pass, delay and Padé approximation | WP 3.4 | WP 10.1 |
| Root locus and sketching rules | WP 3.5 | — |
| Routh–Hurwitz | WP 3.6 | — |
| PID law, filtered derivative, implementable form | WP 4.1 | WP 5.7 |
| Sampled-data loop, hold delay, sample-time selection | WP 4.2 | WP 5.1, 5.7 |
| Windup, clamping, back-calculation, setpoint weighting | WP 4.3 | — |
| FOPDT fitting, Ziegler–Nichols, SIMC, relay auto-tuning | WP 4.4 | WP 9.3 |
| Error constants, system type, lead and lag design | WP 4.5 | — |
| Loop shaping, bandwidth, peak sensitivity, Bode's integral | WP 4.6 | WP 10.1 |
| Exact ZOH model, pole mapping, aliasing, Nyquist frequency | WP 5.1 | WP 5.7 |
| z-transform, difference equations, Tustin, prewarping, matched pole–zero | WP 5.2 | WP 5.6 |
| DFT, FFT, bins, resolution, Parseval | WP 5.3 | WP 5.4, 9.3 |
| Leakage, windows, periodogram, Welch, cross-spectra | WP 5.4 | WP 9.3 |
| IIR and FIR structures, DF2T, linear phase, zero-phase filtering | WP 5.5 | — |
| Butterworth, windowed-sinc FIR, notch design | WP 5.6 | WP 5.7 |
| Emulation versus direct design, dead-beat, anti-aliasing | WP 5.7 | — |
| Controllability, observability, PBH, stabilizability, detectability | WP 6.1 | WP 7.1 |
| Pole placement, Ackermann, feedforward gain | WP 6.2 | WP 6.5 |
| Observers, separation principle, observer-based compensator | WP 6.3 | WP 8.3 |
| Lyapunov equations and stability, Gramians, H2 norm | WP 6.4 | WP 10.2 |
| Feedforward `(N_x, N_u)`, integral augmentation | WP 6.5 | WP 7.3 |
| LQR, CARE, Hamiltonian method | WP 7.1 | WP 8.3, 10.7 |
| Finite-horizon LQ, Riccati recursion, DARE | WP 7.2 | WP 10.4 |
| Bryson's rule, return-difference inequality, guaranteed margins | WP 7.3 | WP 8.3 |
| Equilibria, linearization, region of attraction | WP 7.4 | WP 8.4, 10.6 |
| Bellman equation, value iteration | WP 7.5 | WP 10.5 |
| White noise, covariance, noise discretization, PRBS, Monte Carlo | WP 8.1 | WP 9.1 |
| Kalman filter, innovation, consistency (NIS) | WP 8.2 | WP 8.4 |
| Steady-state Kalman filter, duality, LQG, loss of margins | WP 8.3 | WP 10.7 |
| Extended and unscented Kalman filters | WP 8.4–8.5 | WP 10.5 |
| ARX least squares, persistency of excitation, bias | WP 9.1 | WP 9.4 |
| Recursive least squares, forgetting | WP 9.2 | — |
| ETFE, coherence, rational fitting, margins from data | WP 9.3 | WP 10.1 |
| Markov parameters, ERA, residual validation | WP 9.4 | — |
| Multiplicative uncertainty, small-gain theorem, robust performance | WP 10.1 | WP 10.7 |
| H∞ norm, bounded-real lemma | WP 10.2 | WP 10.7 |
| QP, KKT conditions, Hildreth's method | WP 10.3 | WP 10.4 |
| Receding horizon, condensed MPC, terminal cost, soft constraints | WP 10.4–10.5 | — |
| Iterative LQR, trajectory optimisation | WP 10.6 | — |
| Generalized plant, DGKF central controller, γ-iteration | WP 10.7 | — |

---
## 7. Work Breakdown Structure

### 7.1 Stages

The project is divided into eleven stages, each producing a coherent increment of the library and ending with an example program and a stage review (Table 7-1). Stages are divided into work packages (WP), each self-contained: it has a defined scope, named prerequisites, named products and acceptance checks, and it is complete when its tests pass and its products are committed. A work package is sized for one working session of 3–5 hours; packages marked (×2) or (×3) are larger and are divided at the point indicated.

**Table 7-1 — Stages**

| Stage | Title | Packages | Increment | Example |
|---|---|---|---|---|
| 1 | Foundations | 1.1–1.10 | Toolchain, build system, integrators, `StateSpace`, tests, CI | `ex01` |
| 2 | Representations and time domain | 2.1–2.7 | `Polynomial`, `TransferFunction`, `LTISystem`, conversions, plotter, time responses, specifications, signals | — |
| 3 | Frequency domain | 3.1–3.6 | Frequency response, margins, interconnections, delay, root locus, Routh, umbrella header | `ex02` |
| 4 | Classical design | 4.1–4.6 | `Controller`, PID, `Simulator`, tuning, lead/lag, loop shaping | `ex03` |
| 5 | Discrete time and DSP | 5.1–5.8 | Discretization, z-domain controller, FFT, spectra, filters, digital control | `ex04` |
| 6 | State-space design | 6.1–6.7 | Controllability, pole placement, observers, Lyapunov, integral action, copy/move audit | `ex05` |
| 7 | Optimal control | 7.1–7.6 | CARE/DARE, LQR, LQR properties, nonlinear systems, dynamic programming | `ex06` |
| 8 | Estimation | 8.1–8.6 | Noise, Kalman filter, LQG, EKF, UKF, Monte Carlo | `ex07` |
| 9 | Identification | 9.1–9.4 | ARX, RLS, frequency-domain identification, ERA, validation | `ex08` |
| 10 | Robust, predictive and nonlinear optimal control | 10.1–10.7 | Uncertainty, H∞ norm, QP, MPC, iLQR, H∞ synthesis | `ex09` |
| 11 | Release | 11.1–11.4 | Install/export, documentation, CI, full pipeline | `ex10` |

### 7.2 Sequence, Core Track and Branches

**Figure 7-1 — Stage sequence, core track and branches**

```
  CORE TRACK (strict order)
  Stage 1 ─▶ Stage 2 ─▶ Stage 3 ─▶ Stage 4 ─▶ WP 5.1, 5.2, 5.7 ─▶ Stage 6 ─▶ WP 7.1–7.3 ─▶ WP 8.1–8.3 ─▶ WP 11.1
     │            │            │            │                                  │              │
     ▼            ▼            ▼            ▼                                  ▼              ▼
  BRANCHES (any order after their prerequisites; any may be deferred)
  A  DSP           WP 5.3–5.6, 5.8      requires core through WP 5.2
  B  Nonlinear     WP 7.4–7.6, 8.4–8.6  requires WP 7.1–7.3 (8.4–8.6 also 8.1–8.3)
  C  Identification WP 9.1–9.4          requires core; WP 9.3 requires branch A
  D  Predictive    WP 7.5, 10.3–10.6    requires WP 7.1–7.3; WP 10.6 requires branch B
  E  Robust        WP 10.1, 10.2, 10.7  requires WP 7.1, 8.3; WP 10.1 uses branch C for data-derived weights
  RELEASE          WP 11.2–11.4         after the core and the completed branches
```

### 7.3 Work Package Format

Each package below states: **Scope** (what is built and why it belongs here); **C++** and **Control** (what is learned, keyed to Tables 6-1 and 6-2); **Produces** (files and functions; signatures are in Chapter 8); **Prerequisites**; **Acceptance** (the known answers the tests must reproduce). The generic completion standard for every package is: tests pass in Debug and Release; every new public function has a documentation comment; the package's central equation or algorithm has been reproduced on paper; the failure case named in Acceptance is explained; the work is committed.

### 7.4 Stage 1 — Foundations

| WP 1.1 — Toolchain and compilation pipeline | |
|---|---|
| Scope | Install and verify the toolchain (Table 4-1); compile a one-file program in one step and in explicit compile and link steps; create `.gitignore` |
| C++ | Preprocessor, compiler, linker; object files; the structure of `main` |
| Control | Vocabulary: system, state, input, output, open and closed loop; the reference models of Table 5-2 |
| Produces | `sandbox/day01/hello.cpp`, `.gitignore` |
| Prerequisites | none |
| Acceptance | Program runs; a deliberate `main` misspelling fails at the *link* stage, not at compilation |

| WP 1.2 — First simulation: Euler on E1 | |
|---|---|
| Scope | Simulate `τẏ = −y + Ku` with Euler's method; compare with the exact response; observe accuracy and stability versus step size |
| C++ | Types, initialisation, `const`, loops, `<cmath>`, `<iomanip>` |
| Control | Exact first-order step response; Euler's method, first-order accuracy, stability limit `Δt < 2τ` |
| Produces | `sandbox/day02/thermal_euler.cpp` |
| Prerequisites | WP 1.1 |
| Acceptance | `y(2)`: `1.9245` at `Δt = 0.1`, `1.8991` at `Δt = 0.01` (exact `1.8964`); divergence at `Δt = 5` |

| WP 1.3 — Functions, headers and linking | |
|---|---|
| Scope | Split the simulation into header, source and main; build with explicit compile and link commands; provoke and interpret three error classes |
| C++ | Declaration versus definition, headers, `#pragma once`, translation units, One Definition Rule |
| Control | Euler stability region `|1 + λΔt| < 1` |
| Produces | `sandbox/day03/{euler.hpp, euler.cpp, main.cpp}` |
| Prerequisites | WP 1.2 |
| Acceptance | Output identical to WP 1.2; unresolved external (link), identifier not found (compile), symbol already defined (link) each reproduced |

| WP 1.4 — CMake and the library skeleton | |
|---|---|
| Scope | Create the root `CMakeLists.txt` with the static library `csl`, the `examples/` subdirectory and warning flags; simulate E2 with two scalar states |
| C++ | CMake targets, `PUBLIC`/`PRIVATE`/`INTERFACE`, out-of-source builds, build types, generator expressions |
| Control | Second-order system as two first-order equations; evaluating all derivatives from the current state |
| Produces | `CMakeLists.txt`, `examples/CMakeLists.txt`, `simulation/ODESolver.hpp` (scalar Euler, temporary), `examples/ex01_first_order_sim.cpp` |
| Prerequisites | WP 1.3 |
| Acceptance | Build succeeds; E2 settles to `F/k = 0.25` with period `3.17 s` and first peak `0.418` at `1.58 s` |

| WP 1.5 — Containers, references, CSV output | |
|---|---|
| Scope | Replace scalar bookkeeping with `std::vector`; write CSV files; sweep the damping ratio |
| C++ | `std::vector`, range-for, references and `const&`, `std::string`, `std::ofstream`, `std::max_element` |
| Control | Underdamped, critically damped and overdamped regimes; overshoot formula |
| Produces | `plot/CSV.hpp` (`writeCSV`, vector overload) |
| Prerequisites | WP 1.4 |
| Acceptance | Overshoot `67.2 %`, `16.3 %`, `0`, `0` for `ζ = 0.125, 0.5, 1, 2` |

| WP 1.6 — Eigen, namespaces and the state-space form | |
|---|---|
| Scope | Fetch Eigen through CMake as an interface target with build and install include paths; define the type aliases; write the reference-model factories; simulate E2 as `ẋ = Ax + Bu` |
| C++ | Namespaces, aliases, `inline constexpr`, Eigen construction and arithmetic, function overloading, `FetchContent` |
| Control | State-space form; matrices of E1–E4 |
| Produces | `core/Types.hpp` (Listing 3-1), `examples/plants.hpp`, `writeCSV` matrix overload |
| Prerequisites | WP 1.5 |
| Acceptance | CSV identical to WP 1.4; E3 dc gain `−CA⁻¹B = 0.0999` |

| WP 1.7 — Lambdas and the generic integrator | |
|---|---|
| Scope | Replace the scalar Euler with `eulerStep`, `rk4Step`, `integrate` over `std::function` dynamics; define `SimResult` |
| C++ | Lambdas and captures, `std::function`, `auto`, plain-data `struct`, `enum class` as selector |
| Control | RK4, fourth-order accuracy, step-size rule |
| Produces | `simulation/SimResult.hpp`, `simulation/ODESolver.hpp` (final) |
| Prerequisites | WP 1.6 |
| Acceptance | E1 error at `t = 10`: Euler `+1.1e-2, +2.4e-3, +2.5e-4` and RK4 `−4e-6, −5e-9, −5e-13` for `Δt = 0.5, 0.1, 0.01`; `integrate(0, 1, 0.25)` has 5 samples |

| WP 1.8 — Classes, exceptions and `StateSpace` | |
|---|---|
| Scope | Implement `StateSpace` with a validating constructor, accessors, `derivative`, `output`, `dcGain`; define the exception hierarchy; convert the factories to return `StateSpace` |
| C++ | Class definition, member-initialiser lists, invariants, `const` members, `explicit`, exceptions, anonymous namespaces |
| Control | Dimensions, feedthrough, dc gain `D − CA⁻¹B` and `D + C(I − A)⁻¹B` |
| Produces | `core/Exceptions.hpp` (Listing 3-6), `systems/StateSpace.hpp` |
| Prerequisites | WP 1.7 |
| Acceptance | E3 dc gain `2/20.02` to `1e-12`; E2 dc gain `0.25`; a `3×2` `A` throws `DimensionMismatch`; `n = 0` accepted |

| WP 1.9 — Complex numbers, poles and stability | |
|---|---|
| Scope | Compute poles as eigenvalues of `A`; classify stability for continuous and discrete systems |
| C++ | `std::complex`, `Eigen::EigenSolver`, Eigen-to-`std::vector` by iterator range |
| Control | Modes, `ω_n`/`ζ`/`τ` from pole location, dominant poles, marginal stability |
| Produces | `StateSpace::poles`, `StateSpace::isStable` (moved to `LTISystem` in WP 2.3) |
| Prerequisites | WP 1.8 |
| Acceptance | Poles of E1–E4 as Table 5-3; E4 not stable; `[[0,1],[−4,0]]` gives `±2j`, not stable, sustained oscillation of period `π` |

| WP 1.10 — Tests, debugger, minimal CI, stage review | |
|---|---|
| Scope | Add Google Test through CMake; write the first test files; set up the debugger; create a minimal CI workflow; write the stage review |
| C++ | `TEST`, `EXPECT_NEAR`, `EXPECT_THROW`, CTest, breakpoints, `assert`, warnings as errors, GitHub Actions basics |
| Control | — (consolidation) |
| Produces | `tests/CMakeLists.txt`, `tests/{systems,simulation,plot}/test_*.cpp`, `.github/workflows/ci.yml`, `examples/ex01` final, `sandbox/phase1_review.md` |
| Prerequisites | WP 1.9 |
| Acceptance | At least 8 tests pass; RK4 error ratio between `Δt = 0.1` and `0.05` exceeds 12; Euler ratio `2.0 ± 0.2`; CI green |

### 7.5 Stage 2 — Representations and Time Domain

| WP 2.1 — `Polynomial` | |
|---|---|
| Scope | Polynomial class with descending coefficients, Horner evaluation, arithmetic operators, `fromRoots`, derivative, trimming |
| C++ | Operator overloading (free and member), `std::initializer_list`, call operator, `friend`, static factories, approximate comparison |
| Control | Laplace transform, transfer function as a ratio of polynomials, characteristic polynomial |
| Produces | `core/Polynomial.hpp` |
| Prerequisites | WP 1.10 |
| Acceptance | `{1,1}·{1,2} = {1,3,2}`; `{1,3,2}(−1) = 0`; `fromRoots({−1±2j}) = {1,2,5}`; `{0,0,1,3}` has degree 1 |

| WP 2.2 — `TransferFunction` and companion-matrix roots | |
|---|---|
| Scope | Roots through the companion matrix; `TransferFunction` with monic normalisation, poles, zeros, properness, dc gain, `fromZPK`, `minreal`, printing |
| C++ | Composition, reuse of `EigenSolver`, `std::ostringstream`, `std::sort` with comparators |
| Control | Poles and zeros, relative degree, zero-pole-gain form, cancellation |
| Produces | `Polynomial::roots`, `systems/TransferFunction.hpp`, `plants::thermalTF`, `plants::massSpringDamperTF` |
| Prerequisites | WP 2.1 |
| Acceptance | `{1}/{1,3,2}` poles `{−2,−1}`; E1 TF stored as `{1.5}/{1,0.5}` with dc gain 3; `{1,1}/{1,3,2}.minreal()` has order 1; `{1}/{0}` throws |

| WP 2.3 — `LTISystem` and polymorphism | |
|---|---|
| Scope | Abstract base; `StateSpace::evaluate` by complex solve; SISO transmission zeros by the generalized eigenproblem; `damp`; `isStable` in the base |
| C++ | Inheritance, pure virtual functions, `override`, virtual destructors, polymorphic references, slicing |
| Control | `H(s) = C(sI−A)⁻¹B + D`; transmission zeros; pole summary |
| Produces | `systems/LTISystem.hpp` (Listing 3-2), `damp` |
| Prerequisites | WP 2.2 |
| Acceptance | E2 `ss.evaluate(s)` equals `tf.evaluate(s)` at `1 + 2j` and `0.3j` to `1e-9`; zeros of the canonical form of `(s+3)/(s²+3s+2)` are `{−3}`; `damp(E2)` gives `ω_n = 2`, `ζ = 0.125` |

| WP 2.4 — Realizations: `tf2ss`, `ss2tf`, similarity transforms | |
|---|---|
| Scope | Controllable canonical form including the static-gain case; Faddeev–LeVerrier `ss2tf` with the Cayley–Hamilton assertion; `charPoly`; `similarityTransform`; `toStateSpace` |
| C++ | Eigen block operations, assertions of numerical identities |
| Control | Non-uniqueness of realizations, minimality, internal versus input–output stability |
| Produces | `systems/Conversions.hpp` (conversion part) |
| Prerequisites | WP 2.3 |
| Acceptance | `ss2tf(E2) = {1}/{1,0.5,4}`, `ss2tf(E3) = {2}/{1,12,20.02}`; `ss2tf(tf2ss(H)) = H` for proper and strictly proper `H`; static gain `2.5` round-trips with order 0; similarity transform preserves poles and `evaluate` |

| WP 2.5 — Plotter (×2, part A) | |
|---|---|
| Scope | `Figure`: line and scatter series, guides, labels, logarithmic axes, limits, SVG rendering, HTML pages; control-independent |
| C++ | Method chaining, `std::ostringstream`, SVG, coordinate mapping, nice tick computation, layering |
| Control | — |
| Produces | `plot/Figure.hpp`, `plot/Plots.hpp` (`plotOutputs`, `plotStates`) |
| Prerequisites | WP 2.4 |
| Acceptance | Two-series SVG contains two `<polyline` elements and the title; a page renders with axes, grid and legend |

| WP 2.6 — Time responses (×2, part B) | |
|---|---|
| Scope | `lsim` with zero-order hold (RK4 within intervals; exact recursion for discrete systems); `step`; `impulse` with the continuous (`x₀ = B`) and discrete (`u[0] = 1`) conventions; automatic horizon |
| C++ | Default arguments, `const` member functions, result structs |
| Control | Step, impulse and arbitrary-input responses; hold between samples; Markov parameters |
| Produces | `simulation/TimeResponse.hpp` (`step`, `impulse`, `lsim`, `autoHorizon`) |
| Prerequisites | WP 2.5 |
| Acceptance | `step(E1)` at `t = 2` is `3(1 − e⁻¹)`; `step(E2)` peaks at `0.418` near `1.58 s`; discrete impulse of `c2d(E1, 0.1)` gives `0, B_d, A_dB_d` (checked when `c2d` exists); impulse equals the derivative of step to `1e-3` |

| WP 2.7 — Specifications and test signals | |
|---|---|
| Scope | `stepInfo` (steady state, rise, peak, overshoot, settling) with STL algorithms; deterministic signal generators |
| C++ | Iterators, `std::max_element`, `std::find_if`, reverse iterators, `std::distance` |
| Control | Second-order specification formulas and their use for pole-region design; ramp/sine/chirp/square signals |
| Produces | `stepInfo`, `signals/Signals.hpp` (`timeGrid`, `step`, `ramp`, `sine`, `square`, `chirp`) |
| Prerequisites | WP 2.6 |
| Acceptance | E2: overshoot `67.3 ± 0.3 %`, peak time `1.583 ± 0.002 s`, rise `0.56 ± 0.01 s`, settling `14.5 ± 0.2 s`; E1: rise `4.394`, settling `7.82`; ramp response of E1 lags by `τ` |

### 7.6 Stage 3 — Frequency Domain

| WP 3.1 — Frequency response and Bode | |
|---|---|
| Scope | `frequencyResponse` through `evaluate` at `jω` or `e^{jωT_s}`; `bode` with phase unwrapping; default frequency grids; math utilities; `plotBode` |
| C++ | Inline header utilities, `std::arg`, complex exponentials, logarithmic axes |
| Control | Sinusoidal steady state, Bode asymptotes, unwrapping, Nyquist frequency cap |
| Produces | `core/MathUtils.hpp`, `analysis/FrequencyResponse.hpp` (`FrequencyResponse`, `BodeData`, `defaultFrequencies`, `frequencyResponse`, `bode`), `plotBode` |
| Prerequisites | WP 2.7 |
| Acceptance | E1 at `0.5 rad/s`: `6.532 dB`, `−45°`; E2 at `2 rad/s`: `0 dB`, `−90°`, peak `0.07 dB` near `1.968`; unwrap of `[3, −3, 3]` gives `[3, 3.283, 3]` |

| WP 3.2 — Nyquist and stability margins | |
|---|---|
| Scope | `margins` from a model or from sampled data (gain, phase and delay margins as optionals; phase crossover located through `g = wrapToPi(φ + π)` sign changes with jump exclusion); `nyquist`; `plotNyquist` |
| C++ | `std::optional`, interpolation on sampled data |
| Control | Nyquist criterion `Z = N + P`, contour qualification for imaginary-axis poles, margin definitions and limits |
| Produces | `Margins`, `margins`, `NyquistData`, `nyquist`, `plotNyquist` |
| Prerequisites | WP 3.1 |
| Acceptance | `1/(s(s+1)(s+2))`: `GM = 15.56 dB` at `1.414`, `PM = 53.4°` at `0.446`, delay margin `2.09 s`; `1/(s+1)`: both margins absent; `10/(s(s+1))`: `PM = 18.0°` at `3.08` |

| WP 3.3 — Interconnections and sensitivity functions | |
|---|---|
| Scope | `series`, `parallel`, `feedback` for transfer functions and state space; operators for transfer functions; gang of four; `closedLoopStable` from the model |
| C++ | Overload sets across types, free operator functions, comma-initialised block matrices, consistency checks |
| Control | Block-diagram algebra, closed-loop poles, `S + T = 1`, algebraic loops |
| Produces | `systems/Interconnect.hpp` |
| Prerequisites | WP 3.2 |
| Acceptance | E2 with gain 10: denominator `{1,0.5,14}`, dc gain `10/14`; gain 6 on `1/(s(s+1)(s+2))` is marginal, 7 unstable; `(S + T).minreal() = 1`; `D_G = 1`, `D_H = −1` throws |

| WP 3.4 — Minimum phase, right-half-plane zeros, delay | |
|---|---|
| Scope | Padé approximation of delay; `isMinimumPhase` |
| C++ | Static factories, factorials, coefficient reversal, `std::any_of` |
| Control | Phase of RHP zeros, undershoot, bandwidth limits, all-pass factors, delay as non-minimum-phase behaviour |
| Produces | `TransferFunction::pade`, `analysis/Stability.hpp` (`isMinimumPhase`) |
| Prerequisites | WP 3.3 |
| Acceptance | `pade(1,1) = {−1,2}/{1,2}`; `pade(1,3)` phase `−57.30°` at `ω = 1`, `0 dB` everywhere; `(1−s)/((s+1)(s+2))` step starts with slope `−1`; `pade(0.5,2)` in the reference loop gives `PM ≈ 40.6°` |

| WP 3.5 — Root locus | |
|---|---|
| Scope | Locus by repeated root finding with nearest-neighbour branch continuation; asymptotes; boundary crossings; `plotRootLocus`, `plotPoleZero` |
| C++ | Nested vectors, greedy matching, comparators, marker plots |
| Control | Evans' rules, crossings as gain margin, locus as a design tool |
| Produces | `analysis/RootLocus.hpp` |
| Prerequisites | WP 3.4 |
| Acceptance | `1/(s(s+1)(s+2))`: asymptotes `{60°,180°,300°}`, centroid `−1`, crossing at `K = 6.0 ± 0.05`, `±1.414j`; `1/(s(s+2))` at `K = 5`: `−1 ± 2j` |

| WP 3.6 — Routh–Hurwitz, umbrella header, `ex02`, stage review | |
|---|---|
| Scope | Routh array with the `ε` and zero-row cases; `csl.hpp`; each header compiles standalone; `ex02_representations` exercising every module so far |
| C++ | Header hygiene, umbrella headers, `inline` rules |
| Control | Routh array, parametric stability ranges |
| Produces | `routhArray`, `routhRHPRootCount`, `routhIsStable`, `csl.hpp`, `examples/ex02_representations.cpp`, stage review |
| Prerequisites | WP 3.5 |
| Acceptance | `{1,2,3,4}` stable, `{1,2,3,10}` has 2 RHP roots, `{1,0,−1}` has 1, `{1,1,2,2,3}` has 2; `ex02` numbers agree across margins, locus and Routh (`K < 6`) |

### 7.7 Stage 4 — Classical Design

| WP 4.1 — `Controller` interface and PID | |
|---|---|
| Scope | Abstract `Controller` (Listing 3-4); `PID` in implementable form: filtered derivative, derivative on measurement, backward-Euler integral, output limits, setpoint weight, continuous transfer function for analysis |
| C++ | Abstract base designed from the consumer, configuration structs with default initialisers, stateful objects, `std::clamp` |
| Control | PID term effects, derivative filter, derivative kick, discrete law |
| Produces | `interfaces/Controller.hpp`, `control/PID.hpp` |
| Prerequisites | WP 3.6 |
| Acceptance | `K_p = 2`: `compute(0.5, 1) = 1`; `K_i = 1`, `T_s = 0.1`, ten unit errors give `K_p + 1`; `toTransferFunction` for `(2, 1, 0.5, N = 10)` is `{0.55, 2.025, 1}/{0.025, 1, 0}`; E1 with PI has closed-loop dc gain 1 |

| WP 4.2 — Memory model and the closed-loop `Simulator` (×2) | |
|---|---|
| Scope | Part A: stack versus heap, one deliberate `new`/`delete` exercise, RAII, `unique_ptr` ownership and transfer, incomplete-type rules. Part B: `Simulator` owning plant and controller, executing the sampled-data loop with RK4 sub-steps and zero-order hold, validating measurement kind and reference size, rejecting `D ≠ 0` |
| C++ | `std::unique_ptr`, `std::make_unique`, `std::move`, `std::shared_ptr` (when to use), `nullptr` |
| Control | Sampled-data loop, hold delay, sample-time rule, algebraic loops |
| Produces | `simulation/Simulator.hpp` (without the estimator member, added in WP 6.3) |
| Prerequisites | WP 4.1 |
| Acceptance | E1 with `K_p = 2`: final `0.8571`, closed-loop time constant `0.2857`; PI overshoot matches the transfer-function prediction within `0.3 %` at `T_s = 1 ms` and grows at `0.5 s`; a moved-from `unique_ptr` is null |

| WP 4.3 — Anti-windup and setpoint weighting | |
|---|---|
| Scope | Clamping and back-calculation strategies; comparative trajectory tests under saturation |
| C++ | `enum class` strategies, refactoring under test |
| Control | Windup, conditional integration, tracking time constant, two-degree-of-freedom structure |
| Produces | `PID` complete |
| Prerequisites | WP 4.2 |
| Acceptance | E3 with `±5 V` limit: overshoot with no anti-windup ≥ 2× clamping; back-calculation ≤ none; all equal without limits; `b = 0.5` leaves the disturbance response unchanged to `1e-9` |

| WP 4.4 — Tuning rules and relay auto-tuning | |
|---|---|
| Scope | FOPDT fit from a step response; Ziegler–Nichols step and ultimate rules; SIMC; `Relay` controller; relay experiment through the `Simulator` |
| C++ | `std::pair`/`std::tuple`, structured bindings, a trivial second `Controller` |
| Control | Process-reaction and ultimate-cycle methods, describing-function estimate `K_u ≈ 4d/(πa)`, SIMC |
| Produces | `control/Tuning.hpp`, `control/Relay.hpp` |
| Prerequisites | WP 4.3 |
| Acceptance | `ultimateFromModel` on the reference loop: `K_u = 6.00`, `T_u = 4.443`; relay estimate within 15 % and 3 %; FOPDT of E1 with `pade(0.5,3)`: `L = 0.5 ± 0.05`, `T = 2 ± 0.1`; SIMC PI gives `K_p = 0.667`, `T_i = 2`, overshoot `< 5 %` |

| WP 4.5 — Lead/lag compensation and root finding | |
|---|---|
| Scope | `bisection`, `goldenSectionMin`; error constants and system type; lead design by the Bode procedure (searching the new crossover by bisection); lag design |
| C++ | Root finding over `std::function`; aggregate initialisation of result structs |
| Control | `K_p`, `K_v`, `K_a`; `φ_max = asin((α−1)/(α+1))` at `√(zp)`; design procedure |
| Produces | `core/RootFinding.hpp`, `control/LeadLag.hpp` |
| Prerequisites | WP 4.4 |
| Acceptance | `lead(1,10)`: `φ_max = 54.9°` at `3.162`; `errorConstants(10/(s(s+1)))`: type 1, `K_v = 10`; `designLead(1/(s(s+1)), 10, 45°)` achieves `PM ∈ [45°, 52°]`; lag design on the reference loop achieves `≥ 44°` |

| WP 4.6 — Loop shaping, gang of four, `ex03`, stage review | |
|---|---|
| Scope | Peak sensitivity with implied margins; bandwidth; Bode's sensitivity integral; `plotGangOfFour`; `ex03_classical_design` on E3 |
| C++ | Composition; `const`-correctness review |
| Control | `S`/`T` trade-off, `M_s` guarantees, waterbed effect, loop-shaping rules |
| Produces | `sensitivityPeak`, `bandwidth`, `sensitivityIntegral`, `plotGangOfFour`, `examples/ex03_classical_design.cpp`, stage review |
| Prerequisites | WP 4.5 |
| Acceptance | Reference loop `M_s = 1.57` at `0.73 rad/s`, implied `GM = 8.8 dB`, `PM = 37.2°`; sensitivity integral `0 ± 0.02` (stable) and `π ± 0.05` for `5/((s−1)(s+3))`; bandwidth of `feedback(10/(s(s+1)))` is `4.83 rad/s` |

### 7.8 Stage 5 — Discrete Time and DSP

| WP 5.1 — Sampling and zero-order-hold discretization | |
|---|---|
| Scope | `c2d` (ZOH by augmented matrix exponential; forward and backward Euler; Tustin state-space form) and `d2c` (ZOH by matrix logarithm); discrete `damp` |
| C++ | `enum class` with exhaustive `switch`, Eigen `exp` and `log` |
| Control | Exact discrete model, pole mapping `z = e^{sT_s}`, aliasing, Nyquist frequency, hold delay |
| Produces | `DiscretizationMethod`, `c2d`, `d2c` for `StateSpace` |
| Prerequisites | WP 4.6 |
| Acceptance | E1 at `0.1 s`: `A_d = e^{−0.05}`, `B_d = 3(1 − e^{−0.05})` to `1e-9`; discrete step equals continuous step at sample instants to `1e-10`; `d2c(c2d(E3))` recovers `A`, `B` to `1e-8` |

| WP 5.2 — z-domain, Tustin and matched pole–zero, transfer-function controller | |
|---|---|
| Scope | Polynomial Tustin substitution with prewarping; matched pole–zero (zeros at `−1` to relative degree 1, dc gain matched); `d2c` for transfer functions; `TransferFunctionController` executing a discrete transfer function as a difference equation |
| C++ | Polynomial composition; a generic `Controller` adapter |
| Control | z-transform, difference equations, `s → z` maps, frequency warping |
| Produces | `c2d`/`d2c` for `TransferFunction`, `control/TransferFunctionController.hpp` |
| Prerequisites | WP 5.1 |
| Acceptance | Tustin of `1/(s+1)` at `0.1 s` is `{1/21, 1/21}/{1, −19/21}`; prewarped response exact at `ω₀`; MPZ of `1/(s+1)` has no finite zero, of `1/((s+1)(s+2))` one zero at `−1`; integrator controller outputs `(k−1)T_s`; forward Euler of E2 at `0.5 s` unstable, backward Euler and Tustin stable |

| WP 5.3 — DFT and FFT | |
|---|---|
| Scope | Iterative radix-2 FFT with bit reversal, inverse FFT, real-input wrappers, frequency grids, a naive DFT as test oracle, timing comparison |
| C++ | Bitwise operators, `std::size_t`, in-place algorithms, `std::swap`, `std::chrono` |
| Control | DFT definition, bins and resolution, conjugate symmetry, Parseval, Cooley–Tukey decomposition |
| Produces | `dsp/FFT.hpp` |
| Prerequisites | WP 5.2 |
| Acceptance | Impulse gives all ones; constant gives `[N, 0, …]`; `bitReverse(0b0011, 4) = 0b1100`; `fft` versus `dft` on random `N = 1024` within `1e-9`; `cos(2π·5t)` at `f_s = 64`, `N = 64` gives `|X[5]| = |X[59]| = 32`; FFT in milliseconds at `2¹⁶` versus DFT in seconds |

| WP 5.4 — Windows, periodogram, Welch | |
|---|---|
| Scope | Window functions; one-sided periodogram with correct scaling; Welch averaging; cross-spectral density; PSD integration; `plotSpectrum`; `SimResult::sampleRate`/`channel` |
| C++ | `enum class` factories, `Vector::segment`, coefficient-wise products, overlapping-segment indexing |
| Control | Leakage, window trade-offs, periodogram variance, Welch's method, cross-spectra |
| Produces | `dsp/Spectral.hpp` (`window`, `periodogram`, `welch`, `csd`, `integratePSD`), `plotSpectrum` |
| Prerequisites | WP 5.3 |
| Acceptance | Hann(8) table values to `1e-4`; sinusoid of amplitude `A` at `f_s = N = 1024`: peak-bin power × `Δf = A²/2`; unit-variance white noise at `1 kHz`: Welch level `0.002 ± 10 %`, integrated PSD `1 ± 5 %` |

| WP 5.5 — Digital filter structures | |
|---|---|
| Scope | `DigitalFilter` interface (Listing 3-5); `IIRFilter` (direct form II transposed), `FIRFilter` (circular delay line), `filtfilt`, moving average, exponential smoothing; `using DigitalFilter::process` in derived classes |
| C++ | Non-virtual convenience overloads, name hiding and `using` declarations, `final`, `clone` |
| Control | IIR/FIR structures, linear phase and group delay, zero-phase filtering, a filter as a discrete system |
| Produces | `interfaces/DigitalFilter.hpp`, `dsp/Filters.hpp` |
| Prerequisites | WP 5.4 |
| Acceptance | Moving average of 4 has a null at `f_s/4`; exponential smoothing `α = 0.1` has pole `0.9`; `IIRFilter(H).process(step)` equals `step(H)` sample-wise to `1e-12`; `filtfilt` has zero phase; symmetric FIR delays by `M/2`; `f.process(Vector)` compiles on the derived type |

| WP 5.6 — Filter design and function templates | |
|---|---|
| Scope | Butterworth poles, analog prototype, order formula, digital Butterworth by prewarped Tustin; windowed-sinc FIR; notch biquad; `rms`/`clampTo` templates; `Figure::line` accepting Eigen expressions |
| C++ | Function templates, `Eigen::MatrixBase<Derived>` idiom |
| Control | Maximally flat response, bilinear digitisation, windowed-sinc design, notch zero placement, second-order-section caveat for high orders |
| Produces | `dsp/FilterDesign.hpp`, templates in `core/MathUtils.hpp` |
| Prerequisites | WP 5.5 |
| Acceptance | `butterworthPoles(2,1) = −√½ ± √½ j`; `butterworthAnalog(3,1) = {1}/{1,2,2,1}`; `butterworth(4, 100, 1000)`: `−3.01 dB` at 100 Hz, `−27.97 dB` at 200 Hz; `butterworthOrder(2π·100, 2π·200, 1, 40) = 8`; 51-tap FIR has group delay 25; `notch(50, 2, 1000)` below `−40 dB` at 50 Hz and above `−1 dB` at 40/60 Hz |

| WP 5.7 — Digital control experiments | |
|---|---|
| Scope | Phase cost of sampling for emulated PI designs; dead-beat control; z-plane root locus; aliasing with and without an *analog* anti-aliasing filter modelled before the sampler; `setMeasurementFilter` for in-band noise |
| C++ | Composition and measurement |
| Control | Emulation versus direct design, `ΔPM ≈ ω_gc(T_s/2 + T_s)`, dead-beat, anti-aliasing placement |
| Produces | `Simulator::setMeasurementFilter`, `tests/control/test_DigitalControl.cpp` |
| Prerequisites | WP 5.6 |
| Acceptance | Measured PM loss within `2°` of prediction for `ω_gc T_s < 0.5`; dead-beat reaches the reference in one sample to `1e-9`; z-plane crossing gain equals the discrete gain margin within 1 %; 450 Hz interference at `f_s = 500 Hz` aliases to 50 Hz, is unchanged by a digital post-filter, and is attenuated by at least 20 dB by the analog pre-filter |

| WP 5.8 — `ex04` and stage review | |
|---|---|
| Scope | Chirp, FFT, Butterworth filtering with spectra before and after, notch removal of a 50 Hz tone from a noisy E2 response, `filtfilt` versus causal filtering, the sampling table; zero warnings; Release tests; optional sanitizer run |
| Produces | `examples/ex04_dsp_and_filters.cpp`, stage review |
| Prerequisites | WP 5.7 |
| Acceptance | Five plots produced; zero warnings at `/W4`; Release suite passes |
### 7.9 Stage 6 — State-Space Design

| WP 6.1 — Controllability and observability | |
|---|---|
| Scope | Numerical rank with a relative singular-value threshold; controllability and observability matrices; PBH tests naming the offending modes; stabilizability and detectability for continuous and discrete systems |
| C++ | SVD-based rank, block assembly in loops |
| Control | Kalman rank tests, duality, PBH, stabilizability and detectability, conditioning of `W_c` |
| Produces | `core/Controllability.hpp`, `StateSpace::isControllable/isObservable` |
| Prerequisites | WP 5.2 (core track) |
| Acceptance | E4 controllable and observable; `A = −I`, `B = [1;1]` has rank 1 and uncontrollable mode `−1` but is stabilizable; `diag(1,−1)`, `B = [0;1]` not stabilizable; `tf2ss({1,1}/{1,3,2})` has unobservable mode `−1`; `numericalRank([[1,2],[2,4+1e-15]]) = 1` |

| WP 6.2 — Pole placement | |
|---|---|
| Scope | Ackermann's formula by a transposed solve; `Polynomial::evaluate(Matrix)`; feedforward gain; `StateFeedbackController` declaring `MeasurementKind::State` |
| C++ | Horner with matrices, solving instead of inverting |
| Control | `det(sI − A + BK) = Δ_d(s)`, pole-selection guidelines, `N̄` |
| Produces | `control/StateFeedback.hpp` (`place`, `feedforwardGain`, `StateFeedbackController`) |
| Prerequisites | WP 6.1 |
| Acceptance | E4 poles `−1 ± j` give `K = [2, 2]`; E3 poles `{−20,−25}` give `charPoly(A − BK) = {1,45,500}`; discrete E1 pole `{0}` gives `A_d − B_dK = 0`; uncontrollable pair throws |

| WP 6.3 — Observer, `Estimator` interface, separation principle | |
|---|---|
| Scope | `Estimator` (Listing 3-5); current-estimator Luenberger observer with gain by duality; `Simulator` estimator member, per-tick order `update → compute → predict`, sample-time check; observer-based compensator for analysis |
| C++ | `unique_ptr` member added once the type is complete, operation ordering |
| Control | Error dynamics, observer pole selection, separation principle, compensator form |
| Produces | `interfaces/Estimator.hpp`, `estimation/LuenbergerObserver.hpp`, `Simulator::setEstimator`, `observerBasedCompensator` |
| Prerequisites | WP 6.2 |
| Acceptance | Error-norm ratio approaches `e^{−0.4}` for observer poles `e^{−0.4}`, `e^{−0.5}`; closed-loop poles of `feedback(plant, compensator)` equal `{−20,−25,−40,−50}` to `1e-8`; exact initial estimate tracks the true state to `1e-12` |

| WP 6.4 — Lyapunov equations, Gramians, H2 norm (×2) | |
|---|---|
| Scope | Part A: `kron`, `vec`, `unvec`, `lyap`, `dlyap`, Lyapunov stability test. Part B: Gramians, `h2norm` (continuous strictly proper; discrete including `tr(DᵀD)`) |
| C++ | Kronecker products, `Eigen::Map`, Cholesky as definiteness test, symmetrisation |
| Control | Lyapunov's direct method, Gramians and minimum energy, H2 norm as impulse energy |
| Produces | `core/Lyapunov.hpp`, `isLyapunovStable`, `analysis/Norms.hpp` (`h2norm`) |
| Prerequisites | WP 6.3 |
| Acceptance | `lyap(−1, 2) = 1`; Lyapunov residual `< 1e-12` and `X ≻ 0` for E2; Gramian equals the numerical integral to `1e-4`; `h2norm({1},{1,1}) = √½`, `h2norm(E2) = 0.5`, discrete `0.5/(z−0.5)` gives `1/√3`, discrete static gain 1 gives 1 |

| WP 6.5 — Feedforward and integral action | |
|---|---|
| Scope | `(N_x, N_u)` feedforward gains; integral augmentation; `IntegralStateFeedbackController` with anti-windup |
| C++ | Augmented block matrices, stateful controller configuration |
| Control | Steady-state feedforward, integral augmentation and its robustness, controllability condition (no zero at the origin) |
| Produces | `feedforwardGains`, `augmentWithIntegrator`, `IntegralStateFeedbackController` |
| Prerequisites | WP 6.4 |
| Acceptance | `N̄` equals WP 6.2's value to `1e-12`; exact-model feedforward tracks to `1e-6`, a 20 % parameter error leaves an offset; integral action rejects a `2 V` disturbance to `1e-3` within 2 s; augmentation of a plant with a zero at 0 is uncontrollable |

| WP 6.6 — Copy and move audit | |
|---|---|
| Scope | Static assertions of nothrow movability for every value type; `Simulator` with deleted copies and explicitly defaulted `noexcept` moves; a timed copy-versus-move benchmark; a return-value-elision check; a one-page object-model note |
| C++ | Special member functions, Rule of Zero, deleted copies suppressing implicit moves, `std::move` as a cast, RVO, `noexcept` |
| Control | — |
| Produces | `tests/core/test_SpecialMembers.cpp`, annotations on `Simulator` and the abstract bases, `docs/notes/object_model.md` |
| Prerequisites | WP 6.5 |
| Acceptance | All static assertions compile; move at least 100× faster than copy for a `500×500` system; returned `SimResult` keeps its buffer address |

| WP 6.7 — `ex05` and stage review | |
|---|---|
| Scope | E3 with speed measurement only: controllability report, pole placement with feedforward, observer at `1 ms`, integral action under a load disturbance, Bode and margins of the compensator loop, H2 norm from disturbance to speed; API friction corrected in the library |
| Produces | `examples/ex05_state_space_design.cpp`, stage review |
| Prerequisites | WP 6.6 |
| Acceptance | Example runs; results consistent with WP 6.2–6.5 tests |

### 7.10 Stage 7 — Optimal Control

| WP 7.1 — LQR and the continuous Riccati equation (×2) | |
|---|---|
| Scope | Part A: derivation by completing the square; `solveARE` by the Hamiltonian eigenvector method with residual, symmetry and stabilizing-property checks. Part B: `lqr` with stabilizability and weight preconditions; cost verification against a simulated integral |
| C++ | Block matrices with `<<`, complex eigenvectors, transposed complex solves, residual self-checks |
| Control | LQR problem and solution, existence conditions (stabilizable; no unobservable mode of `(A, Q^{1/2})` on the imaginary axis), Hamiltonian structure |
| Produces | `core/Riccati.hpp` (`solveARE`, `solveCARE`), `control/LQR.hpp` (`lqr`, `lqrCost`) |
| Prerequisites | WP 6.7 |
| Acceptance | E4 with `Q = I`, `R = 1`: `P = [[√3,1],[1,√3]]`, `K = [1, √3]`, poles `−0.866 ± 0.5j`, residual `< 1e-12`; E3 simulated cost within `0.5 %` of `x₀ᵀPx₀`; `A = 0, B = 1, Q = 0` throws while `A = 1, S = 1, Q = 0` returns the stabilizing `P = 2` |

| WP 7.2 — Discrete LQR and the DARE | |
|---|---|
| Scope | Finite-horizon Riccati recursion returning time-varying gains; `solveDARE` by iteration to convergence with stabilizing verification; `dlqr` |
| C++ | Backward loops over `std::vector<Matrix>`, convergence loops with diagnostics |
| Control | Bellman principle for the LQ problem, time-varying gains near the horizon end, DARE, sampled-data limit |
| Produces | `solveDARE`, `finiteHorizonLQR`, `dlqr` |
| Prerequisites | WP 7.1 |
| Acceptance | Discrete E4 at `0.1 s`: DARE residual `< 1e-9`; `K[0]` of a 300-step horizon equals `dlqr` to `1e-6`; `dlqr(c2d(E3, 1 ms), QT_s, RT_s)` within 2 % of `lqr`; summed closed-loop cost within 1 % of `x₀ᵀPx₀` |

| WP 7.3 — LQR properties | |
|---|---|
| Scope | Bryson weights; speed–effort sweep; loop transfer at the plant input and the return-difference bound; output weighting; integral LQR |
| C++ | Parameter sweeps and tables, bound assertions on grids |
| Control | Bryson's rule, symmetric root locus, guaranteed margins and their scope |
| Produces | `brysonWeights`, `lqrLoopTransfer`, `returnDifferenceMin`, `lqrIntegral` |
| Prerequisites | WP 7.2 |
| Acceptance | Monotone settling time and peak input over `ρ ∈ {0.01…100}`; `PM ≥ 60°` and `min|1 + L(jω)| ≥ 1` for every `ρ`; E4 with `Q = diag(1,0)` has poles at `135°` for all `ρ`; integral LQR rejects a step disturbance |

| WP 7.4 — Nonlinear systems and linearization | |
|---|---|
| Scope | `NonlinearSystem` over `std::function`; central-difference Jacobians; Newton equilibrium search; `linearize`; `Simulator` accepting both plant kinds; cart–pole model |
| C++ | Numerical differentiation, Newton's method, refactoring the simulator without behaviour change |
| Control | Equilibria, linearization, Lyapunov's indirect method, region of attraction |
| Produces | `systems/NonlinearSystem.hpp`, `core/Jacobian.hpp`, `plants::cartPole` |
| Prerequisites | WP 7.3 |
| Acceptance | Jacobian of `[x₁², x₁x₂]` at `[2,3]` is `[[4,0],[3,2]]` to `1e-7`; cart–pole linearization matches the closed form to `1e-6` with poles `{0,0,±4.646}`; LQR on the linearization balances the nonlinear plant from `θ₀ = 0.2` and fails from `1.0`; linear plant through `fromLinear` reproduces WP 4.2 to `1e-12` |

| WP 7.5 — Dynamic programming on a grid | |
|---|---|
| Scope | Value iteration with bilinear interpolation on a flattened grid; quadratic-cost and time-to-origin variants of the constrained double integrator |
| C++ | Multi-dimensional indexing, interpolation, arg-min loops, timing |
| Control | Bellman equation, value iteration, curse of dimensionality, bang-bang switching curve |
| Produces | `control/DynamicProgramming.hpp` |
| Prerequisites | WP 7.2 |
| Acceptance | 1-D `x⁺ = 0.9x + u` value matches `1.4839x²` within 3 %; policy signs at `(1,0)`, `(−1,0)`, `(0,1)`, `(−0.6,1)` are `−,+,−,+`; quadratic-cost value table saved for WP 10.5 |

| WP 7.6 — `ex06` and stage review | |
|---|---|
| Scope | Cart–pole: linearization, Bryson weights, continuous and discrete LQR, nonlinear closed loop, `ρ` sweep, input-loop margins, region-of-attraction scan, finite-horizon gain plot |
| Produces | `examples/ex06_lqr_pendulum.cpp`, stage review |
| Prerequisites | WP 7.4, 7.5 |
| Acceptance | Example runs with eight sections |

### 7.11 Stage 8 — Estimation

| WP 8.1 — Randomness, noise, PRBS, Monte Carlo | |
|---|---|
| Scope | `WhiteNoise`, `MultivariateNormal` (Cholesky), `PRBS` (maximal-length LFSR); sample covariance; noise discretization; `Simulator` noise hooks; `monteCarlo` over a factory lambda |
| C++ | `<random>`, seeding policy, generators as members, LFSR bit operations, factory lambdas for move-only objects |
| Control | White and Gaussian noise, process and measurement noise, `Q_d` from `Q_c`, PRBS properties, Monte Carlo evaluation |
| Produces | `signals` noise classes, `core/Statistics.hpp` (`sampleCovariance`, `discretizeNoise`), `Simulator::setMeasurementNoise/setProcessNoise`, `simulation/MonteCarlo.hpp` |
| Prerequisites | WP 7.3 (core track) |
| Acceptance | `WhiteNoise(2, 42)`: variance `4 ± 2 %`, flat PSD; `PRBS(10)`: period 1023, 512 ones, autocorrelation `≤ 0.01`; `discretizeNoise(−1, 2, 0.1) = 1 − e^{−0.2}`; Monte Carlo output deviation within 30 % of `σ·h2norm` |

| WP 8.2 — Kalman filter (×2) | |
|---|---|
| Scope | Part A: scalar conditional-mean derivation, covariance propagation. Part B: `KalmanFilter` with Joseph-form update, innovation diagnostics, NIS; consistency tests |
| C++ | Diagnostic accessors, symmetrisation, statistical tests with seeds |
| Control | Predict/update recursion, gain as trust ratio, innovation whiteness, covariance convergence |
| Produces | `estimation/KalmanFilter.hpp` |
| Prerequisites | WP 8.1 |
| Acceptance | Constant-velocity target (seed 7): position RMSE `< 0.45` against measurement RMSE 1; mean NIS in `[0.8, 1.25]`; converged `P⁻` equals `solveDARE(Aᵀ, Cᵀ, Q, R)` to `1e-8`; overconfident `R` gives NIS `> 3`; `C = 0` makes `tr P` unbounded |

| WP 8.3 — Steady-state Kalman filter and LQG | |
|---|---|
| Scope | Steady-state gain by the DARE on the dual; `LQGController` composing a Kalman filter and the LQR gain; compensator export; margin check; Monte Carlo comparison with a faster pole-placed observer |
| C++ | Composition of an estimator and a gain inside a controller |
| Control | Duality table, separation theorem, LQG as a dynamic compensator, loss of guaranteed margins |
| Produces | `steadyStateKalman`, `control/LQG.hpp` (`LQGController`, `lqgCompensator`) |
| Prerequisites | WP 8.2 |
| Acceptance | Steady-state gain equals the converged gain to `1e-8`; closed-loop poles equal `eig(A − BK) ∪ eig((I − LC)A)`; Kalman RMSE below the faster observer's in Monte Carlo mean (experiment, margin recorded); fast filter drops PM below `30°` |

| WP 8.4 — Extended Kalman filter | |
|---|---|
| Scope | EKF with optional analytic Jacobians and numerical fallback; discretization by one RK4 step; angle-wrapping hook; pendulum and cart–pole tests |
| C++ | `std::optional<std::function>`, Jacobian of the discrete map |
| Control | Relinearisation, divergence, `Q` inflation |
| Produces | `estimation/ExtendedKalmanFilter.hpp`, `plants::pendulum` |
| Prerequisites | WP 8.3, 7.4 |
| Acceptance | EKF equals KF on a linear plant to `1e-9`; pendulum `θ` RMSE `< 0.02` (seed 3), analytic versus numerical Jacobians within `1e-6`, mean NIS in `[0.7, 1.4]`; cart–pole balances from `0.15` with noisy `[p, θ]` |

| WP 8.5 — Unscented Kalman filter | |
|---|---|
| Scope | Unscented transform with guarded Cholesky; UKF sharing the EKF configuration fields; comparisons recorded as experiments |
| C++ | `Eigen::LLT` square roots, sigma-point matrices, weighted statistics |
| Control | Sigma points and weights, second-order accuracy, cross-covariance gain |
| Produces | `unscentedTransform` in `core/Statistics.hpp`, `estimation/UnscentedKalmanFilter.hpp` |
| Prerequisites | WP 8.4 |
| Acceptance | Linear map reproduced to `1e-12`; `x²` of `N(0,1)` gives mean 1, variance 2 to `1e-9`; pendulum RMSE within 10 % of the EKF's; convergence and range–bearing comparisons recorded |

| WP 8.6 — `ex07` and stage review | |
|---|---|
| Scope | Target tracking with NIS band; steady-state KF versus observer under noise; LQG margins for two filter speeds; cart–pole with EKF and UKF estimates, Monte Carlo of 20 runs; range–bearing comparison |
| Produces | `examples/ex07_estimation.cpp`, stage review |
| Prerequisites | WP 8.5 |
| Acceptance | Example runs with five sections |

### 7.12 Stage 9 — Identification

| WP 9.1 — ARX identification | |
|---|---|
| Scope | Regression matrix, QR least squares, condition number, one-step prediction and simulation, fit metric, train/validation split; bias under output noise stated |
| C++ | Regression assembly, `ColPivHouseholderQR`, SVD condition numbers |
| Control | ARX model, persistency of excitation, prediction versus simulation fit, order selection, coloured equation error |
| Produces | `sysid/ARX.hpp`, `sysid/Validation.hpp` (`fitPercent`, `splitData`) |
| Prerequisites | WP 8.1, 5.2 |
| Acceptance | Noise-free PRBS data recover `c2d(E3, 0.01)` to `1e-8`; output noise `σ = 0.01` gives validation fit `> 95 %` and poles within 2 % (bias recorded at `σ = 0.05`); order sweep saturates at 2; step input gives condition number `> 1e6` |

| WP 9.2 — Recursive least squares | |
|---|---|
| Scope | RLS with forgetting; parameter-history helper; equivalence tests with regularised batch least squares and with a Kalman filter on a static parameter |
| C++ | Stateful online estimator, equivalence testing |
| Control | Matrix-inversion-lemma recursion, forgetting factor, covariance blow-up, RLS as a Kalman filter |
| Produces | `sysid/RLS.hpp`, `KalmanFilter` time-varying `C` setter |
| Prerequisites | WP 9.1 |
| Acceptance | RLS with `λ = 1`, `P₀ = p₀I` equals `(ΦᵀΦ + I/p₀)⁻¹Φᵀy` to `1e-9` and `arx` to `1e-5` at `p₀ = 1e8`; `λ = 0.98` tracks a doubled gain within 200 samples; `tr P` grows 100× without excitation; RLS equals the Kalman filter to `1e-10` |

| WP 9.3 — Frequency-domain identification (×2) | |
|---|---|
| Scope | Part A: ETFE through Welch cross-spectra with coherence. Part B: Levy/Sanathanan–Koerner rational fitting (continuous and discrete), FOPDT from a frequency response, margins from measured data |
| C++ | Complex coefficient-wise operations, weighted complex least squares by real stacking, iterative reweighting |
| Control | Smoothed ETFE, coherence, rational fitting, model-free margins |
| Produces | `dsp::etfe`, `sysid/FrequencyDomainID.hpp` (`fitRational`, `fitFOPDTfromFRF`) |
| Prerequisites | WP 9.2, 5.4 |
| Acceptance | E2 chirp data: `|Ĥ|` within `0.5 dB` and `3°` where coherence `> 0.95`; continuous fit recovers `{1}/{1,0.5,4}` within 2 %; discrete fit poles within 1 %; measured-loop PM within `2°` of the model; FOPDT of E1 with delay recovers `K = 3`, `T = 2`, `L = 0.5` |

| WP 9.4 — ERA, validation, `ex08`, stage review | |
|---|---|
| Scope | Markov parameters from impulse data or by correlation; eigensystem realisation with order selection by singular-value gap; residual autocorrelation and cross-correlation tests; `ex08` comparing ARX, ERA and rational fit |
| C++ | Hankel assembly, `Eigen::BDCSVD`, truncation |
| Control | Hankel rank as order, balanced realisation, whiteness and independence tests |
| Produces | `sysid/Subspace.hpp`, `validate`, `examples/ex08_sysid.cpp`, stage review |
| Prerequisites | WP 9.3 |
| Acceptance | Exact impulse data: `s(2)/s(1) < 1e-10`, poles to `1e-8`; 1 % noise keeps order 2 clear and poles within 2 %; correlation-derived Markov parameters within 3 %; whiteness true for the correct order and false for `n_a = 1`; three methods agree within 2 % |

### 7.13 Stage 10 — Robust, Predictive and Nonlinear Optimal Control

| WP 10.1 — Uncertainty models and small gain | |
|---|---|
| Scope | Relative-error envelope from a plant family; delay uncertainty weight `2.2τs/(τs + 1)` with numerical cover verification; robustness report (robust stability, nominal and robust performance) |
| C++ | Pointwise operations on frequency responses, report structs |
| Control | Multiplicative uncertainty, small-gain theorem, `‖W_mT‖∞ < 1`, robust performance, Nyquist disc picture |
| Produces | `analysis/Robustness.hpp` |
| Prerequisites | WP 8.3, 9.3 |
| Acceptance | Weight covers `2|sin(ωτ/2)|` for all `τ ≤ τ_max`; robust stability turns false slightly before the exact delay margin; ±30 % gain family gives envelope `0.3`; an over-demanding `W_s` fails robust performance |

| WP 10.2 — H∞ norm | |
|---|---|
| Scope | Bounded-real-lemma bisection on the Hamiltonian; MIMO `σ_max`; discrete systems through the bilinear map; grid version for data; exact robust-stability test |
| C++ | Bisection on a spectrum property, bracket selection |
| Control | Worst-case gain, bounded-real lemma, why grids miss narrow peaks |
| Produces | `hinfnorm`, `sigmaMax`, `peakFrequency` in `analysis/Norms.hpp` |
| Prerequisites | WP 10.1 |
| Acceptance | `hinfnorm({1},{1,1}) = 1`; `hinfnorm(E2) = 1.0079` at `1.968` with a 200-point grid at least 0.1 % low; `‖S‖∞ = 1.57` for the reference loop; MIMO example within `1e-4` of a dense grid; discrete E2 within 0.1 % |

| WP 10.3 — Quadratic programming | |
|---|---|
| Scope | KKT conditions; equality-constrained QP by the KKT system; Hildreth's dual coordinate ascent with the correct update `λ_i ← max(0, (d_i − Σ_{j≠i}P_{ij}λ_j)/P_{ii})`, `d = Ax_u − b`; null-space elimination of equalities; status enum; warm start; KKT residual |
| C++ | Solver status, warm starts, `CompleteOrthogonalDecomposition`, `FullPivLU::kernel` |
| Control | Convex QP, KKT, duality, infeasibility detection |
| Produces | `core/QP.hpp` (`solveQP`, `solveEqualityQP`, `kktResidual`) |
| Prerequisites | WP 7.2 |
| Acceptance | Table 5-3 test QP gives `(0.5, 1.5)`, `λ = 1`; inactive constraint gives `(1, 2)`; box constraints; random 20-variable QP with KKT residual `< 1e-7`; equality case `(0.5, 0.5)`; mixed case `(1,1,1)`; infeasible pair throws; warm start under 20 % of cold iterations |

| WP 10.4 — MPC formulation (×3 with 10.5, part A) | |
|---|---|
| Scope | Prediction matrices `F`, `Φ`; condensed cost; input, rate and state constraints; terminal weight from the DARE; control horizon with blocking; `buildQP` separated from `compute` |
| C++ | Block-Toeplitz assembly, configurations with optional constraint fields |
| Control | Receding horizon, condensed QP, terminal cost as Lyapunov function, feasibility |
| Produces | `control/MPC.hpp` |
| Prerequisites | WP 10.3, 7.2 |
| Acceptance | `F`, `Φ` reproduce `lsim` to `1e-12`; unconstrained MPC with `Q_N = P` equals `−K_dlqr x` to `1e-8` at `N = 5`; `Q_N = Q` converges to `dlqr` as `N` grows; KKT residual `< 1e-8` with bounds; blocking equalises later inputs |

| WP 10.5 — Constrained MPC, comparisons, `ex09` (part B) | |
|---|---|
| Scope | Soft output constraints with slack variables; warm start; solve timing; comparisons with the DP policy (same quadratic cost) and saturated LQR; cart–pole MPC with force limits and the EKF; state-constrained balancing; Monte Carlo |
| C++ | Slack augmentation, timing, Monte Carlo of a controller |
| Control | Soft constraints, planning saturation, tuning knobs |
| Produces | `MPC` complete, `examples/ex09_mpc.cpp` |
| Prerequisites | WP 10.4, 7.5, 8.4 |
| Acceptance | Closed-loop cost within 3 % of the DP optimum and not above saturated LQR's (experiment, values recorded); `|u| ≤ 1 + 1e-9`; soft constraint slack vanishes while the hard version throws at `k = 0`; cart–pole balances from `0.25 rad` with `|F| ≤ 5` where saturated LQR fails; warm start halves iterations |

| WP 10.6 — Iterative LQR | |
|---|---|
| Scope | iLQR with regularised backward pass, backtracking line search, cost-based convergence; cart–pole swing-up; tracking the result with the time-varying feedback |
| C++ | Large-algorithm bookkeeping, line search, regularisation |
| Control | Local LQR along a trajectory, feedforward plus feedback, difference from DDP |
| Produces | `control/iLQR.hpp` |
| Prerequisites | WP 7.4, 7.2 |
| Acceptance | Linear quadratic problem converges in ≤ 2 iterations to the finite-horizon LQR rollout (`1e-8`); swing-up reaches `|θ_N| < 0.05`, `|p_N| < 0.2` with monotone cost in `< 60` iterations and at least one swing reversal; tracking from `π + 0.05` ends within `0.1` |

| WP 10.7 — H∞ synthesis (×3) | |
|---|---|
| Scope | Part A: disturbance-rejection generalized plant `w = [d; n]`, `y = G(u + d) + n`, `z = [W_sG(u + d); ρu; W_tG(u + d)]`, and a regularity checker. Part B: DGKF central controller at fixed `γ` with existence conditions. Part C: γ-iteration by bisection, closed-loop construction, verification against `hinfnorm` and the LQG limit |
| C++ | Decomposition of a large function into testable units with assumption checks |
| Control | Generalized plant and LFT, H∞ problem, two Riccati equations, `ρ(X∞Y∞) < γ²`, LQG as the `γ → ∞` limit; the reference-tracking form deferred to Table 11-1 |
| Produces | `control/HInfinity.hpp` (`disturbanceRejectionPlant`, `checkRegularity`, `hinfsynFixedGamma`, `hinfsyn`, `closedLoop`) |
| Prerequisites | WP 10.2, 8.3 |
| Acceptance | `G = 1/(s−1)` with the stated weights passes `checkRegularity`; `hinfnorm(closedLoop) ≤ γ*(1 + 1e-3)`; `K` stabilises `G`; each row of `T_zw` below `γ*` on a grid; at `γ = 10⁴` the controller matches LQG within 1 %; a fixed `γ` below `γ*` reports the failed condition; the biproper reference-tracking plant fails regularity with `D₁₁ ≠ 0` named |

### 7.14 Stage 11 — Release

| WP 11.1 — Installation and export | |
|---|---|
| Scope | Install rules for `csl` and the `eigen` interface target in one export set; vendored Eigen headers; package configuration and version files; consumer project and install-test scripts; the `find_dependency(Eigen3)` alternative documented |
| C++ | `install(TARGETS … EXPORT …)`, `install(EXPORT …)`, `CMakePackageConfigHelpers`, build versus install interfaces |
| Produces | install rules, `cmake/cslConfig.cmake.in`, `tests/consumer/`, `scripts/test_install.*` |
| Prerequisites | core track complete |
| Acceptance | Installed tree contains the library, `csl` and Eigen headers, and the config files; the consumer compiles and runs with `find_package(csl)` only and sees Eigen through the exported target; `find_package(csl 3.0)` fails with a version message |

| WP 11.2 — Documentation | |
|---|---|
| Scope | README with module table, quick start and plot gallery; documentation comment on every public declaration (scripted check); five tutorials derived from the examples; naming and convention audit |
| Produces | `README.md`, `docs/tutorials/`, API comments, optional `docs/Doxyfile` |
| Prerequisites | WP 11.1 and the completed branches |
| Acceptance | Quick start executes from a clean clone; zero public declarations without a documentation comment |

| WP 11.3 — Continuous integration and static checks | |
|---|---|
| Scope | Extend the CI workflow: Windows and Linux, Debug and Release, sanitizers on Linux, install test, dependency cache, formatting check; warnings as errors; `CHANGELOG.md`; optional `clang-tidy` |
| Produces | `.github/workflows/ci.yml` (full), `.clang-format`, `CHANGELOG.md` |
| Prerequisites | WP 11.2 |
| Acceptance | Both jobs green; sanitizers report nothing |

| WP 11.4 — Full pipeline and retrospective (×3) | |
|---|---|
| Scope | `ex10_full_pipeline` on E3 with the cart–pole finale: modelling and plots; PRBS experiment with noise; identification by three methods with validation; SIMC design with margins, saturation and discretization cost; LQR, steady-state Kalman filter, LQG, margins, Monte Carlo; MPC with voltage and current limits; uncertainty from model spread, small gain, H∞ norm and synthesis; iLQR swing-up, EKF balancing, MPC under force limits; residual PSD and whiteness; all outputs to `docs/img/` and CSV. Retrospective on C++, control, the type contract and future work |
| Produces | `examples/ex10_full_pipeline.cpp`, `docs/RETROSPECTIVE.md`, tag `v2.0.0` |
| Prerequisites | all stages |
| Acceptance | Table 2-3 criteria F1–F7 |

---
## 8. Function Inventory

Every public function and class of the finished library, by module, with its purpose and the work package that creates it. Signatures use the type aliases of Listing 3-1; `sys` denotes an `LTISystem` reference unless stated. Throwing behaviour follows C13 and is stated only where it is specific.

### 8.1 core

**Table 8-1 — `csl` (core)**

| Declaration | Purpose | WP |
|---|---|---|
| `Types.hpp`: `Real`, `Index`, `Vector`, `RowVector`, `Matrix`, `Complex`, `ComplexVector`, `ComplexMatrix`; `PI`, `TWO_PI`, `EPS`, `INF` | Type aliases and constants (Listing 3-1) | 1.6 |
| `Exceptions.hpp`: `Error`, `DimensionMismatch`, `InvalidArgument`, `NumericalError`, `NotSupported` | Exception hierarchy (Listing 3-6) | 1.8 |
| `Real dB(Real)`, `Real fromDB(Real)`, `Real rad2deg(Real)`, `Real deg2rad(Real)`, `Real wrapToPi(Real)` | Unit conversions | 3.1 |
| `Vector linspace(Real a, Real b, Index n)`; `Vector logspace(Real exp10_a, Real exp10_b, Index n)` | Grids | 3.1 |
| `void unwrapPhase(Vector& phase_rad)` | Remove `2π` jumps in place | 3.1 |
| `template<class Derived> Real rms(const Eigen::MatrixBase<Derived>&)`; `template<class T> T clampTo(T v, T lo, T hi)` | Expression-friendly utilities | 5.6 |
| `class Polynomial`: ctors from `initializer_list` or `Vector` (descending); `static fromRoots(roots, leading)`; `degree()`, `coeffs()`, `operator[]`, `leading()`; `operator()(Real)`, `operator()(Complex)`; `derivative()`, `monic()`, `evaluate(const Matrix&)`; `roots()`; `isApprox(other, tol)`; `operator+ − *`, scalar `*`, unary `−`, `operator<<`; free `pow(p, k)` | Real polynomials; roots by companion-matrix eigenvalues | 2.1, 2.2, 6.2 |
| `Real bisection(f, a, b, tol = 1e-10, maxIter = 200)`; `Real goldenSectionMin(f, a, b, tol = 1e-8)` | Scalar root finding and minimisation over `std::function<Real(Real)>` | 4.5 |
| `Index numericalRank(const Matrix&, Real tol = −1)` | Rank by singular values with relative threshold | 6.1 |
| `Matrix controllabilityMatrix(A, B)`; `Matrix observabilityMatrix(A, C)`; `bool isControllable(A, B, tol)`; `bool isObservable(A, C, tol)` | Kalman rank tests | 6.1 |
| `std::vector<Complex> uncontrollableModes(A, B, tol)`; `unobservableModes(A, C, tol)`; `bool isStabilizable(A, B, Ts = 0, tol)`; `bool isDetectable(A, C, Ts = 0, tol)` | PBH tests and weaker conditions | 6.1 |
| `Matrix kron(A, B)`; `Vector vec(const Matrix&)`; `Matrix unvec(const Vector&, rows, cols)` | Vectorisation helpers | 6.4 |
| `Matrix lyap(A, Q)`; `Matrix dlyap(A, Q)` | `AX + XAᵀ + Q = 0`; `AXAᵀ − X + Q = 0` | 6.4 |
| `Matrix controllabilityGramian(const StateSpace&)`; `observabilityGramian(const StateSpace&)` | Gramians (continuous or discrete) | 6.4 |
| `Matrix solveARE(A, S, Q)`; `Matrix solveCARE(A, B, Q, R)` | `AᵀX + XA − XSX + Q = 0`, stabilizing solution, verified | 7.1 |
| `Matrix solveDARE(A, B, Q, R, tol = 1e-10, maxIter = 10000)` | Discrete ARE by Riccati iteration, verified | 7.2 |
| `Matrix numericalJacobian(f, x)`; `Vector newtonSolve(f, x0, tol)` | Central-difference Jacobian; Newton's method | 7.4 |
| `Matrix sampleCovariance(const Matrix& rows)`; `Matrix discretizeNoise(A, Qc, Ts)` | Statistics; `∫₀^{T_s} e^{Aτ}Q_c e^{Aᵀτ}dτ` | 8.1 |
| `UnscentedTransformResult unscentedTransform(g, mean, cov, alpha, beta, kappa)` | Sigma-point transform (`mean`, `cov`, `crossCov`, `sigmaOut`) | 8.5 |
| `enum class QPStatus`; `struct QPResult { x, lambda, mu, status, iterations, maxViolation, kktResidual }` | Solver outcome | 10.3 |
| `QPResult solveQP(H, f, A, b, Aeq, beq, tol, maxIter, lambda0)`; `QPResult solveEqualityQP(H, f, Aeq, beq)`; `Real kktResidual(H, f, A, b, result)` | Convex QP by Hildreth's method with null-space elimination; KKT system; KKT check | 10.3 |

### 8.2 systems

**Table 8-2 — `csl::systems`**

| Declaration | Purpose | WP |
|---|---|---|
| `class LTISystem` (Listing 3-2); `struct PoleInfo { pole, wn, zeta, tau }`; `std::vector<PoleInfo> damp(sys)` | Abstract base; pole summary | 2.3 |
| `class StateSpace : LTISystem`: `StateSpace(A, B, C, D, Ts = 0)`; `A() B() C() D()`; `derivative(x, u)`, `output(x, u)`; `poles()`, `zeros()`, `evaluate(s)`, `dcGain()`; `isControllable(tol)`, `isObservable(tol)` | MIMO state-space model; `n = 0` permitted | 1.8, 1.9, 2.3, 6.1 |
| `class TransferFunction : LTISystem`: ctors `(Polynomial, Polynomial, Ts)` and `(init_list, init_list, Ts)`; `static fromZPK(zeros, poles, gain, Ts)`; `static pade(T, order)`; `num() den()`; `isProper()`, `isStrictlyProper()`, `relativeDegree()`; `poles()`, `zeros()`, `evaluateScalar(s)`, `evaluate(s)`, `dcGain()`; `minreal(tol)`; `operator<<` | SISO transfer function with monic denominator | 2.2, 3.4 |
| `StateSpace tf2ss(const TransferFunction&)` | Controllable canonical form (order 0 permitted) | 2.4 |
| `TransferFunction ss2tf(const StateSpace&)` | Faddeev–LeVerrier, SISO | 2.4 |
| `Polynomial charPoly(const Matrix& A)`; `StateSpace similarityTransform(sys, T)`; `StateSpace toStateSpace(const LTISystem&)` | Characteristic polynomial; change of coordinates; generic conversion | 2.4 |
| `enum class DiscretizationMethod { ZOH, Tustin, ForwardEuler, BackwardEuler, MatchedPoleZero }` | Method selector | 5.1 |
| `StateSpace c2d(const StateSpace&, Ts, method = ZOH, prewarp_w = 0)`; `TransferFunction c2d(const TransferFunction&, Ts, method = Tustin, prewarp_w = 0)` | Discretization | 5.1, 5.2 |
| `StateSpace d2c(const StateSpace&, method = ZOH)`; `TransferFunction d2c(const TransferFunction&, method = Tustin)` | Inverse discretization | 5.1, 5.2 |
| `series(first, second)`, `parallel(a, b)`, `feedback(G, H, sign = −1)`, `feedback(G, sign = −1)` for `TransferFunction` and for `StateSpace`; `operator*`, `operator+` on `TransferFunction` | Interconnections | 3.3 |
| `struct GangOfFour { S, T, CS, PS }`; `GangOfFour gangOfFour(C, P)` | Sensitivity functions of a unity loop | 3.3 |
| `class NonlinearSystem`: `(Dynamics f, Output h, n, m, p)`; `derivative(t, x, u)`, `output(x, u)`; `order() inputs() outputs()`; `linearize(xe, ue, t = 0)`; `findEquilibrium(xGuess, ue, tol)`; `static fromLinear(const StateSpace&)` | Nonlinear model with numerical linearization | 7.4 |

### 8.3 interfaces

**Table 8-3 — Abstract bases (`include/csl/interfaces/`)**

| Declaration | Purpose | WP |
|---|---|---|
| `enum class control::MeasurementKind { Output, State }`; `class control::Controller` (Listing 3-4) | Controller contract | 4.1 |
| `class estimation::Estimator` (Listing 3-5) | Estimator contract | 6.3 |
| `class dsp::DigitalFilter` (Listing 3-5) | Filter contract | 5.5 |

### 8.4 simulation

**Table 8-4 — `csl::simulation`**

| Declaration | Purpose | WP |
|---|---|---|
| `struct SimResult { t, x, y, u; samples(); sampleRate(); channel(j) }` | Time-domain result (Listing 3-3) | 1.7, 5.4 |
| `using Dynamics = std::function<Vector(Real, const Vector&)>`; `enum class Integrator { Euler, RK4 }` | Integrator inputs | 1.7 |
| `Vector eulerStep(f, t, x, dt)`; `Vector rk4Step(f, t, x, dt)`; `SimResult integrate(f, x0, t0, tf, dt, method = RK4)` | Fixed-step integration | 1.7 |
| `SimResult step(sys, tf = −1, dt = −1, inputIndex = 0)`; `SimResult impulse(sys, …)`; `SimResult lsim(sys, const Matrix& u, const Vector& t, x0 = {})`; `void autoHorizon(sys, Real& tf, Real& dt)` | Time responses (ZOH between samples; exact recursion for discrete systems; discrete impulse from `u[0] = 1`) | 2.6 |
| `struct StepInfo { steadyState, riseTime, peakTime, peak, overshootPercent, settlingTime, settlingThreshold }`; `StepInfo stepInfo(const SimResult&, j = 0, thr = 0.02)`; `StepInfo stepInfo(sys, j, thr)` | Step specifications | 2.7 |
| `class Simulator`: `(StateSpace plant, unique_ptr<Controller>)`; `(NonlinearSystem plant, unique_ptr<Controller>)`; `setReference(Signal)`, `setInputDisturbance(Signal)`, `setOutputDisturbance(Signal)`, `setPlantSubsteps(int)`, `setEstimator(unique_ptr<Estimator>)`, `setMeasurementNoise(std, seed)`, `setProcessNoise(Qd, seed)`, `setMeasurementFilter(unique_ptr<DigitalFilter>)`; `SimResult run(x0, tf)`; move-only | Sampled-data closed-loop simulator with contract validation | 4.2, 5.7, 6.3, 7.4, 8.1 |
| `struct MonteCarloResult { runs, t, yMean, yStd, uMean, uStd }`; `MonteCarloResult monteCarlo(makeSimulator, x0, tf, runs, baseSeed)` | Repeated runs with independent noise | 8.1 |

### 8.5 analysis

**Table 8-5 — `csl::analysis`**

| Declaration | Purpose | WP |
|---|---|---|
| `struct FrequencyResponse { w, H, Ts }` (Listing 3-3); `struct BodeData { w, magnitude_dB, phase_deg }` | Frequency-domain results | 3.1 |
| `Vector defaultFrequencies(sys, n = 400)`; `FrequencyResponse frequencyResponse(sys, w)`; `frequencyResponse(sys)`; `BodeData bode(const FrequencyResponse&)` | Frequency response and Bode data | 3.1 |
| `struct Margins { optional gainMargin_dB, phaseCrossover_w, phaseMargin_deg, gainCrossover_w, delayMargin_s; optional<bool> closedLoopStable }`; `Margins margins(sys)`; `Margins margins(const FrequencyResponse&)` | Stability margins from a model or from data | 3.2 |
| `struct NyquistData { w, re, im }`; `NyquistData nyquist(const FrequencyResponse&)` | Nyquist curve | 3.2 |
| `struct RootLocusData { gains, branches, openLoopPoles, openLoopZeros }`; `rootLocus(L, gains)`; `rootLocus(L)`; `asymptoteAngles_deg(L)`; `asymptoteCentroid(L)`; `stabilityBoundaryCrossings(rl)` | Root locus | 3.5 |
| `bool isMinimumPhase(sys)`; `Matrix routhArray(p)`; `int routhRHPRootCount(p)`; `bool routhIsStable(p)`; `bool isLyapunovStable(A)` | Stability tests | 3.4, 3.6, 6.4 |
| `struct SensitivityPeak { Ms, w, impliedGM_dB, impliedPM_deg }`; `sensitivityPeak(L)`; `Real bandwidth(T, dropDB = −3)`; `Real sensitivityIntegral(L, wMax, n)` | Sensitivity analysis | 4.6 |
| `Real returnDifferenceMin(L, w)` | `min|1 + L(jω)|` on a grid | 7.3 |
| `Real h2norm(const StateSpace&)`; `Real hinfnorm(const StateSpace&, tol = 1e-6)`; `Real hinfnorm(const FrequencyResponse&)`; `Real sigmaMax(const ComplexMatrix&)`; `peakFrequency(…)` | System norms | 6.4, 10.2 |
| `Vector relativeErrorEnvelope(nominal, family, w)`; `TransferFunction delayUncertaintyWeight(tauMax, gain = 2.2)`; `struct RobustnessReport { … }`; `RobustnessReport robustness(L, Wm, Ws, w)` | Uncertainty weights and small-gain tests | 10.1 |

### 8.6 control

**Table 8-6 — `csl::control`**

| Declaration | Purpose | WP |
|---|---|---|
| `class PID final : Controller`: `struct Config { Kp, Ki, Kd, N, Ts, uMin, uMax, derivativeOnMeasurement, setpointWeight, antiWindup, trackingTime }`; `explicit PID(const Config&)`; `compute(…)`, `Real compute(y, r)`, `reset()`; `toTransferFunction()`; `derivativeTimeConstant()`; `config()` | Practical PID | 4.1, 4.3 |
| `class Relay final : Controller`: `(amplitude, hysteresis, Ts)` | Relay for auto-tuning | 4.4 |
| `namespace tuning`: `struct FOPDT { K, L, T; model(padeOrder) }`; `FOPDT fitFOPDT(const SimResult&, j, stepAmplitude)`; `struct UltimatePoint { Ku, Tu }`; `ultimateFromModel(sys)`; `relayExperiment(plant, d, Ts, tf, hysteresis)`; `enum class Rule { ZN_Step_PID, ZN_Step_PI, ZN_Ultimate_PID, ZN_Ultimate_PI, SIMC_PI }`; `PID::Config tune(Rule, const FOPDT&, Ts, tauC)`; `tune(Rule, const UltimatePoint&, Ts)` | Tuning rules | 4.4 |
| `TransferFunction lead(z, p, K)`; `lag(z, p, K)`; `struct ErrorConstants { Kp, Kv, Ka, systemType }`; `errorConstants(L)`; `struct LeadDesign { C, z, p, K, phiMax_deg, wm, achievedPM_deg, achievedGM_dB }`; `designLead(P, K, targetPM, safety)`; `designLag(P, K, targetPM, decadeBelow)` | Lead/lag compensation | 4.5 |
| `class TransferFunctionController final : Controller`: `explicit (TransferFunction discreteH)` | Any discrete SISO transfer function as a controller on `e = r − y` | 5.2 |
| `Matrix place(A, B, poles)`; `place(const StateSpace&, poles)`; `Real feedforwardGain(sys, K)`; `class StateFeedbackController final : Controller` `(K, Nbar, Ts)`, `gain()` | Pole placement (SISO Ackermann) and state feedback | 6.2 |
| `struct FeedforwardGains { Nx, Nu, Nbar }`; `feedforwardGains(sys, K)`; `StateSpace augmentWithIntegrator(sys)`; `class IntegralStateFeedbackController final : Controller` with `Config { K, Ki, C, Ts, uMin, uMax, antiWindup }` | Tracking with feedforward or integral action | 6.5 |
| `struct LQRResult { K, P, closedLoopPoles }`; `lqr(A, B, Q, R)`; `lqr(const StateSpace&, Q, R)`; `Real lqrCost(P, x0)` | Continuous LQR with preconditions | 7.1 |
| `struct TimeVaryingGains { K, P }`; `finiteHorizonLQR(A, B, Q, R, QN, N)`; `dlqr(A, B, Q, R)`; `dlqr(const StateSpace& sysd, Q, R)` | Discrete LQR | 7.2 |
| `Matrix brysonWeights(const Vector& maxValues)`; `TransferFunction lqrLoopTransfer(sys, K)`; `struct IntegralLQRResult { K, Ki, P }`; `lqrIntegral(sys, Q, Qi, R)` | LQR tuning, loop transfer, integral LQR | 7.3 |
| `struct GridDP { axes, actions, step, stageCost, discount }`; `struct DPResult { V, policy, iterations, lastChange }`; `valueIteration(const GridDP&, tol, maxIter)`; `Real interpolate(const GridDP&, V, x)` | Grid dynamic programming | 7.5 |
| `class LQGController final : Controller` with `Config { Qx, Ru, Qw, Rv, x0, P0, steadyState, Nbar }`; `filter()`, `gain()`; `StateSpace lqgCompensator(sysd, K, L)` | LQG | 8.3 |
| `class MPC final : Controller` with `Config { N, Nu, Q, R, QN, uMin, uMax, duMin, duMax, xMin, xMax, ySoftMin, ySoftMax, slackWeight, warmStart }`; `struct QP { H, f, A, b }`; `buildQP(x0, xRef)`; `predictionF()`, `predictionPhi()`; `lastSolve()` | Model predictive control | 10.4, 10.5 |
| `struct iLQRProblem { f, l, lN, n, m, N, Ts }`; `struct iLQRResult { x, u, K, costHistory, converged, iterations }`; `ilqr(problem, x0, uInit, maxIter, tol, muInit)` | Iterative LQR | 10.6 |
| `struct GeneralizedPlant { P, nw, nu, nz, ny }`; `disturbanceRejectionPlant(G, Ws, Wt, rho = 1)`; `void checkRegularity(const GeneralizedPlant&)`; `struct HInfResult { K, gamma, Xinf, Yinf, ok, message }`; `hinfsynFixedGamma(P, gamma)`; `hinfsyn(P, gammaLo, gammaHi, tol)`; `StateSpace closedLoop(P, K)` | H∞ synthesis | 10.7 |

### 8.7 estimation

**Table 8-7 — `csl::estimation`**

| Declaration | Purpose | WP |
|---|---|---|
| `class LuenbergerObserver final : Estimator`: `(StateSpace discretePlant, Matrix L)`; `static designGain(discretePlant, zPoles)` | Current-estimator observer | 6.3 |
| `class KalmanFilter : Estimator`: `(discretePlant, Q, R, x0, P0)`; `reset(x0, P0)`; `innovation()`, `innovationCovariance()`, `gain()`, `nis()`; time-varying `C` setter | Linear Kalman filter, Joseph form | 8.2, 9.2 |
| `struct SteadyStateKF { Pminus, Pplus, K, poles }`; `steadyStateKalman(sysd, Q, R)` | Steady-state gain by DARE | 8.3 |
| `class ExtendedKalmanFilter final : Estimator` with `Config { f, h, Q, R, x0, P0, optional F, optional H, innovationFix }`; `static discretize(const NonlinearSystem&, Ts)` | EKF with optional analytic Jacobians | 8.4 |
| `class UnscentedKalmanFilter final : Estimator` with `Config { f, h, Q, R, x0, P0, alpha, beta, kappa, innovationFix }` | UKF | 8.5 |

### 8.8 dsp

**Table 8-8 — `csl::dsp`**

| Declaration | Purpose | WP |
|---|---|---|
| `bool isPowerOfTwo(n)`; `Index nextPowerOfTwo(n)`; `Index bitReverse(i, bits)`; `void fft(ComplexVector&)`; `void ifft(ComplexVector&)`; `ComplexVector fft(const Vector&, nfft = −1)`; `rfft(const Vector&, nfft)`; `Vector fftFrequencies(nfft, fs_hz)`; `rfftFrequencies(nfft, fs_hz)`; `ComplexVector dft(const Vector&)` | FFT family and test oracle | 5.3 |
| `enum class Window { Rectangular, Hann, Hamming, Blackman }`; `Vector window(Window, N)` | Window functions | 5.4 |
| `struct Spectrum { f_hz, P }`; `struct CrossSpectrum { f_hz, Pxy, Pxx, Pyy }`; `periodogram(x, fs_hz, window, nfft)`; `welch(x, fs_hz, nperseg, overlap, window)`; `csd(x, y, fs_hz, nperseg, overlap, window)`; `Real integratePSD(const Spectrum&)` | Spectral estimation | 5.4 |
| `struct ETFEResult { H, coherence, f_hz }`; `etfe(u, y, fs_hz, nperseg, overlap, window)` | Empirical transfer-function estimate | 9.3 |
| `class IIRFilter final : DigitalFilter`: `(b, a, Ts)`, `explicit (const TransferFunction&)`; `class FIRFilter final : DigitalFilter`: `(h, Ts)`, `groupDelaySamples()`; both with `using DigitalFilter::process` | Filter structures (DF2T; circular delay line) | 5.5 |
| `Vector filtfilt(const DigitalFilter&, x)`; `FIRFilter movingAverage(N, Ts)`; `IIRFilter exponentialSmoothing(alpha, Ts)` | Zero-phase filtering; elementary filters | 5.5 |
| `enum class FilterType { Lowpass, Highpass }`; `butterworthPoles(order, wc)`; `butterworthAnalog(order, wc, type)`; `int butterworthOrder(wp, ws, Ap_dB, As_dB)`; `IIRFilter butterworth(order, fc_hz, fs_hz, type)`; `FIRFilter firWindowedSinc(numTaps, fc_hz, fs_hz, window, type)`; `IIRFilter notch(f0_hz, bandwidth_hz, fs_hz)` | Filter design | 5.6 |

### 8.9 sysid

**Table 8-9 — `csl::sysid`**

| Declaration | Purpose | WP |
|---|---|---|
| `struct ARXModel { a, b, na, nb, nk, Ts, noiseVariance, conditionNumber; toTransferFunction(); predictOneStep(y, u); simulate(u) }`; `arx(y, u, na, nb, nk, Ts)`; `Matrix arxRegressor(y, u, na, nb, nk)` | ARX least squares | 9.1 |
| `Real fitPercent(y, yhat)`; `splitData(u, y, fraction)`; `Vector autocorrelation(x, maxLag)`; `crossCorrelation(x, y, maxLag)`; `struct ValidationReport { fitPercent, residualAutocorr, residualInputXcorr, band, residualsWhite, residualsIndependent }`; `validate(model, u, y, maxLag = 25)` | Validation | 9.1, 9.4 |
| `class RLS`: `(nParams, lambda = 1, P0 = 1e4)`; `update(y, phi)`; `theta()`, `covariance()`, `lastResidual()`, `reset(P0)`; `Matrix rlsARX(y, u, na, nb, nk, lambda, P0)` | Recursive least squares | 9.2 |
| `struct RationalFitOptions { numOrder, denOrder, skIterations, minCoherence }`; `TransferFunction fitRational(const FrequencyResponse&, options, weights = {})`; `tuning::FOPDT fitFOPDTfromFRF(const FrequencyResponse&)` | Frequency-domain identification | 9.3 |
| `markovParametersFromImpulse(h, p, m)`; `markovParametersByCorrelation(u, y, K)`; `struct ERAResult { sys, singularValues, order }`; `era(h, r, order, Ts)` | Eigensystem realisation | 9.4 |

### 8.10 signals

**Table 8-10 — `csl::signals`**

| Declaration | Purpose | WP |
|---|---|---|
| `Vector timeGrid(t0, tf, dt)`; `step(t, amplitude, tStart)`; `ramp(t, slope, tStart)`; `sine(t, amplitude, w, phase)`; `square(t, amplitude, period)`; `chirp(t, f0_hz, f1_hz, amplitude)` | Deterministic signals | 2.7 |
| `class WhiteNoise`: `(stdDev, seed)`; `sample()`, `sample(n)`, `reseed(seed)`; `class MultivariateNormal`: `(Sigma, seed)`; `sample()`; `class PRBS`: `(bits, amplitude, seed)`; `next()`, `sample(n)`, `period()` | Random and pseudo-random signals | 8.1 |

### 8.11 plot

**Table 8-11 — `csl::plot`**

| Declaration | Purpose | WP |
|---|---|---|
| `void writeCSV(path, headers, columns)`; `void writeCSV(path, headers, const Vector& t, const Matrix& data)` | CSV output | 1.5, 1.6 |
| `class Figure`: `explicit (title)`; `line(x, y, label)`, `scatter(x, y, label, marker)`, `hline(y, label)`, `vline(x, label)`, `xlabel`, `ylabel`, `logX`, `logY`, `xlim`, `ylim`, `size`; `std::string svg()`; `save(htmlPath)`; `static savePage(htmlPath, figures)` | SVG/HTML figures; `line` accepts Eigen expressions | 2.5, 5.6 |
| `plotOutputs(const SimResult&, title)`; `plotStates(const SimResult&, title)`; `plotBode(responses, path)`; `plotNyquist(fr, path)`; `plotRootLocus(rl, path)`; `plotPoleZero(sys, path)`; `plotGangOfFour(g4, path)`; `plotSpectrum(spectrum, path)` | Control-specific plots | 2.5, 3.1, 3.2, 3.5, 4.6, 5.4 |

---
## 9. Algorithm Specifications

The equations the library implements, numbered for reference from Chapter 7 and Chapter 8. Derivations and C++ listings are in the Design Document V4; this chapter is the specification the implementation must satisfy.

### 9.1 Integration (WP 1.2, 1.7)

```
Euler:  x_{k+1} = x_k + Δt f(t_k, x_k);   stable on ẏ = λy iff |1 + λΔt| < 1                       (9-1)
RK4:    k₁ = f(t, x), k₂ = f(t + Δt/2, x + Δt k₁/2), k₃ = f(t + Δt/2, x + Δt k₂/2), k₄ = f(t + Δt, x + Δt k₃)
        x_{k+1} = x_k + (Δt/6)(k₁ + 2k₂ + 2k₃ + k₄);   global error O(Δt⁴)                          (9-2)
```

### 9.2 Linear Systems (WP 1.8, 2.2–2.4)

```
dc gain:   G(0) = D − CA⁻¹B (continuous);   G(1) = D + C(I − A)⁻¹B (discrete)                      (9-3)
transfer:  H(s) = C(sI − A)⁻¹B + D                                                                 (9-4)
companion matrix of sⁿ + a₁sⁿ⁻¹ + … + aₙ:  first row [−a₁ … −aₙ], ones on the subdiagonal           (9-5)
controllable canonical form of (b₀sⁿ + … + bₙ)/(sⁿ + a₁sⁿ⁻¹ + … + aₙ):
   A: ones on the superdiagonal, last row [−aₙ … −a₁];  B = eₙ;  C = [bₙ − b₀aₙ … b₁ − b₀a₁];  D = b₀  (9-6)
Faddeev–LeVerrier:  M₁ = I, a₁ = −tr(AM₁);  M_k = AM_{k−1} + a_{k−1}I, a_k = −tr(AM_k)/k;
   numerator coefficient of s^{n−k} is CM_kB + Da_k;  M_{n+1} = 0 (Cayley–Hamilton)                  (9-7)
transmission zeros (SISO): finite generalized eigenvalues of ([[A, B], [C, D]], diag(I, 0))         (9-8)
```

### 9.3 Time Responses and Specifications (WP 2.6, 2.7)

Step: `u = 1`, `x₀ = 0`. Impulse (continuous, strictly proper): zero input from `x₀ = B`; (discrete): `u[0] = 1`, `x₀ = 0`, so `y[0] = D`, `y[k] = CA^{k−1}B`. `lsim`: input held over each interval, RK4 within intervals; exact recursion for discrete systems. Specifications: rise time 10–90 %; peak time; overshoot `(y_max − y_ss)/y_ss`; 2 % settling time; second-order approximations `t_p = π/ω_d`, `M_p = exp(−πζ/√(1−ζ²))`, `t_s ≈ 4/(ζω_n)`.

### 9.4 Frequency Domain (WP 3.1–3.6, 4.6)

```
frequency response:  H(jω) (continuous);  H(e^{jωT_s}) (discrete, ω < π/T_s)                      (9-9)
margins:  GM = 1/|L(jω_pc)| with ∠L(jω_pc) = −180°;  PM = 180° + ∠L(jω_gc) with |L(jω_gc)| = 1;
          delay margin = PM[rad]/ω_gc;  phase crossover located as a sign change of g = wrapToPi(φ + π)
          between consecutive samples with |Δg| < π                                                  (9-10)
feedback:  G/(1 + GH);  TF: num = N_G D_H, den = D_G D_H + N_G N_H;  SS: E = I + DD_h invertible     (9-11)
gang of four:  S = 1/(1+L), T = L/(1+L), CS = C/(1+L), PS = P/(1+L);  S + T = 1                      (9-12)
Padé:  e^{−sT} ≈ P_n(−sT)/P_n(sT),  P_n(x) = Σ_{k=0}^{n} [(2n−k)! n!]/[(2n)! k! (n−k)!] x^k           (9-13)
root locus:  roots of D(s) + K N(s) for K ∈ [0, ∞);  asymptotes (2k+1)180°/(n−m) from (Σp − Σz)/(n−m) (9-14)
Routh:  b_i = (a₁a_{2i} − a₀a_{2i+1})/a₁, …;  RHP roots = sign changes in the first column            (9-15)
peak sensitivity:  M_s = max|S|;  GM ≥ M_s/(M_s − 1),  PM ≥ 2 asin(1/(2M_s));  ∫₀^∞ ln|S| dω = π Σ Re p_unstable (9-16)
```

### 9.5 Classical Control (WP 4.1–4.5)

```
PID (discrete):  e_k = r_k − y_k;  I_k = I_{k−1} + K_i T_s e_k;  D_k = (T_f D_{k−1} − K_d(y_k − y_{k−1}))/(T_f + T_s);
                 u_k = clamp(K_p(b r_k − y_k) + I_k + D_k, u_min, u_max);  T_f = K_d/(K_p N)             (9-17)
PID (continuous, for analysis):  C(s) = [(K_pT_f + K_d)s² + (K_p + K_iT_f)s + K_i]/[T_f s² + s]           (9-18)
anti-windup:  clamping — hold I when saturated in the direction of e;  back-calculation — I_k += (T_s/T_t)(u_sat − u_unsat),
              T_t = √(T_iT_d) or T_i                                                                       (9-19)
tuning:  ZN step PID K_p = 1.2T/(KL), T_i = 2L, T_d = L/2;  ZN ultimate PID K_p = 0.6K_u, T_i = T_u/2, T_d = T_u/8;
         SIMC PI K_p = T/(K(τ_c + L)), T_i = min(T, 4(τ_c + L));  relay K_u ≈ 4d/(πa)                     (9-20)
lead:  C = K(s+z)/(s+p), α = p/z;  φ_max = asin((α−1)/(α+1)) at ω_m = √(zp) where |C/K| = +10 log₁₀α dB;
       procedure: K from error constant → φ needed + safety → α → ω_m from |KP| = −10 log₁₀α → z, p       (9-21)
```

### 9.6 Discrete Time and DSP (WP 5.1–5.6)

```
ZOH:  [[A_d, B_d], [0, I]] = exp([[A, B], [0, 0]] T_s);  z = e^{sT_s};  d2c by the matrix logarithm      (9-22)
maps:  forward Euler s = (z−1)/T_s;  backward Euler s = (z−1)/(T_sz);  Tustin s = a(z−1)/(z+1), a = 2/T_s
       or a = ω₀/tan(ω₀T_s/2) (prewarp);  MPZ: z = e^{sT_s} per pole and zero, zeros at −1 to relative
       degree 1, dc gain matched                                                                           (9-23)
Tustin on polynomials:  Σ p_i s^{d−i} → Σ p_i a^{d−i}(z−1)^{d−i}(z+1)^{n−d+i}                              (9-24)
DFT:  X[k] = Σ x[n]e^{−j2πkn/N};  bins f_k = kf_s/N;  FFT: bit-reversal permutation then log₂N butterfly
      stages (u, v·W) → (u + vW, u − vW), W = e^{−j2π/len};  IFFT by conjugation and 1/N                 (9-25)
periodogram (one-sided):  P[k] = 2|X[k]|²/(f_s Σw²) for 0 < k < N/2;  Welch: average over overlapping
      windowed segments;  cross-spectrum uses conj(X)·Y                                                    (9-26)
DF2T:  y = b₀x + s₁;  s_i = b_ix − a_iy + s_{i+1};  s_n = b_nx − a_ny                                       (9-27)
Butterworth:  poles ω_c e^{j[π/2 + (2k−1)π/(2n)]}, k = 1…n;  order n ≥ log₁₀[(10^{A_s/10}−1)/(10^{A_p/10}−1)]/(2log₁₀(ω_s/ω_p));
      digital filter = prewarped Tustin of the prototype                                                   (9-28)
FIR windowed sinc:  h[n] = sin(ω_c(n − M/2))/(π(n − M/2)) · w[n];  notch: (1 − 2cosω₀z⁻¹ + z⁻²)/(1 − 2r cosω₀z⁻¹ + r²z⁻²),
      r = e^{−πBW/f_s}, dc gain normalised to 1                                                            (9-29)
```

### 9.7 State-Space Design (WP 6.1–6.5)

```
controllability:  rank[B, AB, …, Aⁿ⁻¹B] = n;  PBH: rank[λI − A, B] = n ∀λ;  dual for observability     (9-30)
Ackermann:  K = eₙᵀ W_c⁻¹ Δ_d(A);  N̄ = −1/(C(A − BK)⁻¹B)                                                  (9-31)
observer (current form):  x̄ = A_dx̂ + B_du;  x̂ = x̄ + L(y − Cx̄);  e_{k+1} = (I − LC)A_d e_k;
      L = place(A_dᵀ, (CA_d)ᵀ, poles)ᵀ;  closed-loop poles = eig(A − BK) ∪ eig(A − LC)                    (9-32)
Lyapunov:  AX + XAᵀ + Q = 0 via (I⊗A + A⊗I)vec X = −vec Q;  AXAᵀ − X + Q = 0 via (A⊗A − I)vec X = −vec Q;
      W_c = lyap(A, BBᵀ), W_o = lyap(Aᵀ, CᵀC)                                                             (9-33)
H2 norm:  ‖G‖₂² = tr(BᵀW_oB) (continuous, D = 0);  = tr(BᵀW_oB) + tr(DᵀD) (discrete)                     (9-34)
feedforward:  [[A, B], [C, D]][N_x; N_u] = [0; I];  u = −K(x − N_xr) + N_ur;  integral augmentation
      [ẋ; ẋ_I] = [[A, 0], [−C, 0]][x; x_I] + [B; 0]u + [0; I]r                                            (9-35)
```

### 9.8 Optimal Control (WP 7.1–7.5)

```
CARE:  AᵀP + PA − PBR⁻¹BᵀP + Q = 0,  K = R⁻¹BᵀP;  Hamiltonian H = [[A, −S], [−Q, −Aᵀ]], S = BR⁻¹Bᵀ;
       P = X₂X₁⁻¹ from the stable eigenvectors [X₁; X₂];  verify residual, symmetry, A − SP stable        (9-36)
finite horizon:  K_k = (R + BᵀP_{k+1}B)⁻¹BᵀP_{k+1}A,  P_k = Q + AᵀP_{k+1}(A − BK_k),  P_N = Q_N            (9-37)
DARE:  P = Q + AᵀPA − AᵀPB(R + BᵀPB)⁻¹BᵀPA by iterating (9-37) to convergence                            (9-38)
return difference (SISO, loop at plant input):  |1 + L(jω)| ≥ 1 ⇒ GM ∈ [1/2, ∞), PM ≥ 60°                  (9-39)
linearization:  A = ∂f/∂x, B = ∂f/∂u at (x_e, u_e), central differences with h = ∛ε·max(1, |x_i|)          (9-40)
Bellman:  V(x) = min_u[l(x, u) + V(f(x, u))];  value iteration on a grid with bilinear interpolation       (9-41)
```

### 9.9 Estimation (WP 8.1–8.5)

```
noise:  Q_d = ∫₀^{T_s} e^{Aτ}Q_c e^{Aᵀτ}dτ (augmented exponential);  v = Lz with LLᵀ = Σ                   (9-42)
Kalman:  x̂⁻ = Ax̂ + Bu, P⁻ = APAᵀ + Q;  S = CP⁻Cᵀ + R, K = P⁻CᵀS⁻¹, x̂ = x̂⁻ + K(y − Cx̂⁻),
         P = (I − KC)P⁻(I − KC)ᵀ + KRKᵀ;  NIS = νᵀS⁻¹ν with mean p                                        (9-43)
steady state:  P⁻ solves (9-38) with (A, B, Q, R) → (Aᵀ, Cᵀ, Q, R);  LQG: u = −K_lqr x̂                    (9-44)
EKF:  F = ∂f_d/∂x|x̂, H = ∂h/∂x|x̂⁻, f_d = one RK4 step;  otherwise (9-43)                                 (9-45)
UKF:  χ₀ = x̂, χ_i = x̂ ± (√((n+λ)P))_i, λ = α²(n+κ) − n;  W_m⁰ = λ/(n+λ), W_c⁰ = W_m⁰ + 1 − α² + β,
      W^i = 1/(2(n+λ));  K = P_xyP_yy⁻¹                                                                    (9-46)
```

### 9.10 Identification (WP 9.1–9.4)

```
ARX:  y[k] + Σa_iy[k−i] = Σb_iu[k−n_k−i+1] + e[k];  θ̂ = argmin‖y − Φθ‖² by QR;  fit = 100(1 − ‖y − ŷ‖/‖y − ȳ‖)  (9-47)
RLS:  K = Pφ/(λ + φᵀPφ);  θ̂ += K(y − φᵀθ̂);  P = (P − KφᵀP)/λ;  λ = 1 and P₀ = p₀I equals (ΦᵀΦ + I/p₀)⁻¹Φᵀy  (9-48)
ETFE:  Ĥ = P_uy/P_uu;  γ² = |P_uy|²/(P_uuP_yy);  rational fit min Σw_k|N(jω_k) − Ĥ_kD(jω_k)|², D monic,
       SK reweighting w_k = 1/|D_prev(jω_k)|²                                                             (9-49)
ERA:  H₀ = [h_{i+j−1}], H₁ = [h_{i+j}], H₀ = UΣVᵀ truncated to n;  A = Σ^{−½}UᵀH₁VΣ^{−½}, B = first m columns
      of Σ^{½}Vᵀ, C = first p rows of UΣ^{½}, D = h₀                                                      (9-50)
```

### 9.11 Robust, Predictive and Nonlinear Optimal Control (WP 10.1–10.7)

```
uncertainty:  P̃ = P(1 + W_mΔ), ‖Δ‖∞ ≤ 1;  delay weight W_m = 2.2τ_max s/(τ_max s + 1) ≥ 2|sin(ωτ_max/2)|;
      robust stability ‖W_mT‖∞ < 1;  robust performance max_ω(|W_sS| + |W_mT|) < 1                      (9-51)
H∞ norm:  ‖G‖∞ < γ iff H_γ = [[A + BR⁻¹DᵀC, BR⁻¹Bᵀ], [−Cᵀ(I + DR⁻¹Dᵀ)C, −(A + BR⁻¹DᵀC)ᵀ]], R = γ²I − DᵀD,
      has no imaginary-axis eigenvalue;  bisection on γ                                                   (9-52)
QP:  min ½xᵀHx + fᵀx s.t. Ax ≤ b, A_eqx = b_eq;  KKT: Hx + f + Aᵀλ + A_eqᵀμ = 0, λ ≥ 0, λ_i(Ax − b)_i = 0;
     Hildreth: x_u = −H⁻¹f, P = AH⁻¹Aᵀ, d = Ax_u − b, λ_i ← max(0, (d_i − Σ_{j≠i}P_ijλ_j)/P_ii), x = x_u − H⁻¹Aᵀλ;
     equalities eliminated by x = x_p + Nz, N = null(A_eq)                                                (9-53)
MPC:  X = Fx₀ + ΦU, F = [A; …; A^N], Φ_ij = A^{i−j}B (i ≥ j);  H = 2(ΦᵀQ̄Φ + R̄), f = 2ΦᵀQ̄(Fx₀ − X_ref),
      Q̄ = diag(Q, …, Q_N);  constraints linear in U;  soft constraints with slack ε ≥ 0 penalised         (9-54)
iLQR:  linearise and quadratise along the nominal trajectory;  backward pass gives k_k, K_k;  forward pass
      u_k = ū_k + αk_k + K_k(x_k − x̄_k) with backtracking α;  regularise with μI when needed               (9-55)
H∞ synthesis (regular problem, D₁₁ = D₂₂ = 0, D₁₂ᵀ[C₁ D₁₂] = [0 I], D₂₁[B₁ᵀ D₂₁ᵀ] = [0 I]):
      X∞ = solveARE(A, B₂B₂ᵀ − γ⁻²B₁B₁ᵀ, C₁ᵀC₁);  Y∞ = solveARE(Aᵀ, C₂ᵀC₂ − γ⁻²C₁ᵀC₁, B₁B₁ᵀ);
      require X∞ ⪰ 0, Y∞ ⪰ 0, ρ(X∞Y∞) < γ²;  F = −B₂ᵀX∞, L = −Y∞C₂ᵀ, Z = (I − γ⁻²Y∞X∞)⁻¹,
      Â = A + γ⁻²B₁B₁ᵀX∞ + B₂F + ZLC₂;  K: x̂̇ = Âx̂ − ZLy, u = Fx̂;  plant: w = [d; n], y = G(u + d) + n,
      z = [W_sG(u + d); ρu; W_tG(u + d)] with G strictly proper                                          (9-56)
```

---

## 10. Testing Strategy

### 10.1 Principles

T1–T7 of Section 4.5 apply. Cross-method identities (T4) are tested deliberately because they verify that modules built on different days agree: the stability limit `K = 6` of `1/(s(s+1)(s+2))` from margins, root locus and Routh; the LQR cost versus a simulated integral; the Kalman covariance versus the DARE; unconstrained MPC versus `dlqr`; iLQR versus finite-horizon LQR on a linear problem; the H∞ controller versus LQG as `γ → ∞`.

### 10.2 Oracles by Module

**Table 10-1 — Test oracles by module**

| Module | Oracles |
|---|---|
| core | closed-form polynomial algebra; companion roots versus known roots; `kron`/`vec` identities; Lyapunov, ARE, DARE residuals and stabilizing checks; KKT residuals; hand-solved QPs |
| systems | cross-representation `evaluate`; `ss2tf(tf2ss(H)) = H` including order 0; similarity invariance; interconnection identities; exact ZOH step; `d2c(c2d) = id` |
| simulation | analytic responses of E1 and E2; order-of-accuracy ratios; specifications versus second-order formulas; Simulator versus transfer-function closed loop |
| analysis | reference margins (Table 5-3); agreement of Nyquist, root locus and Routh; `S + T = 1`; sensitivity integral; `hinfnorm` versus closed form and grid; small gain versus delay margin |
| control | PID algebra; tuning tables; LQR reference gain and cost; LQR margin bounds; separation-principle pole sets; MPC equals `dlqr` unconstrained; iLQR equals LQR on linear problems; H∞ → LQG limit; `hinfnorm(T_zw) ≤ γ` |
| estimation | Kalman covariance versus DARE; EKF equals KF on linear plants; unscented transform exact on linear and quadratic maps; RMSE and NIS bounds with seeds |
| dsp | `fft` versus `dft`; impulse and constant spectra; Parseval; window tables; white-noise PSD level; filter responses at known frequencies; `process(step)` equals `step(toTransferFunction())` |
| sysid | exact recovery from noise-free data; bounded error with noise; RLS equals regularised batch and equals the Kalman filter; ERA singular-value gap; whiteness on correct versus under-fitted models |
| signals, plot | statistics with seeds; PRBS period, balance and autocorrelation; SVG structure; CSV round trip |

### 10.3 Experiments

Comparisons whose outcome is not guaranteed by theory are experiments (T6): they record both values and are reviewed, not asserted. In the current plan these are: Kalman filter versus a faster pole-placed observer (ordering follows from optimality and is asserted in the mean); UKF versus EKF convergence and range–bearing accuracy; MPC versus DP and saturated LQR cost (the DP comparison asserts closeness because both approximate the same optimum).

---

## 11. Extensions

**Table 11-1 — Extensions beyond the final product**

| Extension | Builds on | Content |
|---|---|---|
| Adaptive Runge–Kutta (Dormand–Prince) | WP 1.7 | `rk45Step`, `integrateAdaptive` with error control |
| Bartels–Stewart Lyapunov solver | WP 6.4 | `Eigen::RealSchur` with quasi-triangular back-substitution |
| Ordered-Schur Riccati solver | WP 7.1 | Schur-form reordering; more robust than the eigenvector method [R32] |
| DARE by doubling or symplectic pencil | WP 7.2 | quadratic convergence |
| MIMO transfer-function matrices and transmission zeros | WP 2.2–2.4 | generalized eigenproblem for non-square systems |
| MIMO pole placement (Kautsky–Nichols) | WP 6.2 | robust eigenstructure assignment |
| Balanced realization and truncation | WP 6.4, 6.6 | model reduction with Hankel-singular-value bounds |
| Second-order-section IIR realisation | WP 5.5–5.6 | numerically robust high-order filters |
| Active-set and interior-point QP; external solver behind `solveQP` | WP 10.3 | [R41]; OSQP |
| Nonlinear MPC with iLQR as the inner solver | WP 10.4–10.6 | real-time iteration |
| Particle filter | WP 8.1, 8.5 | sequential importance resampling |
| Loop transfer recovery | WP 8.3 | recovery of LQR margins |
| μ-analysis | WP 10.1–10.2 | structured singular value bounds |
| General H∞ (reference-tracking mixed sensitivity, loop shifting, scaling); H∞ loop shaping | WP 10.7 | [R23, ch. 14, 16] |
| ARMAX, output-error, instrumental variables, prediction-error method | WP 9.1 | unbiased identification under coloured noise [R19] |
| N4SID / MOESP | WP 9.4 | subspace identification from input–output data [R50] |
| Chebyshev, elliptic, Bessel filters; band transforms; Parks–McClellan FIR | WP 5.6 | remainder of the filter-design toolbox [R29] |
| Describing functions and limit cycles | WP 4.4, 7.4 | relay feedback analysed rigorously |
| Indirect adaptive control | WP 9.2, 6.2 | RLS with online pole placement [R27] |
| Python bindings (pybind11) | WP 11.1 | zero-copy Eigen–NumPy interoperation |
| Real-time profile | WP 4.2 | fixed-size Eigen types; allocation-free `compute` paths |

---

## Appendix A — Glossary

### A.1 C++ and Tooling

| Term | Definition |
|---|---|
| Translation unit; object file; linker | A source file after preprocessing; its compiled code and symbol table; the tool resolving symbols across object files into an executable |
| Static library | An archive of object files linked into an executable |
| Header; `#pragma once`; One Definition Rule | Declarations shared between translation units; guard against double inclusion; one definition per entity (`inline` permits header definitions) |
| CMake target; `PUBLIC`/`PRIVATE`/`INTERFACE` | A buildable library or executable with usage requirements; who inherits a property |
| Value; reference; pointer | A copy; an alias; an address that may be null |
| `const`; `constexpr` | Immutable after initialisation; compile-time evaluated |
| Stack; heap; RAII | Automatic storage; dynamic storage; acquisition in constructors and release in destructors |
| `unique_ptr`; `shared_ptr` | Sole-owner and reference-counted owning pointers |
| Class invariant | A condition true of every valid object, established by the constructor |
| Special member functions; Rule of Zero | Default and copy/move constructors and assignments, destructor; declare none when members manage themselves |
| Move semantics; rvalue | Transfer of a resource from an expiring object; an expression denoting one |
| Inheritance; virtual; `override`; abstract class | Is-a relation; run-time dispatch; checked overriding; a class with a pure virtual function |
| Name hiding | A derived-class declaration hides all base overloads of the same name; `using Base::f;` restores them |
| Template; lambda; `std::function`; functor | Definition instantiated per type; anonymous function with captures; type-erased callable; object with `operator()` |
| Iterator; algorithm | Position in a sequence; function over `[first, last)` |
| `std::optional`; exception; `enum class` | Possibly absent value; error propagated by unwinding; scoped enumeration |
| Expression template | Lazy evaluation of composite Eigen expressions |
| Sanitizer | Run-time instrumentation for memory and undefined-behaviour errors |

### A.2 Control and Signal Processing

| Term | Definition |
|---|---|
| State; input; output; LTI system | Minimal memory; manipulated variable; measured variable; linear time-invariant |
| Pole; zero; stability | Root of the denominator; root of the numerator; all poles in the open left half-plane (continuous) or open unit disc (discrete) |
| Internal versus input–output stability | Stability of a realization including hidden modes; stability of the transfer function after cancellation |
| `ω_n`; `ζ`; `τ`; dc gain | Natural frequency; damping ratio; time constant; steady-state gain, Eq. (9-3) |
| Frequency response; Bode; Nyquist; margins | `H(jω)`; magnitude and phase versus `log ω`; `H(jω)` in the complex plane; Eq. (9-10) |
| `L`, `S`, `T`; bandwidth; `M_s` | Loop, sensitivity, complementary sensitivity, Eq. (9-12); frequency where `|T|` falls to −3 dB; peak of `|S|` |
| Minimum phase; all-pass; delay margin | No RHP zeros or poles; unit magnitude with phase only; delay that destabilises the loop |
| Root locus; Routh–Hurwitz | Closed-loop poles versus gain, Eq. (9-14); stability from coefficients, Eq. (9-15) |
| PID; anti-windup; setpoint weighting | Eq. (9-17); Eq. (9-19); proportional term on `br − y` |
| Lead; lag; error constants | Phase-adding and gain-adding first-order compensators, Eq. (9-21); `K_p`, `K_v`, `K_a` |
| Sampling; ZOH; aliasing; Nyquist frequency | Periodic measurement; holding `u` between samples; frequency folding; `f_s/2` |
| z-transform; Tustin; matched pole–zero | Discrete transform; bilinear map; pole–zero mapping, Eq. (9-23) |
| DFT; FFT; PSD; window; leakage | Eq. (9-25); its fast algorithm; power per Hz, Eq. (9-26); taper; spectral smearing |
| FIR; IIR; linear phase; DF2T | Non-recursive and recursive filters; constant group delay; Eq. (9-27) |
| Controllable; observable; stabilizable; detectable | Eq. (9-30) and the weaker conditions on unstable modes |
| Pole placement; Ackermann; observer; separation principle | Eq. (9-31); Eq. (9-32) |
| Lyapunov equation; Gramian; H2 norm; H∞ norm | Eq. (9-33); `W_c`, `W_o`; Eq. (9-34); Eq. (9-52) |
| LQR; CARE; DARE; Bryson's rule | Eq. (9-36); Eq. (9-38); weights `1/max²` |
| Linearization; Bellman equation | Eq. (9-40); Eq. (9-41) |
| White noise; covariance; Kalman filter; NIS | Uncorrelated flat-spectrum signal; `E[xxᵀ]`; Eq. (9-43); normalised innovation squared |
| LQG; duality; EKF; UKF | LQR on the Kalman estimate; `(A, B) ↔ (Aᵀ, Cᵀ)`; Eq. (9-45); Eq. (9-46) |
| ARX; RLS; forgetting factor; persistency of excitation | Eq. (9-47); Eq. (9-48); `λ`; full-rank regressor |
| ETFE; coherence; Markov parameters; ERA | Eq. (9-49); `γ²`; impulse response; Eq. (9-50) |
| Multiplicative uncertainty; small gain; robust performance | Eq. (9-51) |
| QP; KKT; Hildreth's method; MPC; iLQR | Eq. (9-53); Eq. (9-54); Eq. (9-55) |
| Generalized plant; LFT; γ-iteration | Eq. (9-56) |

---

## Appendix B — References

### B.1 Reading Roles

| Subject | Primary | Supporting | Optional |
|---|---|---|---|
| C++ | [R1] with [R2] for first-pass explanations | [R3] lookup; [R4] Eigen; [R5] CMake; [R6] GoogleTest | [R7] after WP 6.6; [R49] |
| Control, introductory | [R8] | [R9] worked design examples | [R10] |
| Digital control | [R11] | [R9, ch. 8] | — |
| Linear systems theory | [R8] | [R12] | — |
| Optimal control | [R13] LQ theory; [R14] dynamic programming | [R9, §7.5]; [R15] | [R52] trajectory optimisation |
| Nonlinear | [R16] selected sections | [R45] models | — |
| Estimation | [R17] | [R40]; [R46] consistency | [R18] |
| Identification | [R19] | [R36]; [R43] | [R20]; [R50] |
| Robust control | [R21] | [R23]; [R34] | [R22] |
| MPC and optimisation | [R25] implementation; [R24] theory | [R26] | [R28]; [R41] |
| DSP | [R29] | [R30]; [R31] | [R53] |
| Numerical judgement | [R51] | — | — |

Section numbers refer to the editions listed; where a chapter is cited, the topic is named and located through the chapter's table of contents.

### B.2 Sources

**C++ and tooling**
- [R1] S. B. Lippman, J. Lajoie, B. E. Moo, *C++ Primer*, 5th ed., Addison-Wesley, 2012.
- [R2] learncpp.com, *Learn C++* (online tutorial; 2025 site structure).
- [R3] cppreference.com, *C++ reference*.
- [R4] Eigen project, *Eigen 3.4 documentation*.
- [R5] Kitware, *CMake 3.28 documentation and tutorial* (version pinned).
- [R6] Google, *GoogleTest User's Guide*.
- [R7] S. Meyers, *Effective Modern C++*, O'Reilly, 2014.
- [R47] Mozilla Developer Network, *SVG element reference*.
- [R49] B. Stroustrup, H. Sutter (eds.), *C++ Core Guidelines*, rules C.20 and C.21.
- [R51] N. J. Higham, *Accuracy and Stability of Numerical Algorithms*, 2nd ed., SIAM, 2002.

**Control systems**
- [R8] K. J. Åström, R. M. Murray, *Feedback Systems*, 2nd ed., Princeton University Press, 2021.
- [R9] G. F. Franklin, J. D. Powell, A. Emami-Naeini, *Feedback Control of Dynamic Systems*, 8th ed., Pearson, 2019.
- [R10] N. S. Nise, *Control Systems Engineering*, 8th ed., Wiley, 2019.
- [R11] K. J. Åström, B. Wittenmark, *Computer-Controlled Systems*, 3rd ed., Dover, 2011.
- [R12] C.-T. Chen, *Linear System Theory and Design*, 4th ed., Oxford University Press, 2013.
- [R13] B. D. O. Anderson, J. B. Moore, *Optimal Control: Linear Quadratic Methods*, Dover, 2007.
- [R14] D. P. Bertsekas, *Dynamic Programming and Optimal Control*, Vol. 1, 4th ed., Athena Scientific, 2017.
- [R15] D. E. Kirk, *Optimal Control Theory: An Introduction*, Dover, 2004.
- [R16] H. K. Khalil, *Nonlinear Systems*, 3rd ed., Prentice Hall, 2002.
- [R17] D. Simon, *Optimal State Estimation*, Wiley, 2006.
- [R18] S. Thrun, W. Burgard, D. Fox, *Probabilistic Robotics*, MIT Press, 2005.
- [R19] L. Ljung, *System Identification: Theory for the User*, 2nd ed., Prentice Hall, 1999.
- [R20] R. Pintelon, J. Schoukens, *System Identification: A Frequency Domain Approach*, 2nd ed., Wiley-IEEE, 2012.
- [R21] S. Skogestad, I. Postlethwaite, *Multivariable Feedback Control*, 2nd ed., Wiley, 2005.
- [R22] J. C. Doyle, B. A. Francis, A. R. Tannenbaum, *Feedback Control Theory*, Macmillan, 1992.
- [R23] K. Zhou, J. C. Doyle, *Essentials of Robust Control*, Prentice Hall, 1998.
- [R24] J. B. Rawlings, D. Q. Mayne, M. M. Diehl, *Model Predictive Control: Theory, Computation, and Design*, 2nd ed., Nob Hill, 2017.
- [R25] L. Wang, *Model Predictive Control System Design and Implementation Using MATLAB*, Springer, 2009.
- [R26] S. Boyd, L. Vandenberghe, *Convex Optimization*, Cambridge University Press, 2004.
- [R27] K. J. Åström, B. Wittenmark, *Adaptive Control*, 2nd ed., Dover, 2008.
- [R28] J. M. Maciejowski, *Predictive Control with Constraints*, Prentice Hall, 2002.
- [R45] *Control Tutorials for MATLAB and Simulink* (ctms.engin.umich.edu).
- [R46] Y. Bar-Shalom, X. R. Li, T. Kirubarajan, *Estimation with Applications to Tracking and Navigation*, Wiley, 2001.
- [R52] R. Tedrake, *Underactuated Robotics*, chapter "Trajectory Optimization", MIT course notes.

**Signal processing and numerical methods**
- [R29] A. V. Oppenheim, R. W. Schafer, *Discrete-Time Signal Processing*, 3rd ed., Pearson, 2010.
- [R30] S. W. Smith, *The Scientist and Engineer's Guide to Digital Signal Processing*, 1997.
- [R31] SciPy developers, *scipy.signal* and *scipy.linalg* references.
- [R41] J. Nocedal, S. J. Wright, *Numerical Optimization*, 2nd ed., Springer, 2006.
- [R42] S. C. Chapra, R. P. Canale, *Numerical Methods for Engineers*, 7th ed., McGraw-Hill, 2015.
- [R53] R. G. Lyons, *Understanding Digital Signal Processing*, 3rd ed., Prentice Hall, 2010.

**Papers**
- [R32] A. J. Laub, "A Schur method for solving algebraic Riccati equations," *IEEE TAC* 24(6), 1979.
- [R33] J. C. Doyle, "Guaranteed margins for LQG regulators," *IEEE TAC* 23(4), 1978.
- [R34] J. C. Doyle, K. Glover, P. P. Khargonekar, B. A. Francis, "State-space solutions to standard H2 and H∞ control problems," *IEEE TAC* 34(8), 1989.
- [R35] E. A. Wan, R. van der Merwe, "The unscented Kalman filter for nonlinear estimation," *Proc. IEEE AS-SPCC*, 2000.
- [R36] J.-N. Juang, R. S. Pappa, "An eigensystem realization algorithm for modal parameter identification and model reduction," *J. Guidance, Control, and Dynamics* 8(5), 1985.
- [R37] Y. Tassa, T. Erez, E. Todorov, "Synthesis and stabilization of complex behaviors through online trajectory optimization," *Proc. IEEE/RSJ IROS*, 2012.
- [R38] S. Skogestad, "Simple analytic rules for model reduction and PID controller tuning," *J. Process Control* 13(4), 2003.
- [R39] N. A. Bruinsma, M. Steinbuch, "A fast algorithm to compute the H∞-norm of a transfer function matrix," *Systems & Control Letters* 14(4), 1990.
- [R40] G. Welch, G. Bishop, "An Introduction to the Kalman Filter," TR 95-041, UNC, 2006.
- [R43] E. C. Levy, "Complex-curve fitting," *IRE TAC* 4(1), 1959; C. K. Sanathanan, J. Koerner, "Transfer function synthesis as a ratio of two complex polynomials," *IEEE TAC* 8(1), 1963.
- [R44] M. G. Safonov, M. Athans, "Gain and phase margin for multiloop LQG regulators," *IEEE TAC* 22(2), 1977.
- [R48] S. Boyd, V. Balakrishnan, "A regularity result for the singular values of a transfer matrix and a quadratically convergent algorithm for computing its L∞-norm," *Systems & Control Letters* 15(1), 1990.
- [R50] P. Van Overschee, B. De Moor, *Subspace Identification for Linear Systems*, Kluwer, 1996.

---

*End of Project Plan Outline.*
