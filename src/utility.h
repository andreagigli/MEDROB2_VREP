/* ----------------------------------------------------------- */
/* ----------------------------------------------------------- */
/* -------------------- CONVERSION SOURCE ---------------------*/
/* -------- Conversions from Eigen to simFloat/float* ---------*/
/* ----------------------------------------------------------- */
/* ----------------------------------------------------------- */

//! TUTTE TESTATE.
#pragma once

#include <iostream>
#include <algorithm>
#include <Eigen/Core>
#include <Eigen/Geometry>

#include "luaFunctionData.h"
#include "v_repLib.h"
#include "chai3d.h"

#define ARRAY_SIZE(array) (sizeof((array))/sizeof((array[0])))

using namespace std;
using namespace Eigen;

// This function prints a float[12] array, in matrix style.
// TODO: fix last print
void printSimTransform(const float* sim_T);
// This function prints a generic float[r*c] array, in matrix style.
// TODO: fix last print
void printSimMatrix(const float* sim_M, const int r, const int c);


void eigen2SimTransf(const Matrix4f& in, float* out);
void sim2EigenTransf(const float* in, Matrix4f& out);

void eigen2SimRot(const Matrix3f& in, float* out);
void sim2EigenRot(const float* in, Matrix3f& out);

void eigen2SimVec3f(const Vector3f& in, float* out);
void sim2EigenVec3f(const float* in, Vector3f& out);

// This function given a R (3by3) and a P (3by1), builds a T (3by4).
void simComposeTransform(const float* in_rot, const float* in_pos, float* out_T);
// This function decompose a simFloat T into its rotational part (R 3by3)
// and its traslational one (P 3by1).
void simDecomposeTransform(const float* in_T, float* out_rot, float* out_pos);

void simMultiplyVec3fByScalar(float* v, float k);