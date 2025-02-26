// ------------------------------------------------------------------------
//
// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright (C) 2023 by the deal.II authors
//
// This file is part of the deal.II library.
//
// Part of the source code is dual licensed under Apache-2.0 WITH
// LLVM-exception OR LGPL-2.1-or-later. Detailed license information
// governing the source code and code contributions can be found in
// LICENSE.md and CONTRIBUTING.md at the top level directory of deal.II.
//
// ------------------------------------------------------------------------

#ifndef dealii_lac_orthogonalization_h
#define dealii_lac_orthogonalization_h


#include <deal.II/base/config.h>

DEAL_II_NAMESPACE_OPEN

namespace LinearAlgebra
{
  /**
   * Supported orthogonalization strategies within SolverGMRES and
   * SolverFGMRES.
   */
  enum class OrthogonalizationStrategy
  {
    /**
     * Use modified Gram-Schmidt algorithm.
     */
    modified_gram_schmidt,
    /**
     * Use classical Gram-Schmidt algorithm. Since this approach works on
     * multi-vectors with a single global reduction (of multiple elements), it
     * is more efficient than the modified Gram-Schmidt algorithm. However, it
     * is less stable in terms of roundoff error propagation, requiring
     * additional re-orthogonalization steps more frequently.
     */
    classical_gram_schmidt
  };
} // namespace LinearAlgebra


DEAL_II_NAMESPACE_CLOSE

#endif
