// ------------------------------------------------------------------------
//
// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright (C) 2004 - 2020 by the deal.II authors
//
// This file is part of the deal.II library.
//
// Part of the source code is dual licensed under Apache-2.0 WITH
// LLVM-exception OR LGPL-2.1-or-later. Detailed license information
// governing the source code and code contributions can be found in
// LICENSE.md and CONTRIBUTING.md at the top level directory of deal.II.
//
// ------------------------------------------------------------------------


// test the GMRES solver using the PETSc matrix and vector classes



#include <deal.II/lac/petsc_sparse_matrix.h>
#include <deal.II/lac/petsc_vector.h>
#include <deal.II/lac/precondition.h>
#include <deal.II/lac/solver.h>
#include <deal.II/lac/solver_bicgstab.h>
#include <deal.II/lac/solver_cg.h>
#include <deal.II/lac/solver_control.h>
#include <deal.II/lac/solver_gmres.h>
#include <deal.II/lac/solver_qmrs.h>
#include <deal.II/lac/solver_richardson.h>
#include <deal.II/lac/vector_memory.h>

#include <iostream>
#include <typeinfo>

#include "../tests.h"

#include "../testmatrix.h"


int
main(int argc, char **argv)
{
  initlog();
  deallog << std::setprecision(4);

  Utilities::MPI::MPI_InitFinalize mpi_initialization(argc, argv, 1);
  {
    SolverControl control(100, 1.e-3);

    const unsigned int size = 32;
    unsigned int       dim  = (size - 1) * (size - 1);

    deallog << "Size " << size << " Unknowns " << dim << std::endl;

    // Make matrix
    FDMatrix                    testproblem(size, size);
    PETScWrappers::SparseMatrix A(dim, dim, 5);
    testproblem.five_point(A);

    IndexSet indices(dim);
    indices.add_range(0, dim);
    PETScWrappers::MPI::Vector f(indices, MPI_COMM_WORLD);
    PETScWrappers::MPI::Vector u(indices, MPI_COMM_WORLD);
    f = 1.;
    A.compress(VectorOperation::insert);

    GrowingVectorMemory<PETScWrappers::MPI::Vector> mem;
    SolverGMRES<PETScWrappers::MPI::Vector>         solver(control, mem);
    PreconditionIdentity                            preconditioner;
    deallog << "Solver type: " << typeid(solver).name() << std::endl;
    check_solver_within_range(solver.solve(A, u, f, preconditioner),
                              control.last_step(),
                              74,
                              76);
  }
  GrowingVectorMemory<PETScWrappers::MPI::Vector>::release_unused_memory();
}
