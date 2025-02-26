// ------------------------------------------------------------------------
//
// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright (C) 2012 - 2021 by the deal.II authors
//
// This file is part of the deal.II library.
//
// Part of the source code is dual licensed under Apache-2.0 WITH
// LLVM-exception OR LGPL-2.1-or-later. Detailed license information
// governing the source code and code contributions can be found in
// LICENSE.md and CONTRIBUTING.md at the top level directory of deal.II.
//
// ------------------------------------------------------------------------


#include <deal.II/base/function_lib.h>
#include <deal.II/base/quadrature_lib.h>

#include <deal.II/dofs/dof_tools.h>

#include <deal.II/lac/affine_constraints.h>
#include <deal.II/lac/vector.h>

#include <deal.II/numerics/data_out.h>
#include <deal.II/numerics/vector_tools.h>

#include <sstream>

#include "../tests.h"

#include "dof_tools_periodic.h"

// A simple test for
//   DoFTools::
//   make_periodicity_constraints (const FaceIterator       &,
//                                 const FaceIterator       &,
//                                 dealii::AffineConstraints<double> &,
//                                 const std::vector<bool>  &)
//
// We project an already periodic function onto the FE space of
// periodic functions and store the resulting L2 difference
// between the projected and unprojected function.
// This should reveal any errors in the constraint_matrix.
// AffineConstraints is std::complex.



template <int dim, int spacedim>
void
check_this(const DoFHandler<dim, spacedim> &dof_handler)
{
  Functions::CosineFunction<dim> test_func(dof_handler.get_fe().n_components());

  AffineConstraints<std::complex<double>> cm;

  // Apply periodic boundary conditions only in the one direction where
  // we can match the (locally refined) faces:
  DoFTools::make_periodicity_constraints(dof_handler.begin(0)->face(0),
                                         dof_handler.begin(0)->face(1),
                                         cm);
  cm.close();

  deallog << cm.n_constraints() << std::endl;
  deallog << cm.max_constraint_indirections() << std::endl;

  QGauss<dim> quadrature(6);

  Vector<double>               unconstrained(dof_handler.n_dofs());
  Vector<std::complex<double>> unconstrained_complex(dof_handler.n_dofs());
  Vector<std::complex<double>> constrained(dof_handler.n_dofs());

  // Ensure that we can interpolate:
  if (dof_handler.get_fe().get_unit_support_points().size() == 0)
    return;

  VectorTools::interpolate(dof_handler, test_func, unconstrained);

  constrained = unconstrained;
  cm.distribute(constrained);

  // Cast Vector<double> to Vector<std::complex<double>>
  for (unsigned int index = 0; index < unconstrained.size(); ++index)
    {
      unconstrained_complex[index] = unconstrained[index];
    }

  constrained -= unconstrained_complex;

  const double p_l2_error = constrained.l2_norm();

  Assert(p_l2_error < 1e-11, ExcInternalError());

  deallog << "L2_Error : " << p_l2_error << std::endl;
}
