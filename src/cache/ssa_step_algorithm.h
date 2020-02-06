//
// Created by rafaelsa on 04/02/2020.
//

#ifndef ESBMC_SSA_STEP_ALGORITHM_H
#define ESBMC_SSA_STEP_ALGORITHM_H

#include <solvers/smt/smt_ast.h>

/** @file smt_algorithm.h
 *  An algorithm to run in an SSA step
 *  The concept is to take an AST and return an AST after the algorithm.
 *
 *
 *  @see smt_convt
 *  @see smt_ast
 */
#include <iostream>

#include <goto-symex/symex_target_equation.h>
class ssa_step_algorithm
{
public:
  explicit ssa_step_algorithm(symex_target_equationt::SSA_stepst &steps)
    : steps(steps)
  {
    for(auto i : steps)
    {
      if(i.is_assert())
      {
        std::cout << "Got an assert "
                  << "\n";
        auto &cond = i.cond;
        cond->dump();
      }
    }
  }

protected:
  symex_target_equationt::SSA_stepst steps;
};
#endif //ESBMC_SSA_STEP_ALGORITHM_H
