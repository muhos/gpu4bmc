/*******************************************************************\

Module:

Author: Muhammad Osama, 2021

\*******************************************************************/


#ifndef CPROVER_SOLVERS_SAT_SATCHECK_PARAFROST_H
#define CPROVER_SOLVERS_SAT_SATCHECK_PARAFROST_H

#include "solvers/sat/cnf.h"
#include <solvers/hardness_collector.h>

namespace pFROST { class ParaFROST; }

class satcheck_parafrostt : public cnf_solvert, public hardness_collectort
{
protected:
  resultt do_prop_solve() override;

  pFROST::ParaFROST* solver;
  
  uint32_t time_limit_seconds;

  void add_variables();
  
  bvt assumptions;

  optionalt<solver_hardnesst> solver_hardness;
	
public:
  explicit satcheck_parafrostt(message_handlert &message_handler);

  virtual ~satcheck_parafrostt() override;

  const std::string solver_text() override;

  tvt l_get(literalt a) const override final;

  void lcnf(const bvt &bv) override final;
  
  void set_assignment(literalt a, bool value) override;

  void set_assumptions(const bvt &_assumptions) override;

  void interrupt();

  void clear_interrupt();
  
  void set_frozen(literalt a) override final;
  
  bool is_eliminated(literalt a) const;

  bool is_in_conflict(literalt a) const override;
  
  bool has_set_assumptions() const override final
  {
    return true;
  }
  bool has_is_in_conflict() const override final
  {
    return true;
  }

  void set_time_limit_seconds(uint32_t lim) override
  {
    time_limit_seconds=lim;
  }

  void with_solver_hardness(std::function<void(solver_hardnesst &)> handler) override
  {
    if(solver_hardness.has_value())
    {
      handler(solver_hardness.value());
    }
  }

  void enable_hardness_collection() override
  {
    solver_hardness = solver_hardnesst{};
  }

};

#endif 
