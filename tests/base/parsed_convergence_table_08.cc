// ---------------------------------------------------------------------
//
// Copyright (C) 2019 by the deal.II authors
//
// This file is part of the deal.II library.
//
// The deal.II library is free software; you can use it, redistribute
// it, and/or modify it under the terms of the GNU Lesser General
// Public License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
// The full text of the license can be found in the file LICENSE.md at
// the top level directory of deal.II.
//
// ---------------------------------------------------------------------

// Test the functionality of the ParsedConvergenceTable class for
// custom error computations, with no extra columns.

#include <deal.II/base/function_lib.h>
#include <deal.II/base/parsed_convergence_table.h>

#include <deal.II/dofs/dof_handler.h>

#include <deal.II/fe/fe_q.h>
#include <deal.II/fe/fe_system.h>

#include <deal.II/grid/grid_generator.h>
#include <deal.II/grid/tria.h>

#include <deal.II/lac/vector.h>

#include <map>

#include "../tests.h"

int
main()
{
  initlog();

  ParsedConvergenceTable table({"u"}, {{ParsedConvergenceTableFlags::custom}});

  ParameterHandler prm;
  table.add_parameters(prm);

  std::string input = "set Extra columns = none\n";

  prm.parse_input_from_string(input);

  Triangulation<2> tria;
  GridGenerator::hyper_cube(tria);

  FESystem<2>   fe(FE_Q<2>(1), 1);
  DoFHandler<2> dh(tria);

  for (unsigned int i = 0; i < 5; ++i)
    {
      tria.refine_global(1);
      dh.distribute_dofs(fe);
      auto cycle = [&](const unsigned int) { return (i + 1) * 1.0; };
      table.custom_error(cycle, dh, "cycle", true);
    }
  table.output_table(deallog.get_file_stream());
}
