/*=========================================================================

  Program:   CMake - Cross-Platform Makefile Generator
  Module:    $RCSfile$
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Kitware, Inc., Insight Consortium.  All rights reserved.
  See Copyright.txt or http://www.cmake.org/HTML/Copyright.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "cmakewizard.h"
#include "cmake.h"
#include "cmCacheManager.h"
#include "cmDynamicLoader.h"
#include "cmListFileCache.h"
#include "cmDocumentation.h"

//----------------------------------------------------------------------------
static const cmDocumentationEntry cmDocumentationName[] =
{
  {0,
   "  cmake - Cross-Platform Makefile Generator.", 0},
  {0,0,0}
};

//----------------------------------------------------------------------------
static const cmDocumentationEntry cmDocumentationUsage[] =
{
  {0,
   "  cmake [options] <path-to-source>", 0},
  {0,0,0}
};

//----------------------------------------------------------------------------
static const cmDocumentationEntry cmDocumentationDescription[] =
{
  {0,
   "CMake reads ... ", 0},
  {0,0,0}
};

//----------------------------------------------------------------------------
static const cmDocumentationEntry cmDocumentationOptions[] =
{
  CMAKE_STANDARD_OPTIONS_TABLE,
  {"-i", "Run in wizard mode.",
   "Wizard mode runs cmake interactively without a GUI.  The user is "
   "prompted to answer questions about the project configuration.  "
   "The answers are used to set cmake cache values."},
  {0,0,0}
};

//----------------------------------------------------------------------------
static const cmDocumentationEntry cmDocumentationNOTE[] =
{
  {0,
   "CMake no longer configures a project when run with no arguments.  "
   "In order to configure the project in the current directory, run\n"
   "  cmake .", 0},
  {0,0,0}
};

int do_cmake(int ac, char** av);
void updateProgress(const char *msg, float prog, void *cd);

int main(int ac, char** av)
{
  cmSystemTools::EnableMSVCDebugHook();
  int ret = do_cmake(ac, av);
#ifdef CMAKE_BUILD_WITH_CMAKE
  cmDynamicLoader::FlushCache();
#endif
  cmListFileCache::GetInstance()->ClearCache(); 
  return ret;
}

int do_cmake(int ac, char** av)
{
  cmDocumentation doc;
  if(cmDocumentation::Type ht = doc.CheckOptions(ac, av))
    {
    // Construct and print requested documentation.
    cmake hcm;
    std::vector<cmDocumentationEntry> commands;
    hcm.GetCommandDocumentation(commands);
    doc.SetNameSection(cmDocumentationName);
    doc.SetUsageSection(cmDocumentationUsage);
    doc.SetDescriptionSection(cmDocumentationDescription);
    doc.SetOptionsSection(cmDocumentationOptions);
    doc.SetCommandsSection(&commands[0]);
    doc.PrintDocumentation(ht, std::cout);
  
    // If we were run with no arguments, but a CMakeLists.txt file
    // exists, the user may have been trying to use the old behavior
    // of cmake to build a project in-source.  Print a message
    // explaining the change to standard error and return an error
    // condition in case the program is running from a script.
    if((ac == 1) && cmSystemTools::FileExists("CMakeLists.txt"))
      {
      doc.ClearSections();
      doc.AddSection("NOTE", cmDocumentationNOTE);
      doc.Print(cmDocumentation::UsageForm, std::cerr);
      return 1;
      }
    return 0;
    }
  
  bool wiz = false;
  bool command = false;
  std::vector<std::string> args;
  for(int i =0; i < ac; ++i)
    {
    if(strcmp(av[i], "-i") == 0)
      {
      wiz = true;
      }
    else if (strcmp(av[i], "-E") == 0)
      {
      command = true;
      }
    else 
      {
      args.push_back(av[i]);
      }
    }

  if(command)
    {
    int ret = cmake::CMakeCommand(args);
    return ret;
    }
  if (wiz)
    {
    cmakewizard wizard;
    wizard.RunWizard(args); 
    return 0;
    }
  cmake cm;  
  cm.SetProgressCallback(updateProgress, 0);
  return cm.Run(args); 
}

void updateProgress(const char *msg, float prog, void*)
{
  if ( prog < 0 )
    {
    std::cout << "-- " << msg << std::endl;
    }
  //else
  //{
  //std::cout << "-- " << msg << " " << prog << std::endl;
  //}
}
