/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file Copyright.txt or https://cmake.org/licensing for details.  */
#include "cmAddCxxModuleCommand.h"

#include "cmCustomCommand.h"
#include "cmGeneratorTarget.h"
#include "cmGlobalGenerator.h"
#include "cmLocalGenerator.h"
#include "cmMakefile.h"
#include "cmSourceFile.h"
#include "cmTarget.h"

class cmExecutionStatus;

// cmAddCxxModuleCommand
bool cmAddCxxModuleCommand::InitialPass(std::vector<std::string> const& args,
                                        cmExecutionStatus&)
{
  if (args.empty()) {
    this->SetError("called with an incorrect number of arguments");
    return false;
  }

  auto it = args.begin();
  std::string const& modulename = *it;
  std::string sourcename = modulename + ".cppm";

  ++it;
  if (it != args.end()) {
    // The command takes an extra argument which allows the user to specify
    // that the module is to be built from a different source file.
    sourcename = *it;
    ++it;
  }

  if (it != args.end()) {
    this->SetError("called with an incorrect number of arguments");
    return false;
  }

  const std::string& moduledir =
    this->Makefile->GetSafeDefinition("CMAKE_CXX_MODULE_BINARY_DIR");
  const std::string& moduleflags =
    this->Makefile->GetSafeDefinition("CMAKE_CXX_MODULE_FLAGS_COMPILE");
  if (moduledir.empty() || moduleflags.empty()) {
    this->SetError("using C++ Modules TS requires a C++ compiler that "
                   "supports an experimental implementation of it");
    return false;
  }

  cmTarget* trg = this->Makefile->AddLibrary(
    modulename, cmStateEnums::OBJECT_LIBRARY, {}, true);
  // Set the current target to be compiled as a module.
  trg->InsertCompileOption(moduleflags, this->Makefile->GetBacktrace());

  cmSourceFile* src = trg->AddSource(sourcename);

  // Force CMake to compile the full source path for the source.
  src->GetFullPath();

  // Create a dummy local generator that helps us fetch the output filename.
  cmLocalGenerator* localgen =
    this->Makefile->GetGlobalGenerator()->CreateLocalGenerator(this->Makefile);
  cmGeneratorTarget generator_target(trg, localgen);
  localgen->AddGeneratorTarget(&generator_target);

  const std::string& target_output_dir = cmSystemTools::CollapseCombinedPath(
    this->Makefile->GetCurrentBinaryDirectory(),
    localgen->GetTargetDirectory(&generator_target));

  const std::string& full_output = cmSystemTools::CollapseCombinedPath(
    target_output_dir, generator_target.GetObjectName(src));

  const std::string& comment =
    std::string("Copying module ") + modulename + " to build location";

  const std::string& module_out_path = cmSystemTools::CollapseCombinedPath(
    moduledir,
    modulename +
      this->Makefile->GetSafeDefinition("CMAKE_CXX_MODULE_BINARY_EXTENSION"));

  cmCustomCommandLines commands;

  cmCustomCommandLine make_folder;
  cmCustomCommandLine copy_module_file;
#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
  make_folder.emplace_back("mkdir");
  make_folder.emplace_back(std::string("\"") + moduledir + "\"");

  copy_module_file.emplace_back("copy");
  copy_module_file.emplace_back(std::string("\"") + full_output + "\"");
  copy_module_file.emplace_back(std::string("\"") + module_out_path + "\"");
#else /* Something unix-y */
  make_folder.emplace_back("mkdir");
  make_folder.emplace_back("-p");
  make_folder.emplace_back(std::string("\"") + moduledir + "\"");

  copy_module_file.emplace_back("cp");
  copy_module_file.emplace_back(std::string("\"") + full_output + "\"");
  copy_module_file.emplace_back(std::string("\"") + module_out_path + "\"");
#endif
  commands.push_back(make_folder);
  commands.push_back(copy_module_file);

  cmCustomCommand copyOverCommand(this->Makefile, { module_out_path }, {},
                                  { full_output }, commands, comment.c_str(),
                                  target_output_dir.c_str());

  trg->AddPostBuildCommand(copyOverCommand);
  this->Makefile->AppendProperty("ADDITIONAL_MAKE_CLEAN_FILES",
                                 moduledir.c_str());

  return true;
}
