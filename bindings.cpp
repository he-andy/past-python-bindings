/*
 * project.c: This file is part of the PAST project.
 *
 * PAST: the PoCC Abstract Syntax Tree
 *
 * Copyright (C) 2014 the Ohio State University
 *
 * This program can be redistributed and/or modified under the terms
 * of the license specified in the LICENSE.txt file at the root of the
 * project.
 *
 * Contact: Louis-Noel Pouchet <pouchet@cse.ohio-state.edu>
 * Author: Louis-Noel Pouchet <pouchet@cse.ohio-state.edu>
 *
 */
#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string>

#include <past/abstract_interpretation.h>
#include <past/emit_c.h>
#include <past/equivalence.h>
#include <past/past_api.h>
#include <past/pprint.h>
#include <past/canonicalization.h>
#include <past/timer.h>
#include "common.h"

#include <pybind11/pybind11.h>

int verify(
    std::string file1_arg,
    std::string file2_arg,
    std::string liveoutvars_arg,
    std::string initvals_arg,
    int is_verbose,
    int is_debug,
    int is_no_build_cdag,
    int is_symbolic_conditionals,
    int is_cdag_all_normalizations,
    int is_cdag_ac_normalizations,
    int is_timing_mode,
    int is_no_cdag_compress,
    int is_no_record_semaphore_history,
    int is_ssa_style,
    int is_enable_subtrees,
    int is_enable_cdag_compression,
    int is_emit_c,
    int is_affine_fold,
    int is_enable_preprocessor
){


  // If liveoutvars_arg or initvals_arg is empty, set to NULL
  char* liveoutvars_cstr = liveoutvars_arg.empty() ? NULL : const_cast<char*>(liveoutvars_arg.c_str());
  char* initvals_cstr = initvals_arg.empty() ? NULL : const_cast<char*>(initvals_arg.c_str());

  // Convert file arguments to char*
  char* f1 = const_cast<char*>(file1_arg.c_str());
  char* f2 = const_cast<char*>(file2_arg.c_str());

  if (is_emit_c)
    printf ("[PAST] Interpret program by abstract interpretation, regenerate C code back.\n");
  else
    printf ("[PAST] Verify equivalence of programs by abstract interpretation.\n");

  if (is_timing_mode)
    {
      past_timer_system_initialize ();
      past_timer_n_start (PAST_TIMER_SYSTEM_PARSE);
    }

  if (is_enable_preprocessor)
    {
      f1 = "._input1.pastchecker.c";
      char buffer[4096];
      char* command =
	"sh -c \"gcc -I . -I \"`pwd`\" -D__PAST_TEST_INTERPRETER_MODE__ -E -P %s | grep -v '^;$' | grep -v '^# [0-9]' > %s\"";
      sprintf (buffer, command, file1_arg, f1);
      system (buffer);
      if (! is_emit_c)
	{
	  f2 = "._input2.pastchecker.c";
	  sprintf (buffer, command, file2_arg, f2);
	  system (buffer);
	}
    }

  s_past_node_t* p1 = past_full_parser (f1);
  s_past_node_t* p2 = NULL;
  if (! is_emit_c)
    p2 = past_full_parser (f2);

  if (is_timing_mode)
    past_timer_n_stop (PAST_TIMER_SYSTEM_PARSE);

  printf ("[PAST] Program P1:\n");
  past_pprint (stdout, p1);
  if (p2)
    {
      printf ("[PAST] Program P2:\n");
      past_pprint (stdout, p2);
    }

  if (!p1 || (! is_emit_c && !p2))
    {
      fprintf (stderr, "[PAST][ERROR] Files could not be parsed.\n");
      exit (1);
    }

  char* live_out = liveoutvars_cstr;
  char* init_vals = initvals_cstr;
  // Prepare the live-out array, from ,-separated strings to separate
  // strings.
  char** live_out_vars = NULL;
  // Prepare array of initial values, if any.
  char** values = NULL;
  int i;
  int repeat;

  // Two identically-treated types of arguments.
  for (repeat = 0; repeat < 2; ++repeat)
    {
      char* input_str = NULL;
      char*** destination = NULL;
      if (repeat == 0 && live_out)
	{
	  input_str = live_out;
	  destination = &live_out_vars;
	}
      else if (repeat == 1 && init_vals)
	{
	  input_str = init_vals;
	  destination = &values;
	}

      if (input_str)
	{
	  int nb_str = 1;
	  for (i = 0; input_str[i] != '\0'; ++i)
	    if (input_str[i] == ',')
	      ++nb_str;
	  // special case: empty live-out set.
	  if (i == 0)
	    nb_str = 0;
	  (*destination) = XMALLOC(char*, nb_str + 1);
	  (*destination)[nb_str] = NULL;
	  int str_pos = 0;
	  // Assume names never bigger than 1k
	  char buffer[1024];
	  int buffer_pos = 0;
	  for (i = 0; input_str[i] != '\0'; ++i)
	    {
	      if (input_str[i] == ',' || input_str[i + 1] == '\0')
		{
		  // Create new string.
		  if (input_str[i + 1] == '\0')
		    buffer[buffer_pos++] = input_str[i];
		  buffer[buffer_pos] = '\0';
		  (*destination)[str_pos] = strdup (buffer);
		  (*destination)[str_pos + 1] = NULL;
		  buffer_pos = 0;
		  str_pos++;
		}
	      else
		buffer[buffer_pos++] = input_str[i];
	    }

	  if (is_verbose)
	    {
	      if (repeat == 0)
		{
		  if (is_emit_c)
		    printf ("[PAST] Live-out variables to be computed: ");
		  else
		    printf ("[PAST] Live-out variables to be checked for equivalence: ");
		}
	      else
		printf ("[PAST] Values available for control-flow: ");
	      for (i = 0; (*destination)[i]; ++i)
		printf ("%s ", (*destination)[i]);
	      printf ("\n");
	    }
	}
    }

  // Launch abstract interpretation.
  int ai_options = 0;

  if (is_debug)
    ai_options = PAST_AI_ENGINE_DUMP_FINAL_STATES;

  if (is_verbose)
    ai_options |= PAST_AI_ENGINE_VERBOSE;

  if (is_timing_mode)
    ai_options |= PAST_AI_ENGINE_TIMING_MODE;

  if (liveoutvars_cstr)
    ai_options |= PAST_AI_ENGINE_EQUIV_CHECK_LIVEOUT_ONLY;

  if (is_no_build_cdag)
    ai_options |= PAST_AI_ENGINE_NO_BUILD_CDAG;

  if (is_symbolic_conditionals)
    ai_options |= PAST_AI_ENGINE_COMPUTE_SYMBOLIC_CONDITIONALS;

  if (is_cdag_ac_normalizations)
    ai_options |= PAST_AI_ENGINE_CDAG_NORMALIZATIONS_AC;

  if (is_cdag_all_normalizations)
    ai_options |= PAST_AI_ENGINE_CDAG_NORMALIZATIONS_ALL;

  if (is_no_cdag_compress)
    ai_options |= PAST_AI_ENGINE_NO_COMPRESS_LIVEIN_AS_VREF;

  if (is_no_record_semaphore_history)
    ai_options |= PAST_AI_ENGINE_NO_RECORD_SEMAPHORE_HISTORY;

  if (is_ssa_style)
    ai_options |= PAST_AI_ENGINE_INPUT_IS_SSA_STYLE_CODE;

  if (is_enable_subtrees)
    ai_options |= PAST_AI_ENGINE_ENABLE_CDAG_SUBTREES;

  if (is_enable_cdag_compression)
    ai_options |= PAST_AI_ENGINE_COMPRESS_CDAG_AS_CODE;

  if (is_emit_c)
    ai_options |= PAST_AI_ENGINE_EMIT_C_CODE;

  if (is_affine_fold)
    ai_options |= PAST_AI_ENGINE_CDAG_AFFINE_FOLD;

  int uok = 0;

  if (p2)
    uok = past_abstract_interpretation_are_equivalent (p1, p2,
						       live_out_vars,
						       values,
						       ai_options);
  else
    uok = past_abstract_interpretation_to_c (p1, live_out_vars, values,
					     ai_options);
  if (uok == -1)
    {
      fprintf (stderr, "[PAST][ERROR] Abstract interpretation failed.\n");
      return -1;
    }
  if (p2)
    {
      if (uok)
	printf ("[PAST] YES => Programs %s and %s are equivalent.\n",
		file1_arg, file2_arg);
      else
	printf ("[PAST] NO => Programs %s and %s are NOT equivalent.\n",
		file1_arg, file2_arg);
    }

  // Be clean.
  past_deep_free (p1);
  past_deep_free (p2);

  return uok; // 1 if equivalent, 0 if not equivalent
}


// Pybind11 module definition
PYBIND11_MODULE(past, m) {
    m.def("verify", &verify, 
        "Verify equivalence of programs by abstract interpretation",
        pybind11::arg("file1_arg"),
        pybind11::arg("file2_arg"),
        pybind11::arg("liveoutvars_arg") = "", // Default values for optional arguments
        pybind11::arg("initvals_arg") = "",    // Default values for optional arguments
        pybind11::arg("is_verbose") = 0,       // Default is_verbose to 0
        pybind11::arg("is_debug") = 0,         // Default is_debug to 0
        pybind11::arg("is_no_build_cdag") = 0, // Default is_no_build_cdag to 0
        pybind11::arg("is_symbolic_conditionals") = 0, // Default is_symbolic_conditionals to 0
        pybind11::arg("is_cdag_all_normalizations") = 0, // Default is_cdag_all_normalizations to 0
        pybind11::arg("is_cdag_ac_normalizations") = 0, // Default is_cdag_ac_normalizations to 0
        pybind11::arg("is_timing_mode") = 0, // Default is_timing_mode to 0
        pybind11::arg("is_no_cdag_compress") = 0, // Default is_no_cdag_compress to 0
        pybind11::arg("is_no_record_semaphore_history") = 0, // Default is_no_record_semaphore_history to 0
        pybind11::arg("is_ssa_style") = 0, // Default is_ssa_style to 0
        pybind11::arg("is_enable_subtrees") = 1, // Default is_enable_subtrees to 1
        pybind11::arg("is_enable_cdag_compression") = 0, // Default is_enable_cdag_compression to 0
        pybind11::arg("is_emit_c") = 0, // Default is_emit_c to 0
        pybind11::arg("is_affine_fold") = 0, // Default is_affine_fold to 0
        pybind11::arg("is_enable_preprocessor") = 0 // Default is_enable_preprocessor to 0
    );
}