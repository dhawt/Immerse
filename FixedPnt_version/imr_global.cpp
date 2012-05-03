/***************************************************************************\
  File: IMR_Global.cpp
  Description: Global settings structure.  No functions - just one variable.
  Author: Daniel Hawthorn
  Modified:
  
\***************************************************************************/
#include "IMR_Global.hpp"

// This variable contains a pointer to the global settings of the interface
// currently in use.  It is updated by the interface before calling other
// interfaces that need access to it's global settings, like the lighting
// module.

IMR_Settings *CurrentSettings;


