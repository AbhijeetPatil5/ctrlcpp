# Project Plan Change Log

Changes to the project plan of the Control Systems Library (`csl`), recorded per version of the Project Plan Outline (`docs/PROJECT_PLAN_OUTLINE.md`). Each entry states what changed, why, and where the change is visible in the outline.

---

## 1.0 — 2026-11-01

First version of the Project Plan Outline, derived from Design Document V4. The changes below relative to the earlier design documents (V2/V3) were motivated by an external technical review; each was verified independently before adoption.

### 1. Algorithm and listing corrections

| Area | Change | Where |
|---|---|---|
| Quadratic programming | Hildreth update corrected to `λ_i ← max(0, (d_i − Σ_{j≠i}P_ijλ_j)/P_ii)` with `d = Ax_u − b` (the previous sign returned the infeasible unconstrained point on the first example); equality constraints eliminated through a null-space parameterisation instead of being ignored; a solver status (`Optimal`, `IterationLimit`, `Infeasible`) and a KKT residual added to the result | WP 10.3, Eq. (9-53), Table 8-1 |
| Delay uncertainty | Weight changed to `W_m = 2.2τ_max s/(τ_max s + 1)`; the previous `1.05τs/(1 + τs/2)` does not bound `2|sin(ωτ/2)|` near `ωτ = π` (`1.485` versus `1.683` at `ωτ = 2`); numerical cover verification kept as an acceptance check | WP 10.1, Eq. (9-51) |
| Anti-aliasing experiment | Interference is now attenuated by an analog filter modelled before the sampler (`series(plant, tf2ss(butterworthAnalog(...)))`); a digital post-filter is shown to be unable to remove an alias | WP 5.7 |
| Discrete impulse response | Discrete unit-sample response computed from zero initial state with `u[0] = 1` (`y[0] = D`, `y[k] = CA^{k−1}B`) instead of `x₀ = B`, which shifted the sequence and dropped the feedthrough sample | WP 2.6, Section 9.3 |
| Static gains | `tf2ss` and `ss2tf` handle order zero; `StateSpace` accepts `n = 0` | WP 2.4, Eq. (9-6) |
| Matched pole–zero | Convention fixed as "zeros at `z = −1` until relative degree 1" and the acceptance check aligned with it; `dcGain()` used as the `1×1` matrix it is | WP 5.2, Eq. (9-23) |
| Phase crossover detection | Crossovers located as sign changes of `g = wrapToPi(φ + π)` between samples with `|Δg| < π`; the previous expression was non-negative and had no sign changes | WP 3.2, Eq. (9-10) |
| Simulator ownership | The `std::unique_ptr<Estimator>` member is added only when `Estimator` is a complete type (WP 6.3); copy operations deleted and move operations explicitly defaulted, since a deleted copy suppresses implicit moves | WP 4.2, 6.3, 6.6 |
| Filter interface | Derived filters re-expose the inherited `Vector process(const Vector&)` with `using DigitalFilter::process;` (name hiding) | WP 5.5, Listing 3-5 |
| Lead design result | `LeadDesign` constructed by aggregate initialisation (`TransferFunction` has no default constructor) | WP 4.5 |
| Discrete H2 norm | `tr(DᵀD)` added for discrete systems; continuous systems with `D ≠ 0` throw | WP 6.4, Eq. (9-34) |
| H∞ synthesis | Generalized plant changed to the disturbance-rejection form `w = [d; n]`, `y = G(u + d) + n`, `z = [W_sG(u + d); ρu; W_tG(u + d)]`, which satisfies the DGKF regularity assumptions exactly for strictly proper `G`; a regularity checker added; the reference-tracking mixed-sensitivity form moved to the extensions | WP 10.7, Eq. (9-56), Table 11-1 |
| Package export | The `eigen` interface target exported in the same export set as `csl`, with build and install include interfaces; vendored Eigen headers; `find_dependency` alternative documented | WP 1.6, 11.1 |
| Riccati solvers | Stabilizing property of the solution verified (`A − SP` stable); `lqr` checks stabilizability and weight definiteness; existence stated correctly (no unobservable mode of `(A, Q^{1/2})` *on the imaginary axis*) | WP 7.1–7.2, Eq. (9-36) |

### 2. Acceptance-value corrections

| Check | Change |
|---|---|
| First-order step at `t = τ` | Stated as the closed form `3(1 − e^{−1}) = 1.8963617`; the rounded `1.8964` was outside the stated tolerance |
| ZOH matrices of E1 | `e^{−0.05} = 0.9512294245`, `3(1 − e^{−0.05}) = 0.1463117265` (closed forms); previous decimals were insufficient for the tolerance |
| Tustin of `1/(s+1)` | Stated as exact fractions `1/21`, `19/21` |
| Periodogram peak | Test uses `N = f_s = 1024` so that zero padding does not move the tone off a bin |
| Riccati failure case | `A = 1, S = 1, Q = 0` yields the stabilizing `P = 2`; the throwing case is `A = 0, B = 1, Q = 0` (unobservable mode on the imaginary axis) |
| Kalman versus observer | An observer placed at the Kalman poles has the identical gain; the comparison now uses a faster observer and asserts only the ordering that optimality guarantees |
| RLS equivalence | RLS with finite `P₀` equals regularised batch least squares `(ΦᵀΦ + I/p₀)⁻¹Φᵀy` exactly; unregularised equivalence only in the limit |
| MPC comparison | DP and MPC compared under the same quadratic cost; percentages replaced by recorded values; UKF-versus-EKF comparisons reclassified as experiments |
| ARX with output noise | Bias of ARX under output noise stated; tolerance and a recorded bias check added |

### 3. Architecture and contract changes

- Abstract bases `Controller`, `Estimator` and `DigitalFilter` moved to `include/csl/interfaces/` so that the module dependency graph is acyclic and matches the implementation (Figure 3-2).
- `core` split into single-purpose headers: `RootFinding`, `Controllability`, `Lyapunov`, `Riccati`, `Jacobian`, `Statistics`, `QP` (Figure 3-1).
- `Controller` gained `referenceSize()` and `measurementKind()`; `Estimator` gained `Ts()`; `Margins::closedLoopStable` became `std::optional<bool>`; `SimResult` gained `sampleRate()` and `channel(j)`.
- Semantic contracts added (Table 3-2): measurement kind, reference dimension, estimator timing, noise units, sampled-signal metadata, state coordinates, internal versus input–output stability, unknown stability from data, result-type consumers.
- Numerical policy added (Section 3.8): solve rather than invert, solver status, solution verification, scaling, named sensitive cases, external references (SLICOT, SciPy) as oracles; the claim that no library equivalent exists for Riccati solvers was withdrawn.
- Principle P8 (numerical honesty) added.

### 4. Plan structure

- Work packages organised by stage with explicit prerequisites, products, and acceptance checks; the core track and the five branches defined so that advanced topics can be deferred without invalidating the rest (Figure 7-1).
- Larger packages marked as multi-session with a stated split point.
- A generic completion standard for every package: passing tests, documentation comments, the central derivation reproduced on paper, the failure case explained.
- Final-product acceptance criteria F1–F7 added (Table 2-3).
- Function inventory by module (Chapter 8) and algorithm specifications (Chapter 9) added as the implementation reference of the outline.
- Learning maps for C++ and for control/DSP topics added (Tables 6-1, 6-2).
- Minimal continuous integration introduced with the first tests (WP 1.10); the full pipeline remains in WP 11.3.
- Testing policy T6 distinguishes correctness tests, numerical-quality tests and experiments.

### 5. Reading list

- Reading roles assigned (primary, supporting, optional) per subject (Appendix B.1).
- Chapter references for Åström–Murray aligned with the second edition; Franklin's LQR reference corrected to §7.5; Bertsekas' linear-quadratic section corrected to §3.1; Zhou–Doyle's general H∞ treatment cited as chapter 14.
- The Rule-of-Zero reference replaced by the C++ Core Guidelines rules C.20 and C.21; CMake documentation pinned to a version; Higham (numerical accuracy), Tedrake (trajectory optimisation) and Lyons (DSP introduction) added.

### 6. Review recommendations not adopted

- Renaming or merging modules beyond the `interfaces/` split: the module boundaries are retained.
- Replacing *C++ Primer* or *Discrete-Time Signal Processing* as primary texts: alternatives are listed as supporting sources instead.
- Re-planning the syllabus: the stage sequence was judged sound by the review and is retained.
