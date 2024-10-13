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

#include <past/abstract_interpretation.h>
#include <past/past_api.h>
#include <past/pprint.h>
// #include "common.h"

#include <pybind11/pybind11.h>

// int
// main (int argc, char * const argv[])
int verify(
  std::string file1_arg,
  std::string file2_arg,
  std::string liveoutvars_arg,
  std::string initvals_arg,
  int is_verbose,
  int is_debug
){
  printf ("[PAST] Verify equivalence of programs by abstract interpretation.\n");
//   printf ("Usage: %s <filename1.c> <filename2.c> [<\"live,out,vars\"> [<\"some,init,values\">]]\n", argv[0]);
  
  // If liveoutvars_arg or initvals_arg is empty, set to NULL
  char* liveoutvars_cstr = liveoutvars_arg.empty() ? NULL : const_cast<char*>(liveoutvars_arg.c_str());
  char* initvals_cstr = initvals_arg.empty() ? NULL : const_cast<char*>(initvals_arg.c_str());

  // Convert file arguments to char*
  char* f1 = const_cast<char*>(file1_arg.c_str());
  char* f2 = const_cast<char*>(file2_arg.c_str());
  s_past_node_t* p1 = past_full_parser (f1);
  s_past_node_t* p2 = past_full_parser (f2);

  printf ("[PAST] Program P1:\n");
  past_pprint (stdout, p1);
  printf ("[PAST] Program P2:\n");
  past_pprint (stdout, p2);

  if (!p1 || !p2)
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
	      printf ("[PAST] Live-out variables to be checked for equivalence: ");
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

  if (liveoutvars_cstr)
    ai_options |= PAST_AI_ENGINE_EQUIV_CHECK_LIVEOUT_ONLY;

  ai_options |= PAST_AI_ENGINE_COMPUTE_SYMBOLIC_CONDITIONALS;

  int uok = past_abstract_interpretation_are_equivalent (p1, p2,
							 live_out_vars,
							 values,
							 ai_options);
  if (uok == -1)
    {
      fprintf (stderr, "[PAST][ERROR] Abstract interpretation failed.\n");
      exit (1);
    }
  if (uok)
    printf ("[PAST] YES => Programs %s and %s are equivalent.\n", f1, f2);
  else
    printf ("[PAST] NO => Programs %s and %s are NOT equivalent.\n", f1, f2);

  // Be clean.
  past_deep_free (p1);
  past_deep_free (p2);

  return 0;
}

// Pybind11 module definition
PYBIND11_MODULE(past, m) {
    m.def("verify", &verify, 
          "Verify equivalence of programs by abstract interpretation",
          pybind11::arg("file1_arg"),
          pybind11::arg("file2_arg"),
          pybind11::arg("liveoutvars_arg") = "",   // Default values for optional arguments
          pybind11::arg("initvals_arg") = "",      // Default values for optional arguments
          pybind11::arg("is_verbose") = 0,         // Default is_verbose to 0
          pybind11::arg("is_debug") = 0);          // Default is_debug to 0
}