// ------------------------------------------------------------------------
//
// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright (C) 2011 - 2020 by the deal.II authors
//
// This file is part of the deal.II library.
//
// Part of the source code is dual licensed under Apache-2.0 WITH
// LLVM-exception OR LGPL-2.1-or-later. Detailed license information
// governing the source code and code contributions can be found in
// LICENSE.md and CONTRIBUTING.md at the top level directory of deal.II.
//
// ------------------------------------------------------------------------



// test a problem we used to have: FESystem would delete an internal object
// after reinitialization for the first time if it determined that it was no
// longer necessary. yet, somehow, it was still referenced. the point seems to
// have been that the base element always only had update_default for the
// values that need to be updated on each cell, which is rather uncommon (the
// base element is FE_Nothing)
//
// an extract of this bug is fe/crash_01


#include <deal.II/base/quadrature_lib.h>

#include <deal.II/dofs/dof_accessor.h>
#include <deal.II/dofs/dof_handler.h>
#include <deal.II/dofs/dof_tools.h>

#include <deal.II/fe/fe_nothing.h>
#include <deal.II/fe/fe_q.h>
#include <deal.II/fe/fe_system.h>

#include <deal.II/grid/grid_generator.h>
#include <deal.II/grid/grid_refinement.h>
#include <deal.II/grid/tria.h>
#include <deal.II/grid/tria_accessor.h>
#include <deal.II/grid/tria_iterator.h>

#include <deal.II/hp/fe_collection.h>
#include <deal.II/hp/fe_values.h>

#include <deal.II/lac/affine_constraints.h>

#include "../tests.h"



template <int dim>
void
test()
{
  Triangulation<dim> triangulation;
  GridGenerator::hyper_cube(triangulation, -0.5, 0.5);

  FESystem<dim>   fe(FE_Q<dim>(1), 1, FE_Nothing<dim>(), 1);
  DoFHandler<dim> dof_handler(triangulation);
  dof_handler.distribute_dofs(fe);

  QGauss<dim - 1>            q(2);
  FEFaceValues<dim>          fe_values(fe, q, update_values);
  FEValuesExtractors::Scalar nothing(1);
  fe_values.reinit(dof_handler.begin_active(), 0);

  // the following (second) call to reinit
  // used to abort
  fe_values.reinit(dof_handler.begin_active(), 1);
  for (unsigned int i = 0; i < fe.dofs_per_cell; ++i)
    for (unsigned int q = 0; q < fe_values.n_quadrature_points; ++q)
      deallog << "i=" << i << ", q=" << q
              << ", value=" << fe_values[nothing].value(i, q) << std::endl;
}



int
main()
{
  initlog();
  deallog.get_file_stream().precision(2);

  test<2>();

  deallog << "OK" << std::endl;
}
