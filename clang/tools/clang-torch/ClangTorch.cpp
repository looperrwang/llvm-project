//looperwang@LOOPERWANG-MC3 WeMeetApp % xcodebuild -project "WeMeetApp.xcodeproj" -scheme dailybuild -arch arm64 -configuration Debug | xcpretty -r json-compilation-database
// PPCallbacks - MacroExpands / MacroDefined / MacroUndefined
// https://opensource.apple.com/source/clang/clang-425.0.24/src/tools/clang/
// http://swtv.kaist.ac.kr/courses/cs453-fall13/Clang%20tutorial%20v4.pdf
// http://clang.llvm.org/
// http://clang.llvm.org/doxygen/
// http://amnoid.de/tmp/clangtut/tut.html

// https://clang.llvm.org/docs/UsersManual.html
// https://clang.llvm.org/docs/InternalsManual.html#the-ast-library
// https://clang.llvm.org/docs/index.html

// xcpretty bug - https://github.com/xcpretty/xcpretty/issues/302

#include "llvm/Support/CommandLine.h"
#include "clang/Tooling/JSONCompilationDatabase.h"
#include "clang/Tooling/Tooling.h"
#include "clang/Frontend/FrontendActions.h"
#include "llvm/Support/JSON.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/Error.h"
#include "llvm/Support/Path.h"
#include "llvm/Support/raw_ostream.h"

#include <iostream>
#include <fstream>

static llvm::cl::OptionCategory s_torch_category("Torch options");
static llvm::cl::opt<std::string> s_dbpath("dbpath", // ArgStr
                                           llvm::cl::desc("<compilation_db.json filepath>"), //HelpStr
                                           llvm::cl::value_desc("string"), //ValueStr
                                           llvm::cl::Required,
                                           llvm::cl::cat(s_torch_category));

static clang::tooling::ArgumentsAdjuster getTorchAdjuster() {
  return [](const clang::tooling::CommandLineArguments& Args,
            llvm::StringRef FileName) {
    clang::tooling::CommandLineArguments AdjustedArgs;
    for (size_t i = 0, e = Args.size(); i < e; ++i) {
      llvm::StringRef Arg = Args[i];
      if (Arg == "-index-store-path") {
        ++i;
        continue;
      }
      if ("-gmodules" == Arg) {
        continue;
      }
      AdjustedArgs.push_back(Args[i]);
    }
    return AdjustedArgs;
  };
}

class TorchConsumer : public clang::ASTConsumer {
  using super_class = clang::ASTConsumer;
protected:
  void Initialize(clang::ASTContext &Context) override;
  bool HandleTopLevelDecl(clang::DeclGroupRef D) override;
  void HandleInlineFunctionDefinition(clang::FunctionDecl *D) override;
  void HandleInterestingDecl(clang::DeclGroupRef D) override;
  void HandleTranslationUnit(clang::ASTContext &Ctx) override;
  void HandleTagDeclDefinition(clang::TagDecl *D) override;
  void HandleTagDeclRequiredDefinition(const clang::TagDecl *D) override;
  void HandleCXXImplicitFunctionInstantiation(clang::FunctionDecl *D) override;
  void HandleTopLevelDeclInObjCContainer(clang::DeclGroupRef D) override;
  void HandleImplicitImportDecl(clang::ImportDecl *D) override;
  void CompleteTentativeDefinition(clang::VarDecl *D) override;
  void CompleteExternalDeclaration(clang::VarDecl *D) override;
  void AssignInheritanceModel(clang::CXXRecordDecl *RD) override;
  void HandleCXXStaticMemberVarInstantiation(clang::VarDecl *D) override;
  void HandleVTable(clang::CXXRecordDecl *RD) override;
  clang::ASTMutationListener *GetASTMutationListener() override;
  clang::ASTDeserializationListener *GetASTDeserializationListener() override;
  void PrintStats() override;
  bool shouldSkipFunctionBody(clang::Decl *D) override;
};

void TorchConsumer::Initialize(clang::ASTContext &Context) {
  super_class::Initialize(Context);
}

bool TorchConsumer::HandleTopLevelDecl(clang::DeclGroupRef D) {
  return super_class::HandleTopLevelDecl(D);
}

void TorchConsumer::HandleInlineFunctionDefinition(clang::FunctionDecl *D) {
  super_class::HandleInlineFunctionDefinition(D);
}

void TorchConsumer::HandleInterestingDecl(clang::DeclGroupRef D) {
  super_class::HandleInterestingDecl(D);
}

void TorchConsumer::HandleTranslationUnit(clang::ASTContext &Ctx) {
  super_class::HandleTranslationUnit(Ctx);
}

void TorchConsumer::HandleTagDeclDefinition(clang::TagDecl *D) {
  super_class::HandleTagDeclDefinition(D);
}

void TorchConsumer::HandleTagDeclRequiredDefinition(const clang::TagDecl *D) {
  super_class::HandleTagDeclRequiredDefinition(D);
}

void TorchConsumer::HandleCXXImplicitFunctionInstantiation(clang::FunctionDecl *D) {
  super_class::HandleCXXImplicitFunctionInstantiation(D);
}

void TorchConsumer::HandleTopLevelDeclInObjCContainer(clang::DeclGroupRef D) {
  super_class::HandleTopLevelDeclInObjCContainer(D);
}

void TorchConsumer::HandleImplicitImportDecl(clang::ImportDecl *D) {
  super_class::HandleImplicitImportDecl(D);
}

void TorchConsumer::CompleteTentativeDefinition(clang::VarDecl *D) {
  super_class::CompleteTentativeDefinition(D);
}

void TorchConsumer::CompleteExternalDeclaration(clang::VarDecl *D) {
  super_class::CompleteExternalDeclaration(D);
}

void TorchConsumer::AssignInheritanceModel(clang::CXXRecordDecl *RD) {
  super_class::AssignInheritanceModel(RD);
}

void TorchConsumer::HandleCXXStaticMemberVarInstantiation(clang::VarDecl *D) {
  super_class::HandleCXXStaticMemberVarInstantiation(D);
}

void TorchConsumer::HandleVTable(clang::CXXRecordDecl *RD) {
  super_class::HandleVTable(RD);
}

clang::ASTMutationListener *TorchConsumer::GetASTMutationListener() {
  return super_class::GetASTMutationListener();
}

clang::ASTDeserializationListener *TorchConsumer::GetASTDeserializationListener() {
  return super_class::GetASTDeserializationListener();
}

void TorchConsumer::PrintStats() {
  super_class::PrintStats();
}

bool TorchConsumer::shouldSkipFunctionBody(clang::Decl *D) {
  return super_class::shouldSkipFunctionBody(D);
}

class TorchAction : public clang::ASTFrontendAction {
protected:
  bool PrepareToExecuteAction(clang::CompilerInstance &CI) override;
  std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(clang::CompilerInstance &CI,
                                                        llvm::StringRef InFile) override;
  bool BeginInvocation(clang::CompilerInstance &CI) override;
  bool BeginSourceFileAction(clang::CompilerInstance &CI) override;
  void ExecuteAction() override;
  void EndSourceFileAction() override;
  bool shouldEraseOutputFiles() override;
};

bool TorchAction::PrepareToExecuteAction(clang::CompilerInstance &CI) { //1
  printf("looper_test, PrepareToExecuteAction\n");
  return true;
}

std::unique_ptr<clang::ASTConsumer> TorchAction::CreateASTConsumer(clang::CompilerInstance &CI,
                                                                   llvm::StringRef InFile) { //4
  printf("looper_test, CreateASTConsumer\n");
  return std::make_unique<TorchConsumer>();
}

bool TorchAction::BeginInvocation(clang::CompilerInstance &CI) { //2
  printf("looper_test, BeginInvocation\n");
  return true;
}

bool TorchAction::BeginSourceFileAction(clang::CompilerInstance &CI) { //3
  printf("looper_test, BeginSourceFileAction\n");
  return true;
}

void TorchAction::ExecuteAction() { //5
  printf("looper_test, ExecuteAction\n");
}

void TorchAction::EndSourceFileAction() { //6
  printf("looper_test, EndSourceFileAction\n");
}

bool TorchAction::shouldEraseOutputFiles() { //7
  printf("looper_test, shouldEraseOutputFiles\n");
  return true;
}

class TorchFrontendActionFactory : public clang::tooling::FrontendActionFactory {
protected:
//  bool runInvocation(std::shared_ptr<clang::CompilerInvocation> invocation,
//                     clang::FileManager* files,
//                     std::shared_ptr<clang::PCHContainerOperations> PCHContainerOps,
//                     clang::DiagnosticConsumer *diagConsumer) override;
  std::unique_ptr<clang::FrontendAction> create() override;
};

//bool TorchFrontendActionFactory::runInvocation(std::shared_ptr<clang::CompilerInvocation> invocation,
//                                               clang::FileManager* files,
//                                               std::shared_ptr<clang::PCHContainerOperations> PCHContainerOps,
//                                               clang::DiagnosticConsumer *diagConsumer) {
//  return true;
//}

std::unique_ptr<clang::FrontendAction> TorchFrontendActionFactory::create() {
  return std::make_unique<TorchAction>();
}

class TorchDriver {
public:
  TorchDriver(llvm::StringRef dbpath)
    : dbpath_(dbpath) {
  }
  TorchDriver(std::unique_ptr<clang::tooling::JSONCompilationDatabase> compilation_database)
    : compilation_database_(std::move(compilation_database)) {
  }
public:
  int Run();
private:
  void PrintCompilationDatabase();
  void PreProcess();
  void PostProcess();
public:
  llvm::StringRef dbpath_;
  std::unique_ptr<clang::tooling::JSONCompilationDatabase> compilation_database_;
};

void TorchDriver::PrintCompilationDatabase() {
  const auto all_files = compilation_database_->getAllFiles();
  for (decltype(all_files.size()) index = 0; index < all_files.size(); index++) {
    auto exists = true;
    if (!llvm::sys::fs::exists(all_files[index])) {
      exists = false;
      std::cout << "file[" << index << "] = " << all_files[index] << std::endl;
    }
    const auto compile_commands = compilation_database_->getCompileCommands(all_files[index]);
    for (const auto& command : compile_commands) {
      if (!command.Directory.empty()) {
        if (!exists) {
          std::cout << "    " << "- directory = " << command.Directory << std::endl;
        }
      }
      if (!command.Filename.empty()) {
        if (!exists) {
          std::cout << "    " << "- Filename  = " << command.Filename << std::endl;
        }
      }
//      if (!command.Output.empty()) {
//        std::cout << "    " << "- Output    = " << command.Output << std::endl;
//      }
//      if (!command.Heuristic.empty()) {
//        std::cout << "    " << "- Heuristic = " << command.Heuristic << std::endl;
//      }
//      std::cout << "- CommandLine =" << std::endl;
//      for (const auto& command_line : command.CommandLine) {
//        std::cout << "    " << "- " << command_line << std::endl;
//      }
    }
  }
}

void TorchDriver::PreProcess() {
  llvm::Expected<llvm::sys::fs::file_t> file_t = llvm::sys::fs::openNativeFileForRead(dbpath_);
  if (!file_t) {
    llvm::report_fatal_error("open json-compilation-database file fail, path=" + dbpath_);
    return;
  }
  
  uint64_t file_size = 0;
  if (llvm::sys::fs::file_size(dbpath_, file_size)) {
    llvm::report_fatal_error("get json-compilation-database file size fail, path=" + dbpath_);
    return;
  }
  
  std::string content(file_size, '?');
  llvm::Expected<size_t> read_size = llvm::sys::fs::readNativeFile(*file_t, llvm::makeMutableArrayRef(&*content.begin(), static_cast<size_t>(file_size)));
  if (file_size != *read_size) {
    llvm::report_fatal_error("read json-compilation-database file fail, path=" + dbpath_);
    return;
  }
  
  llvm::Expected<llvm::json::Value> json_value = llvm::json::parse(content);
  if (!json_value) {
    llvm::report_fatal_error("parse json-compilation-database file to json fail, path=" + dbpath_);
    return;
  }
  
  llvm::json::Array* json_array = json_value->getAsArray();
  if (!json_array) {
    llvm::report_fatal_error("json-compilation-database file content error, path=" + dbpath_);
    return;
  }
  
  bool changed = false;
  for (llvm::json::Array::iterator iter = json_array->begin(); iter != json_array->end(); iter++) {
    llvm::json::Value& item = *iter;
    llvm::json::Object* object = item.getAsObject();
    if (!object) {
      llvm::report_fatal_error("json-compilation-database item error");
      continue;
    }
    llvm::Optional<llvm::StringRef> directory = object->getString("directory");
    if (!directory) {
      llvm::report_fatal_error("json-compilation-database directory error");
      continue;
    }
    llvm::Optional<llvm::StringRef> file = object->getString("file");
    if (!file) {
      llvm::report_fatal_error("json-compilation-database file error");
      continue;
    }
    
    const bool is_directory = llvm::sys::fs::is_directory(*directory);
    const bool is_absolute = llvm::sys::path::is_absolute(*directory);
    const bool is_exist = llvm::sys::fs::exists(*directory);
    const bool has_filename = llvm::sys::path::has_filename(*directory);
    if (!is_directory && is_absolute && is_exist && has_filename) {
      llvm::StringRef parent_path = llvm::sys::path::parent_path(*directory);
      llvm::json::Value* DirectoryValue = object->get("directory");
      *DirectoryValue = llvm::json::Value(parent_path.str());
      
      llvm::StringRef filename = llvm::sys::path::filename(*directory);
      llvm::json::Value* FileValue = object->get("file");
      *FileValue = llvm::json::Value(filename.str());
      changed = true;
    }
    
//    llvm::SmallString<128> NativeFilePath;
//    if (llvm::sys::path::is_relative(*file)) {
//      llvm::SmallString<8> absolute_path(*directory);
//      llvm::sys::path::append(absolute_path, *file);
//      llvm::sys::path::remove_dots(absolute_path);
//      llvm::sys::path::native(absolute_path, NativeFilePath);
//    } else {
//      llvm::sys::path::native(*file, NativeFilePath);
//    }
  }
  
  if (changed) {
    std::string json_str;
    llvm::raw_string_ostream OS(json_str);
    llvm::json::OStream J(OS);
    J.array([&] {
      for (const llvm::json::Value& item : *json_array) {
        J.object([&] {
          const llvm::json::Object* object = item.getAsObject();
          for (const llvm::json::Object::value_type& item : *object) {
            J.attribute(item.getFirst(), item.getSecond());
          }
        });
      }
    });
    
  //  llvm::StringRef parent_path = llvm::sys::path::parent_path(dbpath_);
  //  llvm::sys::fs::create_directories(parent_path);
  //  std::string path = parent_path.str() + "compilation_db_new.json";
    std::ofstream os(dbpath_.str());
    os << OS.str();
    os.close();
  }
  
  std::string error_message;
  compilation_database_ = clang::tooling::JSONCompilationDatabase::loadFromFile(dbpath_,
                                                                                error_message,
                                                                                clang::tooling::JSONCommandLineSyntax::AutoDetect);
  if (!compilation_database_) {
    llvm::report_fatal_error("compilation database load error, path=" + dbpath_);
  }
}

void TorchDriver::PostProcess() {
}

int TorchDriver::Run() {
  PreProcess();
  
  PrintCompilationDatabase();
  
  clang::tooling::ClangTool tool(*compilation_database_, compilation_database_->getAllFiles());
  tool.appendArgumentsAdjuster(getTorchAdjuster());
  TorchFrontendActionFactory factory;
  int error = 0;
  error |= tool.run(&factory);
  if (error) {
    llvm::report_fatal_error("run error, path=" + dbpath_ + ", error=" + std::to_string(error));
  }
  
  PostProcess();
  
  return error;
}

class TorchModel {
public:
  TorchModel(const std::string& filepath) : filepath_(filepath) {
  }
private:
  const std::string filepath_;
};

int main(int argc, const char** argv) {
  std::cout << "argc = " << argc << std::endl;
  for (int index = 0; index < argc; index++) {
    std::cout << "argv[" << index << "] = " << *(argv + index) << std::endl;
  }
  
  if (!llvm::cl::ParseCommandLineOptions(argc, argv)) {
    std::cout << "parse command line options error." << std::endl;
    return 1;
  }
  
  const auto dbpath = s_dbpath.getValue();
  TorchDriver driver(dbpath);
  
  return driver.Run();
}
