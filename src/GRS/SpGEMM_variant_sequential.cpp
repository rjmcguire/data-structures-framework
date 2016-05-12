// GRS sequential sparse CSR matrix-matrix multiplication variant.
// The implementation is based on the classical algorithms in
// [Gustavson, ACM TMS 4(3), 1978] and [Gilbert et al., SIAM JMAA 13(1), 1992]
// with some modifications.
//
// Copyright (C) 2016  Anders Gidenstam
//
// This program is free software: you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free Software
// Foundation, either version 3 of the License, or (at your option) any later
// version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
// more details.
//
// You should have received a copy of the GNU General Public License along with
// this program.  If not, see <http://www.gnu.org/licenses/>.
//

#include "SpGEMM_variant_sequential.h"

#include <ctime>

#include <SpGEMM_Gustavson.h>

namespace EXCESS_GRS {

static void   perform_op(int argcin, int argcout,
                         grs_data **argvin, grs_data **argvout);
static double predict_cost(int argcin, int *in_state, int *in_size);
static double predict_switch(int argcin, int *in_state, int *in_size);

SpGEMM_variant_csr_sequential::
  SpGEMM_variant_csr_sequential(struct grs_component* component, int variant)
{
  grs_component_add_variant(component, &this_variant);
  grs_variant_init(&this_variant, perform_op, variant,
                   predict_cost, predict_switch);
}

SpGEMM_variant_csr_sequential::~SpGEMM_variant_csr_sequential()
{
  grs_variant_destroy(&this_variant);
}

static void perform_op(int argcin, int argcout,
                       grs_data **argvin, grs_data **argvout)
{
  std::cout << "SpGEMM_variant_sequential::perform_op started." << std::endl;
  if (argcin != 8 || argcout != 4) {
    std::cout << ("SpGEMM_variant_sequential::perform_op: Error: "
                  "Wrong number of arguments.")
              << std::endl;
    return;
  }
  matrix_csr A = matrix_csr(argvin);
  matrix_csr B = matrix_csr(argvin + 4);

  {
    struct timespec t1, t2; 
    std::cout << "Attempting matrix matrix multiplication "
              << (A.matrix->m) << "x" << (A.matrix->n) << " * "
              << (B.matrix->m) << "x" << (B.matrix->n) << " ... ";

    clock_gettime(CLOCK_REALTIME, &t1);
    SpMatrix C = SpMM_Gustavson_RowStore(*A.matrix, *B.matrix);
    clock_gettime(CLOCK_REALTIME, &t2);

    std::cout << "Ok." << std::endl;
    std::cout << "% Result C is " << (C.m) << "x" << (C.n) << " matrix with "
              << (C.nzmax) << " non-zeros." << std::endl;
    std::cout << "% Duration: "
              << ((double)(t2.tv_sec - t1.tv_sec) +
                  1e-9 * (double)(t2.tv_nsec - t1.tv_nsec))
              << " sec" << std::endl;
    matrix_csr::convert_to_grs_output(C, argvout);
 }

  std::cout << "SpGEMM_variant_sequential::perform_op finished." << std::endl;
}

static double predict_cost(int argcin, int *in_state, int *in_size)
{
  // FIXME.
  return 4.0;
}

static double predict_switch(int argcin, int *in_state, int *in_size)
{
  // FIXME.
  return 0.0;
}

}
