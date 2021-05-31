/*******************************************************************\

Module:

Author: Muhammad Osama, 2021

\*******************************************************************/

#include "satcheck_parafrost.h"
#include "parafrost_config.h"

#include <util/invariant.h>
#include <util/make_unique.h>
#include <util/threeval.h>

#ifndef HAVE_PARAFROST
#error "Expected HAVE_PARAFROST"
#endif

#include <solve.h>
#include <version.h>

static pFROST::ParaFROST* solver_to_interrupt = nullptr;

bool quiet_en = false;
int verbose = 1;

satcheck_parafrostt::satcheck_parafrostt(message_handlert &message_handler)
  : cnf_solvert(message_handler), solver(pFROST::pfrost = new pFROST::ParaFROST()),
    time_limit_seconds(0)
{
  assert(solver == pFROST::pfrost);
  if (quiet_en) verbose = 0;
  // try to find configuration file path
  const char * configpath_str = getenv("PFROSTCONFIG");
  if (configpath_str == NULL) {
	  PFLOGEN("could not read enviromental variable PFROSTCONFIG");
  } 
  else {	 
      PFLOG2(1, "caught configuration file path:");
	  PFLOG2(1, " %s", configpath_str);
	  string configpath(configpath_str);
	  read_configuration(configpath);
	  if (!quiet_en) PFLRULER('-', RULELEN);
  }
}

satcheck_parafrostt::~satcheck_parafrostt() 
{
	delete solver;
}

static void interrupt_solver(int signum)
{
  (void)signum; // unused parameter -- just removing the name trips up cpplint
  solver_to_interrupt->interrupt();
}

void satcheck_parafrostt::interrupt()
{
  solver->interrupt();
}

void satcheck_parafrostt::clear_interrupt()
{
  solver->nointerrupt();
}

const std::string satcheck_parafrostt::solver_text()
{
  return std::string("ParaFROST ") + pFROST::version();
}

void convert(const bvt &bv, pFROST::Lits_t &dest)
{
  PRECONDITION(bv.size() <= static_cast<std::size_t>(std::numeric_limits<int>::max()));

  dest.reserve(static_cast<int>(bv.size()));	
	
  for(const auto &literal : bv)
  {
    if(!literal.is_false())
      dest.push(V2DEC(literal.var_no(), literal.sign()));
  }
}

tvt satcheck_parafrostt::l_get(literalt a) const
{
  if(a.is_true())
    return tvt(true);
  else if(a.is_false())
    return tvt(false);

  tvt result;

  if(a.var_no()>=(unsigned)solver->model.size())
    return tvt::unknown();

  if(solver->model[a.var_no()]==1)
    result=tvt(true);
  else if(solver->model[a.var_no()]==0)
    result=tvt(false);
  else
    return tvt::unknown();

  if(a.sign())
    result=!result;

  return result;
}

void satcheck_parafrostt::add_variables()
{
  while(pFROST::inf.orgVars < no_variables())
    solver->iadd();
}

void satcheck_parafrostt::lcnf(const bvt &bv)
{
  try
  {
    add_variables();

    for(const auto &literal : bv)
    {
      if(literal.is_true())
        return;
      else if(!literal.is_false())
      {
        INVARIANT(
          literal.var_no() < pFROST::inf.orgVars,
          "variable not added yet");
      }
    }

    pFROST::Lits_t in_c, c;
    in_c.reserve(32);
	
    convert(bv, c);

    if (!solver->itoClause(in_c, c)) {
      log.status() << "ParaFROST: instance is UNSATISFIABLE during adding clauses" << messaget::eom;
	  #undef UNSAT
      status = statust::UNSAT;
    }

    with_solver_hardness([this, &bv](solver_hardnesst &hardness) {
      // To map clauses to lines of program code, track clause indices in the
      // dimacs cnf output. Dimacs output is generated after processing
      // clauses to remove duplicates and clauses that are trivially true.
      // Here, a clause is checked to see if it can be thus eliminated. If
      // not, add the clause index to list of clauses in
      // solver_hardnesst::register_clause().
      static size_t cnf_clause_index = 0;
      bvt cnf;
      bool clause_removed = process_clause(bv, cnf);

      if(!clause_removed)
        cnf_clause_index++;

      hardness.register_clause(bv, cnf, cnf_clause_index, !clause_removed);
    });

    clause_counter++;
  }
  catch(const pFROST::MEMOUTEXCEPTION &)
  {
    log.error() << "SAT checker ran out of memory" << messaget::eom;
    status = statust::ERROR;
    throw std::bad_alloc();
  }
}

propt::resultt satcheck_parafrostt::do_prop_solve()
{
  PRECONDITION(status != statust::ERROR);

  log.statistics() << (no_variables() - 1) << " variables, "
                   << solver->maxClauses() << " clauses" << messaget::eom;

  try
  {
    add_variables();

    if(!solver->status())
    {
      log.status() << "SAT checker inconsistent: instance is UNSATISFIABLE"
                   << messaget::eom;
      status = statust::UNSAT;
      return resultt::P_UNSATISFIABLE;
    }

    // if assumptions contains false, we need this to be UNSAT
    for(const auto &assumption : assumptions)
    {
      if(assumption.is_false())
      {
        log.status() << "got FALSE as assumption: instance is UNSATISFIABLE"
                     << messaget::eom;
        status = statust::UNSAT;
        return resultt::P_UNSATISFIABLE;
      }
    }

    pFROST::Lits_t solver_assumptions;
    convert(assumptions, solver_assumptions);

    using pFROST::CNF_ST;

#ifndef _WIN32

    void (*old_handler)(int) = SIG_ERR;

    if(time_limit_seconds != 0)
    {
      solver_to_interrupt = solver;
      old_handler = signal(SIGALRM, interrupt_solver);
      if(old_handler == SIG_ERR)
        log.warning() << "Failed to set solver time limit" << messaget::eom;
      else
        alarm(time_limit_seconds);
    }
	
    solver->isolve(solver_assumptions);
	CNF_ST solver_result = solver->status();

    if(old_handler != SIG_ERR)
    {
      alarm(0);
      signal(SIGALRM, old_handler);
      solver_to_interrupt = solver;
    }

#else // _WIN32

    if(time_limit_seconds != 0)
    {
      log.warning() << "Time limit ignored (not supported on Win32 yet)"
                    << messaget::eom;
    }

    solver->isolve(solver_assumptions);
    CNF_ST solver_result = solver->status();

#endif

    if(solver_result == 1)
    {
      log.status() << "SAT checker: instance is SATISFIABLE" << messaget::eom;
      CHECK_RETURN(solver->model.size() > 0);
	  #undef SAT
      status = statust::SAT;
      return resultt::P_SATISFIABLE;
    }

    if(solver_result == 0)
    {
      log.status() << "SAT checker: instance is UNSATISFIABLE" << messaget::eom;
      status = statust::UNSAT;
      return resultt::P_UNSATISFIABLE;
    }
	
    log.status() << "SAT checker: timed out or other error" << messaget::eom;
    status = statust::ERROR;
    return resultt::P_ERROR;
  }
  catch(const pFROST::MEMOUTEXCEPTION &)
  {
    log.error() << "SAT checker ran out of memory" << messaget::eom;
    status=statust::ERROR;
    return resultt::P_ERROR;
  }
}

void satcheck_parafrostt::set_assignment(literalt a, bool value)
{
  PRECONDITION(!a.is_constant());

  try
  {
    unsigned v = a.var_no();
    bool sign = a.sign();

    solver->model.value.expand(v + 1, UNDEFINED);
    value ^= sign;
    solver->model[v] = pFROST::LIT_ST(value);
  }
  catch(const pFROST::MEMOUTEXCEPTION &)
  {
    log.error() << "SAT checker ran out of memory" << messaget::eom;
    status = statust::ERROR;
    throw std::bad_alloc();
  }
}

bool satcheck_parafrostt::is_in_conflict(literalt a) const
{
  return solver->ifailed(a.var_no());
}

void satcheck_parafrostt::set_assumptions(const bvt &bv)
{
  assumptions.clear();
  for(const auto &assumption : bv)
  {
    if(!assumption.is_true())
    {
      assumptions.push_back(assumption);
    }
  }
}

void satcheck_parafrostt::set_frozen(literalt a)
{
  try
  {
    if(!a.is_constant())
    {
      add_variables();
      solver->ifreeze(a.var_no());
    }
  }
  catch(const pFROST::MEMOUTEXCEPTION &)
  {
    log.error() << "SAT checker ran out of memory" << messaget::eom;
    status = statust::ERROR;
    throw std::bad_alloc();
  }
}

bool satcheck_parafrostt::is_eliminated(literalt a) const
{
  PRECONDITION(!a.is_constant());

  return solver->ieliminated(a.var_no());
}
