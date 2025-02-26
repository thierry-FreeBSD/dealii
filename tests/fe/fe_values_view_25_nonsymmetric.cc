// ------------------------------------------------------------------------
//
// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright (C) 2008 - 2020 by the deal.II authors
//
// This file is part of the deal.II library.
//
// Part of the source code is dual licensed under Apache-2.0 WITH
// LLVM-exception OR LGPL-2.1-or-later. Detailed license information
// governing the source code and code contributions can be found in
// LICENSE.md and CONTRIBUTING.md at the top level directory of deal.II.
//
// ------------------------------------------------------------------------



// like _24_nonsymmetric, but for a simpler mesh for which the output has been
// verified to be correct

#include <deal.II/base/function.h>
#include <deal.II/base/quadrature_lib.h>

#include <deal.II/dofs/dof_handler.h>

#include <deal.II/fe/fe_q.h>
#include <deal.II/fe/fe_system.h>
#include <deal.II/fe/fe_values.h>
#include <deal.II/fe/mapping_q1.h>

#include <deal.II/grid/grid_generator.h>
#include <deal.II/grid/manifold_lib.h>

#include <deal.II/lac/vector.h>

#include "../tests.h"



template <int dim>
void
test(const Triangulation<dim> &tr, const FiniteElement<dim> &fe)
{
  deallog << "FE=" << fe.get_name() << std::endl;

  DoFHandler<dim> dof(tr);
  dof.distribute_dofs(fe);

  Vector<double> fe_function(dof.n_dofs());
  for (unsigned int i = 0; i < dof.n_dofs(); ++i)
    fe_function(i) = i + 1;

  const QGauss<dim> quadrature(2);
  FEValues<dim>     fe_values(fe,
                          quadrature,
                          update_values | update_gradients |
                            update_quadrature_points);
  fe_values.reinit(dof.begin_active());

  // let the FEValues object compute the
  // divergences at quadrature points
  std::vector<Tensor<1, dim>>   divergences(quadrature.size());
  FEValuesExtractors::Tensor<2> extractor(0);
  fe_values[extractor].get_function_divergences(fe_function, divergences);

  // now do the same "by hand"
  std::vector<types::global_dof_index> local_dof_indices(fe.dofs_per_cell);
  dof.begin_active()->get_dof_indices(local_dof_indices);

  for (unsigned int i = 0; i < fe.dofs_per_cell; ++i)
    {
      deallog << "i=" << i << std::endl;

      for (unsigned int q = 0; q < quadrature.size(); ++q)
        deallog << "  q_point=" << fe_values.quadrature_point(q) << std::endl
                << "    value= " << fe_values[extractor].value(i, q)
                << std::endl
                << "    div= " << fe_values[extractor].divergence(i, q)
                << std::endl;
    }
}



template <int dim>
void
test_hyper_cube()
{
  Triangulation<dim> tr;
  GridGenerator::hyper_cube(tr);

  FESystem<dim> fe(FE_Q<dim>(1), Tensor<2, dim>::n_independent_components);
  test(tr, fe);
}


int
main()
{
  initlog();
  deallog << std::setprecision(3);

  test_hyper_cube<1>();
  test_hyper_cube<2>();
  test_hyper_cube<3>();
}
