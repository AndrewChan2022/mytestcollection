"""
B-Spline implementation with constructor, evaluation, interpolation, and approximation.
"""

import numpy as np
import matplotlib.pyplot as plt
from typing import Optional


class BSpline:
    """
    B-Spline curve defined by degree, knot vector, and control points.

    A B-spline of degree p with n+1 control points P_0..P_n is defined by:
        C(t) = sum_{i=0}^{n} N_{i,p}(t) * P_i
    where N_{i,p} are the B-spline basis functions and t in [knots[p], knots[n+1]].
    """

    def __init__(self, control_points: np.ndarray, degree: int, knots: Optional[np.ndarray] = None):
        """
        Construct a B-spline from control points, degree, and optional knot vector.

        Args:
            control_points: (n+1, dim) array of control points
            degree:         polynomial degree p (e.g. 3 for cubic)
            knots:          knot vector of length n+1+p+1; if None, a uniform
                            clamped knot vector is generated automatically
        """
        self.ctrl = np.atleast_2d(control_points).astype(float)
        self.p = degree
        n = len(self.ctrl) - 1          # index of last control point

        if knots is not None:
            self.knots = np.asarray(knots, dtype=float)
        else:
            self.knots = self._uniform_clamped_knots(n, degree)

        # parameter domain: [t_min, t_max]
        self.t_min = self.knots[self.p]
        self.t_max = self.knots[n + 1]

    # ------------------------------------------------------------------
    # Knot vector helpers
    # ------------------------------------------------------------------

    @staticmethod
    def _uniform_clamped_knots(n: int, p: int) -> np.ndarray:
        """
        Build a clamped (open) uniform knot vector for n+1 control points
        of degree p.  The vector has n+p+2 entries:
            [0,...,0, inner_vals, 1,...,1]
              (p+1)                  (p+1)
        """
        m = n + p + 1                       # last knot index
        num_inner = m - 2 * p - 1           # number of strictly interior knots
        inner = np.linspace(0, 1, num_inner + 2)[1:-1] if num_inner > 0 else []
        return np.concatenate([np.zeros(p + 1), inner, np.ones(p + 1)])

    # ------------------------------------------------------------------
    # Basis function evaluation (Cox–de Boor recursion)
    # ------------------------------------------------------------------

    def _basis(self, i: int, p: int, t: float) -> float:
        """Compute N_{i,p}(t) via the Cox–de Boor recursion."""
        if p == 0:
            # Special-case the right endpoint so it maps to the last span
            if self.knots[i] <= t < self.knots[i + 1]:
                return 1.0
            if t == self.knots[-1] and self.knots[i] <= t <= self.knots[i + 1]:
                return 1.0
            return 0.0

        denom1 = self.knots[i + p] - self.knots[i]
        denom2 = self.knots[i + p + 1] - self.knots[i + 1]

        term1 = ((t - self.knots[i]) / denom1 * self._basis(i, p - 1, t)
                 if denom1 != 0 else 0.0)
        term2 = ((self.knots[i + p + 1] - t) / denom2 * self._basis(i + 1, p - 1, t)
                 if denom2 != 0 else 0.0)
        return term1 + term2

    # ------------------------------------------------------------------
    # Curve evaluation
    # ------------------------------------------------------------------

    def eval(self, t: float) -> np.ndarray:
        """
        Evaluate the B-spline at parameter t in [t_min, t_max].

        Returns a 1-D array of length dim (the point on the curve).
        """
        t = float(np.clip(t, self.t_min, self.t_max))
        n = len(self.ctrl) - 1
        point = sum(self._basis(i, self.p, t) * self.ctrl[i] for i in range(n + 1))
        return np.asarray(point)

    def eval_many(self, ts: np.ndarray) -> np.ndarray:
        """Evaluate at multiple parameter values; returns (len(ts), dim) array."""
        return np.array([self.eval(t) for t in ts])

    def sample(self, num: int = 200) -> tuple[np.ndarray, np.ndarray]:
        """
        Uniformly sample the curve.

        Returns:
            ts:     1-D array of parameter values
            pts:    (num, dim) array of curve points
        """
        ts = np.linspace(self.t_min, self.t_max, num)
        return ts, self.eval_many(ts)

    # ------------------------------------------------------------------
    # Interpolation constructor
    # ------------------------------------------------------------------

    @classmethod
    def interpolate(cls, data_points: np.ndarray, degree: int = 3) -> "BSpline":
        """
        Construct a B-spline that passes exactly through every data point.

        Uses chord-length parameterisation and solves the linear system
        N * P = D  for the unknown control points P.

        Args:
            data_points: (m, dim) array of points to interpolate
            degree:      curve degree (default 3 — cubic)

        Returns:
            BSpline that interpolates all data_points
        """
        data_points = np.atleast_2d(data_points).astype(float)
        m = len(data_points)            # number of data points
        n = m - 1                       # last index (n+1 control points = m)
        p = degree

        # --- chord-length parameterisation ---
        diffs = np.diff(data_points, axis=0)
        chord_lengths = np.linalg.norm(diffs, axis=1)
        total = chord_lengths.sum()
        if total == 0:
            params = np.linspace(0.0, 1.0, m)
        else:
            params = np.concatenate([[0.0], np.cumsum(chord_lengths) / total])

        # --- averaging knot vector (Piegl & Tiller, eq. 9.8) ---
        knots = np.zeros(n + p + 2)
        knots[-(p + 1):] = 1.0
        for j in range(1, n - p + 1):
            knots[j + p] = params[j:j + p].mean()

        # --- build collocation matrix N (m x m) ---
        dummy = cls(data_points, p, knots)   # reuse basis evaluation
        N = np.zeros((m, m))
        for row, t in enumerate(params):
            for col in range(m):
                N[row, col] = dummy._basis(col, p, t)

        # --- solve for control points ---
        ctrl = np.linalg.solve(N, data_points)
        return cls(ctrl, p, knots)

    # ------------------------------------------------------------------
    # Least-squares approximation constructor
    # ------------------------------------------------------------------

    @classmethod
    def approximate(cls, data_points: np.ndarray, n_ctrl: int, degree: int = 3) -> "BSpline":
        """
        Fit a B-spline with n_ctrl control points to data_points in the
        least-squares sense (more data points than control points allowed).

        Args:
            data_points: (m, dim) array of data to approximate
            n_ctrl:      number of control points (< m for true approximation)
            degree:      curve degree (default 3)

        Returns:
            BSpline least-squares fit
        """
        data_points = np.atleast_2d(data_points).astype(float)
        m = len(data_points)
        n = n_ctrl - 1
        p = degree

        if n_ctrl >= m:
            return cls.interpolate(data_points, degree)

        # chord-length params
        diffs = np.diff(data_points, axis=0)
        chord_lengths = np.linalg.norm(diffs, axis=1)
        total = chord_lengths.sum()
        params = np.concatenate([[0.0], np.cumsum(chord_lengths) / total]) if total else np.linspace(0, 1, m)

        # uniform interior knots (simple choice for approximation)
        knots = cls._uniform_clamped_knots(n, p)

        # collocation matrix (m x n_ctrl)
        dummy = cls(data_points[:n_ctrl], p, knots)
        N = np.zeros((m, n_ctrl))
        for row, t in enumerate(params):
            for col in range(n_ctrl):
                N[row, col] = dummy._basis(col, p, t)

        # least-squares solve: min ||N*P - D||^2
        ctrl, _, _, _ = np.linalg.lstsq(N, data_points, rcond=None)
        return cls(ctrl, p, knots)

    # ------------------------------------------------------------------
    # Dunder helpers
    # ------------------------------------------------------------------

    def __repr__(self) -> str:
        n = len(self.ctrl)
        dim = self.ctrl.shape[1] if self.ctrl.ndim > 1 else 1
        return (f"BSpline(degree={self.p}, n_ctrl={n}, dim={dim}, "
                f"domain=[{self.t_min:.3f}, {self.t_max:.3f}])")


# ======================================================================
# Demo
# ======================================================================

def demo():
    """Demonstrate interpolation and approximation on a sine curve."""

    # ---- ground-truth sine data ----
    x_data = np.linspace(0, 2 * np.pi, 20)
    y_data = np.sin(x_data)
    data_pts = np.column_stack([x_data, y_data])   # shape (20, 2)

    # ---- 1. Interpolating B-spline (passes through every point) ----
    bs_interp = BSpline.interpolate(data_pts, degree=3)
    print(bs_interp)
    _, curve_interp = bs_interp.sample(300)

    # ---- 2. Approximating B-spline (8 control points, degree 3) ----
    bs_approx = BSpline.approximate(data_pts, n_ctrl=8, degree=3)
    print(bs_approx)
    _, curve_approx = bs_approx.sample(300)

    # ---- 3. Dense reference sine curve ----
    x_ref = np.linspace(0, 2 * np.pi, 500)
    y_ref = np.sin(x_ref)

    # ---- Plot ----
    fig, axes = plt.subplots(1, 2, figsize=(13, 5))
    fig.suptitle("B-Spline demo — Sine curve", fontsize=14, fontweight="bold")

    # -- left: interpolation --
    ax = axes[0]
    ax.plot(x_ref, y_ref, "k--", linewidth=1.2, label="True sin(x)", zorder=1)
    ax.plot(curve_interp[:, 0], curve_interp[:, 1],
            "b-", linewidth=2, label="B-spline interpolation", zorder=2)
    ax.scatter(data_pts[:, 0], data_pts[:, 1],
               color="red", zorder=3, s=40, label="Data points")
    ax.scatter(bs_interp.ctrl[:, 0], bs_interp.ctrl[:, 1],
               color="orange", marker="^", zorder=4, s=60, label="Control points")
    ax.plot(bs_interp.ctrl[:, 0], bs_interp.ctrl[:, 1],
            color="orange", linestyle=":", linewidth=1, zorder=3)
    ax.set_title(f"Interpolation  ({bs_interp})")
    ax.legend(fontsize=8)
    ax.set_xlabel("x")
    ax.set_ylabel("y")
    ax.grid(True, alpha=0.3)

    # -- right: approximation --
    ax = axes[1]
    ax.plot(x_ref, y_ref, "k--", linewidth=1.2, label="True sin(x)", zorder=1)
    ax.plot(curve_approx[:, 0], curve_approx[:, 1],
            "g-", linewidth=2, label="B-spline approximation (8 ctrl)", zorder=2)
    ax.scatter(data_pts[:, 0], data_pts[:, 1],
               color="red", zorder=3, s=40, label="Data points (20)")
    ax.scatter(bs_approx.ctrl[:, 0], bs_approx.ctrl[:, 1],
               color="purple", marker="^", zorder=4, s=80, label="Control points (8)")
    ax.plot(bs_approx.ctrl[:, 0], bs_approx.ctrl[:, 1],
            color="purple", linestyle=":", linewidth=1, zorder=3)
    ax.set_title(f"Approximation  ({bs_approx})")
    ax.legend(fontsize=8)
    ax.set_xlabel("x")
    ax.set_ylabel("y")
    ax.grid(True, alpha=0.3)

    plt.tight_layout()
    out_path = "bspline_demo.png"
    plt.savefig(out_path, dpi=150)
    print(f"Plot saved to {out_path}")
    plt.show()


if __name__ == "__main__":
    demo()
