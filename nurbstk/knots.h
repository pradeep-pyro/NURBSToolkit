/*
@file nurbstk/nurbs_knots.h
@author Pradeep Kumar Jayaraman <pradeep.pyro@gmail.com>

Helper functions for creating and modifying knot vectors.

Use of this source code is governed by a BSD-style license that can be found in
the LICENSE.txt file.
*/

#pragma once

#include <algorithm>
#include <vector>

template <typename T>
bool makeUniformKnotVector(unsigned int degree, size_t nCtrlPts,
                           std::vector<T> &knots) {
    // Compute number of knots
    if (nCtrlPts < degree + 1) {
        return false;
    }
    int nKnots = nCtrlPts + degree + 1;

    // Interior knots
    T step = 1.0 / (nKnots - 1);
    knots.clear();
    knots.reserve(nKnots);
    for (T u = 0.0; u <= 1.0; u += step) {
        knots.push_back(u);
    }

    return true;
}

template <typename T>
bool makeClampedUniformKnotVector(unsigned int degree, size_t nCtrlPts,
                                  std::vector<T> &knots) {
    // Compute number of knots
    if (nCtrlPts < degree + 1) {
        return false;
    }
    int nKnots = nCtrlPts + degree + 1;
    int nIntKnots = nKnots - 2 * degree;

    knots.clear();
    knots.reserve(nKnots);

    // Clamp left side
    for (int i = 0; i < degree; i++) {
        knots.push_back(0.0);
    }

    // Interior knots
    if (nIntKnots > 0) {
        double step = 1.0 / (nIntKnots - 1);
        double u = 0.0;
        for (int i = 0; i < nIntKnots; i++) {
            knots.push_back(static_cast<double>(i) * step);
        }
    }

    // Clamp right side
    for (int i = 0; i < degree; i++) {
        knots.push_back(1.0);
    }

    return true;
}

template <typename T>
void clampKnotVector(unsigned int degree, std::vector<T> &knots) {
    clampKnotVectorLeft(degree, knots);
    clampKnotVectorRight(degree, knots);
}

template <typename T>
void clampKnotVectorLeft(unsigned int degree, std::vector<T> &knots) {
    T start = knots[degree];
    for (int i = 0; i < degree; i++) {
        knots[i] = start;
    }
}

template <typename T>
void clampKnotVectorRight(unsigned int degree, std::vector<T> &knots) {
    T end = knots[knots.size() - degree - 1];
    for (int i = 0; i < degree; i++) {
        knots[knots.size() - 1 - i] = end;
    }
}

template <typename T>
bool isKnotVectorMonotonic(const std::vector<T> &knots) {
    return std::is_sorted(knots.begin(), knots.end());
}

template <typename T>
bool isKnotVectorClosed(unsigned int degree, const std::vector<T> &knots) {
    T eps = std::numeric_limits<T>::epsilon();
    for (int i = 0; i < degree + 2; i++) {
        if (std::abs(knots[i] - knots[knots.size() - degree - 2 + i]) > eps) {
            return false;
        }
    }
    return true;
}

template <typename T, int n>
void curveInsertKnot(T u, unsigned int deg, std::vector<glm::vec<T, n> &knots, std::vector<glm::vec<T, n> &cp) {
    int span = findSpan(u, deg, knots);
    // Insert new control point and knot between span and (span + 1)
    knots.insert(span, u);
    cp.insert(span, tvecn(0.0));
    // Compute new positions for affected control points
    for (int i = span - deg + 1; i <= span; ++i) {
        T a = (u - knots[i]) / (knots[i + deg] - knots[i]);
        cp[i] = (1 - a) * cp[i - 1] + a * cp[i];
    }
}

template <typename T, int n>
void surfaceInsertKnotU(T u, unsigned int deg, std::vector<glm::vec<T, n> &knots, std::vector<glm::vec<T, n> &cp) {
    int span = findSpan(u, deg, knots);
    // Insert new control point and knot between span and (span + 1)
    knots.insert(span, u);
    for (int i = 0; i < cp.size(); ++i) {
        cp[i].insert(span, tvecn(0.0));
    }
    // Compute new positions for affected control points
    for (int i = 0; i < cp.size(); ++i) {
        for (int j = span - deg + 1; j <= span; ++j) {
            T a = (u - knots[i]) / (knots[i + deg] - knots[j]);
            cp[i][j] = (1 - a) * cp[i][j - 1] + a * cp[i][j];
        }
    }
}