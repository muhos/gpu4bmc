/*******************************************************************\

Module:

Author: Muhammad Osama, 2021

\*******************************************************************/

#include "parafrost_config.h"
#include <solve.h>

void read_configuration(const std::string &name)
{
  // check if configuration file exists
  PFLOGN2(1, "reading configuration file..");
  struct stat config_st;
  if(!pFROST::canAccess(name.c_str(), config_st))
  {
    PFLOGEN("cannot access %s", name.c_str());
	return;
  }
  // open it
  std::ifstream configfile;
  configfile.open(name, std::ifstream::in);
  if(!configfile.is_open())
  {
    PFLOGEN("cannot open %s (maybe locked)", name.c_str());
	return;
  }
  PFLDONE(1, 5);
  // parse it
  parse_configuration(configfile);
}

void parse_configuration(std::ifstream &configfile)
{
  PFLOGN2(1, "parsing configuration file..");
  int parsed = 0;
  string line;
  while(getline(configfile, line))
  {
    eatSpaces(line);
    if(line[0] == 'c' || line[0] == '#')
      continue;
    if(line.empty() || line[0] == '\n' || line[0] == '\r')
      continue;
	parsed++;
    string option(line.c_str());
    eatCharsInclusive(line, '='), eatSpaces(line);
    const char *str = option.c_str();
    const char *val = line.c_str();
    if(strhas(str, "verbose"))
    {
      verbose = atof(val);
	  quiet_en = true;
    }
    else if(strhas(str, "quiet"))
    {
      quiet_en = isEnabled(val);
	  verbose = 0;
    }
    else if(strhas(str, "bumpreason"))
    {
      pFROST::pfrost->opts.bumpreason_en = isEnabled(val);
    }
    else if(strhas(str, "chrono"))
    {
      pFROST::pfrost->opts.chrono_en = isEnabled(val);
    }
    else if(strhas(str, "chronoreuse"))
    {
      pFROST::pfrost->opts.chronoreuse_en = isEnabled(val);
    }
    else if(strhas(str, "chronomin"))
    {
      pFROST::pfrost->opts.chrono_min = atof(val);
    }
    else if(strhas(str, "debinary"))
    {
      pFROST::pfrost->opts.debinary_en = isEnabled(val);
    }
    else if(strhas(str, "decompose"))
    {
      pFROST::pfrost->opts.decompose_en = isEnabled(val);
    }
    else if(strhas(str, "decomposemin"))
    {
      pFROST::pfrost->opts.decompose_min = atof(val);
    }
    else if(strhas(str, "decomposelimit"))
    {
      pFROST::pfrost->opts.decompose_limit = atof(val);
    }
    else if(strhas(str, "decomposemineff"))
    {
      pFROST::pfrost->opts.decompose_min_eff = atof(val);
    }
    else if(strhas(str, "model"))
    {
      pFROST::pfrost->opts.model_en = isEnabled(val);
    }
    else if(strhas(str, "modelprint"))
    {
      pFROST::pfrost->opts.modelprint_en = isEnabled(val);
    }
    else if(strhas(str, "modelverify"))
    {
      pFROST::pfrost->opts.modelverify_en = isEnabled(val);
    }
    else if(strhas(str, "modeinc"))
    {
      pFROST::pfrost->opts.mode_inc = atof(val);
    }
    else if(strhas(str, "minimizedepth"))
    {
      pFROST::pfrost->opts.minimize_depth = atof(val);
    }
    else if(strhas(str, "mdmvsidsonly"))
    {
      pFROST::pfrost->opts.mdmvsidsonly_en = isEnabled(val);
    }
    else if(strhas(str, "mdmfusem"))
    {
      pFROST::pfrost->opts.mdmfusem_en = isEnabled(val);
    }
    else if(strhas(str, "mdmfuses"))
    {
      pFROST::pfrost->opts.mdmfuses_en = isEnabled(val);
    }
    else if(strhas(str, "mdmlcv"))
    {
      pFROST::pfrost->opts.mdm_mcv_en = !isEnabled(val);
    }
    else if(strhas(str, "mdmvsidspumps"))
    {
      pFROST::pfrost->opts.mdm_vsids_pumps = atof(val);
    }
    else if(strhas(str, "mdmvmfqpumps"))
    {
      pFROST::pfrost->opts.mdm_vmfq_pumps = atof(val);
    }
    else if(strhas(str, "mdmrounds"))
    {
      pFROST::pfrost->opts.mdm_rounds = atof(val);
    }
    else if(strhas(str, "mdmfreq"))
    {
      pFROST::pfrost->opts.mdm_freq = atof(val);
    }
    else if(strhas(str, "mdminc"))
    {
      pFROST::pfrost->opts.mdm_inc = atof(val);
    }
    else if(strhas(str, "mdmsinc"))
    {
      pFROST::pfrost->opts.mdm_sinc = atof(val);
    }
    else if(strhas(str, "mdmdiv"))
    {
      pFROST::pfrost->opts.mdm_div = atof(val);
    }
    else if(strhas(str, "mapperc"))
    {
      pFROST::pfrost->opts.map_perc = atof(val);
    }
    else if(strhas(str, "nap"))
    {
      pFROST::pfrost->opts.nap = atof(val);
    }
    else if(strhas(str, "proofout"))
    {
      string tmp(val);
      eatSpaces(tmp);
      memcpy(pFROST::pfrost->opts.proof_path, tmp.c_str(), tmp.length());
    }
    else if(strhas(str, "parseonly"))
    {
      pFROST::pfrost->opts.parseonly_en = isEnabled(val);
    }
    else if(strhas(str, "proof"))
    {
      pFROST::pfrost->opts.proof_en = isEnabled(val);
    }
    else if(strhas(str, "probe"))
    {
      pFROST::pfrost->opts.probe_en = isEnabled(val);
    }
    else if(strhas(str, "probesleep"))
    {
      pFROST::pfrost->opts.probe_sleep_en = isEnabled(val);
    }
    else if(strhas(str, "probehyper"))
    {
      pFROST::pfrost->opts.probehbr_en = isEnabled(val);
    }
    else if(strhas(str, "probeinc"))
    {
      pFROST::pfrost->opts.probe_inc = atof(val);
    }
    else if(strhas(str, "probemin"))
    {
      pFROST::pfrost->opts.probe_min = atof(val);
    }
    else if(strhas(str, "probemineff"))
    {
      pFROST::pfrost->opts.probe_min_eff = atof(val);
    }
    else if(strhas(str, "probemaxeff"))
    {
      pFROST::pfrost->opts.probe_max_eff = atof(val);
    }
    else if(strhas(str, "probereleff"))
    {
      pFROST::pfrost->opts.probe_rel_eff = atof(val);
    }
    else if(strhas(str, "prograte"))
    {
      pFROST::pfrost->opts.prograte = atof(val);
    }
    else if(strhas(str, "polarity"))
    {
      pFROST::pfrost->opts.polarity = atof(val);
    }
    else if(strhas(str, "targetphase"))
    {
      pFROST::pfrost->opts.targetphase_en = isEnabled(val);
    }
    else if(strhas(str, "ternary"))
    {
      pFROST::pfrost->opts.ternary_en = isEnabled(val);
    }
    else if(strhas(str, "ternarysleep"))
    {
      pFROST::pfrost->opts.ternary_sleep_en = isEnabled(val);
    }
    else if(strhas(str, "ternarypriorbins"))
    {
      pFROST::pfrost->opts.ternary_priorbins = atof(val);
    }
    else if(strhas(str, "ternarymineff"))
    {
      pFROST::pfrost->opts.ternary_min_eff = atof(val);
    }
    else if(strhas(str, "ternarymaxeff"))
    {
      pFROST::pfrost->opts.ternary_max_eff = atof(val);
    }
    else if(strhas(str, "ternaryreleff"))
    {
      pFROST::pfrost->opts.ternary_rel_eff = atof(val);
    }
    else if(strhas(str, "ternaryperc"))
    {
      pFROST::pfrost->opts.ternary_perc = atof(val);
    }
    else if(strhas(str, "transitive"))
    {
      pFROST::pfrost->opts.transitive_en = isEnabled(val);
    }
    else if(strhas(str, "transitivemineff"))
    {
      pFROST::pfrost->opts.transitive_min_eff = atof(val);
    }
    else if(strhas(str, "transitivemaxeff"))
    {
      pFROST::pfrost->opts.transitive_max_eff = atof(val);
    }
    else if(strhas(str, "transitivereleff"))
    {
      pFROST::pfrost->opts.transitive_rel_eff = atof(val);
    }
    else if(strhas(str, "vsids"))
    {
      pFROST::pfrost->opts.vsids_en = isEnabled(val);
    }
    else if(strhas(str, "vsidsonly"))
    {
      pFROST::pfrost->opts.vsidsonly_en = isEnabled(val);
    }
    else if(strhas(str, "varinc"))
    {
      pFROST::pfrost->opts.var_inc = atof(val);
    }
    else if(strhas(str, "vardecay"))
    {
      pFROST::pfrost->opts.var_decay = atof(val);
    }
    else if(strhas(str, "vivify"))
    {
      pFROST::pfrost->opts.vivify_en = isEnabled(val);
    }
    else if(strhas(str, "vivifypriorbins"))
    {
      pFROST::pfrost->opts.vivify_priorbins = atof(val);
    }
    else if(strhas(str, "vivifymineff"))
    {
      pFROST::pfrost->opts.vivify_min_eff = atof(val);
    }
    else if(strhas(str, "vivifymaxeff"))
    {
      pFROST::pfrost->opts.vivify_max_eff = atof(val);
    }
    else if(strhas(str, "vivifyreleff"))
    {
      pFROST::pfrost->opts.vivify_rel_eff = atof(val);
    }
    else if(strhas(str, "report"))
    {
      pFROST::pfrost->opts.report_en = isEnabled(val);
    }
    else if(strhas(str, "reduce"))
    {
      pFROST::pfrost->opts.reduce_en = isEnabled(val);
    }
    else if(strhas(str, "reduceperc"))
    {
      pFROST::pfrost->opts.reduce_perc = atof(val);
    }
    else if(strhas(str, "reduceinc"))
    {
      pFROST::pfrost->opts.reduce_inc = atof(val);
    }
    else if(strhas(str, "rephase"))
    {
      pFROST::pfrost->opts.rephase_en = isEnabled(val);
    }
    else if(strhas(str, "rephaseinc"))
    {
      pFROST::pfrost->opts.rephase_inc = atof(val);
    }
    else if(strhas(str, "reusetrail"))
    {
      pFROST::pfrost->opts.reusetrail_en = isEnabled(val);
    }
    else if(strhas(str, "restartinc"))
    {
      pFROST::pfrost->opts.restart_inc = atof(val);
    }
    else if(strhas(str, "stable"))
    {
      pFROST::pfrost->opts.stable_en = isEnabled(val);
    }
    else if(strhas(str, "stablerate"))
    {
      pFROST::pfrost->opts.stable_rate = atof(val);
    }
    else if(strhas(str, "sigma"))
    {
      pFROST::pfrost->opts.sigma_en = isEnabled(val);
    }
    else if(strhas(str, "sigmalive"))
    {
      pFROST::pfrost->opts.sigma_live_en = isEnabled(val);
    }
    else if(strhas(str, "sigmasleep"))
    {
      pFROST::pfrost->opts.sigma_sleep_en = isEnabled(val);
    }
    else if(strhas(str, "sigma_inc"))
    {
      pFROST::pfrost->opts.sigma_inc = atof(val);
    }
    else if(strhas(str, "sigmamin"))
    {
      pFROST::pfrost->opts.sigma_min = atof(val);
    }
    else if(strhas(str, "sigmapriorbins"))
    {
      pFROST::pfrost->opts.sigma_priorbins = atof(val);
    }
    else if(strhas(str, "subsume"))
    {
      pFROST::pfrost->opts.subsume_en = isEnabled(val);
    }
    else if(strhas(str, "subsumeinc"))
    {
      pFROST::pfrost->opts.subsume_inc = atof(val);
    }
    else if(strhas(str, "subsumepriorbins"))
    {
      pFROST::pfrost->opts.subsume_priorbins = atof(val);
    }
    else if(strhas(str, "subsumeminoccurs"))
    {
      pFROST::pfrost->opts.subsume_min_occs = atof(val);
    }
    else if(strhas(str, "subsumereleff"))
    {
      pFROST::pfrost->opts.subsume_rel_eff = atof(val);
    }
    else if(strhas(str, "subsumemineff"))
    {
      pFROST::pfrost->opts.subsume_min_eff = atof(val);
    }
    else if(strhas(str, "subsumemaxeff"))
    {
      pFROST::pfrost->opts.subsume_max_eff = atof(val);
    }
    else if(strhas(str, "subsumemaxcsize"))
    {
      pFROST::pfrost->opts.subsume_max_csize = atof(val);
    }
    else if(strhas(str, "seed"))
    {
      pFROST::pfrost->opts.seed = atof(val);
    }
    else if(strhas(str, "lbdtier1"))
    {
      pFROST::pfrost->opts.lbd_tier1 = atof(val);
    }
    else if(strhas(str, "lbdtier2"))
    {
      pFROST::pfrost->opts.lbd_tier2 = atof(val);
    }
    else if(strhas(str, "lbdfast"))
    {
      pFROST::pfrost->opts.lbd_fast = atof(val);
    }
    else if(strhas(str, "lbdslow"))
    {
      pFROST::pfrost->opts.lbd_slow = atof(val);
    }
    else if(strhas(str, "lbdrate"))
    {
      pFROST::pfrost->opts.lbd_rate = atof(val);
    }
    else if(strhas(str, "lubyinc"))
    {
      pFROST::pfrost->opts.luby_inc = atof(val);
    }
    else if(strhas(str, "lubymax"))
    {
      pFROST::pfrost->opts.luby_max = atof(val);
    }
    else if(strhas(str, "learntsubmax"))
    {
      pFROST::pfrost->opts.learntsub_max = atof(val);
    }
    else if(strhas(str, "gcperc"))
    {
      pFROST::pfrost->opts.gc_perc = atof(val);
    }
    else if(strhas(str, "all"))
    {
      pFROST::pfrost->opts.all_en = isEnabled(val);
    }
    else if(strhas(str, "aggresivesort"))
    {
      pFROST::pfrost->opts.aggr_cnf_sort = isEnabled(val);
    }
    else if(strhas(str, "bce"))
    {
      pFROST::pfrost->opts.bce_en = isEnabled(val);
    }
    else if(strhas(str, "bcemax"))
    {
      pFROST::pfrost->opts.bce_limit = atof(val);
    }
    else if(strhas(str, "ere"))
    {
      pFROST::pfrost->opts.ere_en = isEnabled(val);
    }
    else if(strhas(str, "eremax"))
    {
      pFROST::pfrost->opts.ere_limit = atof(val);
    }
    else if(strhas(str, "hse"))
    {
      pFROST::pfrost->opts.hse_en = isEnabled(val);
    }
    else if(strhas(str, "hsemax"))
    {
      pFROST::pfrost->opts.hse_limit = atof(val);
    }
    else if(strhas(str, "lcvemin"))
    {
      pFROST::pfrost->opts.lcve_min = atof(val);
    }
    else if(strhas(str, "vephasemin"))
    {
      pFROST::pfrost->opts.lits_min = atof(val);
    }
    else if(strhas(str, "solve"))
    {
      pFROST::pfrost->opts.solve_en = isEnabled(val);
    }
    else if(strhas(str, "gcfreq"))
    {
      pFROST::pfrost->opts.shrink_rate = atof(val);
    }
    else if(strhas(str, "mupos"))
    {
      pFROST::pfrost->opts.mu_pos = atof(val);
    }
    else if(strhas(str, "muneg"))
    {
      pFROST::pfrost->opts.mu_neg = atof(val);
    }
    else if(strhas(str, "phases"))
    {
      pFROST::pfrost->opts.phases = atof(val);
    }
    else if(strhas(str, "profilesimp"))
    {
      pFROST::pfrost->opts.profile_simp = isEnabled(val);
    }
    else if(strhas(str, "ve+"))
    {
      pFROST::pfrost->opts.ve_plus_en = isEnabled(val);
    }
    else if(strhas(str, "ve"))
    {
      pFROST::pfrost->opts.ve_en = isEnabled(val);
    }
    else if(strhas(str, "velbound"))
    {
      pFROST::pfrost->opts.ve_lbound_en = isEnabled(val);
    }
    else if(strhas(str, "veclausemax"))
    {
      pFROST::pfrost->opts.ve_clause_limit = atof(val);
    }
    else if(strhas(str, "xormaxarity"))
    {
      pFROST::pfrost->opts.xor_max_arity = atof(val);
    }

  } // while-end
  if(quiet_en)
    pFROST::pfrost->opts.report_en = false;
  if(pFROST::pfrost->opts.all_en)
  {
    pFROST::pfrost->opts.ve_en = 1;
    pFROST::pfrost->opts.ve_plus_en = 1;
    pFROST::pfrost->opts.bce_en = 1;
    pFROST::pfrost->opts.ere_en = 1;
  }
  if(
    !pFROST::pfrost->opts.phases &&
    (pFROST::pfrost->opts.ve_en || pFROST::pfrost->opts.hse_en ||
     pFROST::pfrost->opts.bce_en))
    pFROST::pfrost->opts.phases = 1; // at least 1 phase needed
  if(
    pFROST::pfrost->opts.phases &&
    !(pFROST::pfrost->opts.ve_en || pFROST::pfrost->opts.hse_en ||
      pFROST::pfrost->opts.bce_en))
    pFROST::pfrost->opts.phases = 0;
  if(pFROST::pfrost->opts.phases > 1 && !pFROST::pfrost->opts.ve_en)
    pFROST::pfrost->opts.phases = 1;
  PFLENDING(1, 5, "(read %d options)", parsed);
}