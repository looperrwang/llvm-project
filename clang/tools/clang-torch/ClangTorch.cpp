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
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Lex/PreprocessorOptions.h"
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

class TorchImportInfo {
public:
  std::string name_;
  bool is_valid_;
  bool is_file_id_;
  bool is_macro_id_;
  std::string des_;
  std::string file_name_;
  
  std::string test_string_;
  unsigned test_unsigned_;
};

class TorchModel {
public:
  TorchModel(const std::string& filepath) : filepath_(filepath) {
  }
public:
  std::vector<TorchImportInfo> import_infos_;
private:
  const std::string filepath_;
};

static std::vector<std::shared_ptr<TorchModel>> torch_models;

class TorchConsumer : public clang::ASTConsumer {
  using super_class = clang::ASTConsumer;
  
public:
  TorchConsumer(TorchModel& torch_model) : torch_model_(torch_model) {
  }
  
protected:
  void Initialize(clang::ASTContext &Context) override;
  bool HandleTopLevelDecl(clang::DeclGroupRef D) override;
  void HandleInlineFunctionDefinition(clang::FunctionDecl *D) override;
//  void HandleInterestingDecl(clang::DeclGroupRef D) override;
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
  
public:
  TorchModel& torch_model_;
};

void TorchConsumer::Initialize(clang::ASTContext &Context) {
  super_class::Initialize(Context);
}

bool TorchConsumer::HandleTopLevelDecl(clang::DeclGroupRef D) {
//  for (clang::Decl* item : D) {
//    item->dump();
//  }
  return super_class::HandleTopLevelDecl(D);
}

void TorchConsumer::HandleInlineFunctionDefinition(clang::FunctionDecl *D) {
  super_class::HandleInlineFunctionDefinition(D);
}

//void TorchConsumer::HandleInterestingDecl(clang::DeclGroupRef D) {
//  for (clang::Decl* item : D) {
//    item->dump();
//  }
//  super_class::HandleInterestingDecl(D);
//}

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

// 最先检测到的是 pch 文件中的 #import / #include
void TorchConsumer::HandleImplicitImportDecl(clang::ImportDecl *D) {
  //D->dump();
  return super_class::HandleImplicitImportDecl(D);
  
  
  
  if (D->isImplicit()) {
    //隐式声明，如 pch 中的声明
    super_class::HandleImplicitImportDecl(D);
    return;
  }
  
  
  
  
  TorchImportInfo info;
  {
    // 被引的 module 信息
    clang::Module* imported_module = D->getImportedModule();
    info.name_ = imported_module->Name;
    info.is_valid_ = imported_module->DefinitionLoc.isValid();
    info.is_file_id_ = imported_module->DefinitionLoc.isFileID();
    info.is_macro_id_ = imported_module->DefinitionLoc.isMacroID();
    info.des_ = imported_module->DefinitionLoc.printToString(D->getASTContext().getSourceManager()); //file + line + column
    info.file_name_ = D->getASTContext().getSourceManager().getFilename(imported_module->DefinitionLoc).str();
    info.test_string_ = D->getASTContext().getSourceManager().getModuleImportLoc(imported_module->DefinitionLoc).second.str();
    info.test_string_ = D->getASTContext().getSourceManager().getCharacterData(imported_module->DefinitionLoc); //文件内容片段
    info.test_unsigned_ = D->getASTContext().getSourceManager().getSpellingColumnNumber(imported_module->DefinitionLoc); //column
    info.test_unsigned_ = D->getASTContext().getSourceManager().getExpansionColumnNumber(imported_module->DefinitionLoc); //column
    info.test_unsigned_ = D->getASTContext().getSourceManager().getPresumedColumnNumber(imported_module->DefinitionLoc); //column
    info.test_unsigned_ = D->getASTContext().getSourceManager().getSpellingLineNumber(imported_module->DefinitionLoc); //line
    info.test_unsigned_ = D->getASTContext().getSourceManager().getExpansionLineNumber(imported_module->DefinitionLoc); //line
    info.test_unsigned_ = D->getASTContext().getSourceManager().getPresumedLineNumber(imported_module->DefinitionLoc); //line
    
//    const clang::FileEntry* file_entry = imported_module->Umbrella.get<const clang::FileEntry *>();
//    if (file_entry) {
//    }
//    const clang::DirectoryEntry* directory_entry = imported_module->Umbrella.get<const clang::DirectoryEntry *>();
//    if (directory_entry) {
//    }
    
    info.test_string_ = imported_module->getFullModuleName();
    info.test_string_ = imported_module->getUmbrellaDir().NameAsWritten;
    info.test_string_ = imported_module->getUmbrellaDir().Entry->getName().str();
  }
  
  llvm::ArrayRef<clang::SourceLocation> identifier_locs = D->getIdentifierLocs();
  for (const clang::SourceLocation& loc : identifier_locs) {
    info.test_string_ = D->getASTContext().getSourceManager().getFilename(loc).str();
  }
  
  //</Users/looperwang/Desktop/WeMeet/app/app/iOS/Src/App/WeMeet/Meet/WeMeetPrefixHeader.pch:7:1>
  //可以区分 .pch
  info.test_string_ = D->getSourceRange().printToString(D->getASTContext().getSourceManager());
  info.test_string_ = D->getASTContext().getSourceManager().getFilename(D->getLocation()).str();
  
  clang::Decl::Kind kind = D->getKind();
  info.test_string_ = D->getDeclKindName();
  clang::Decl* next_decl = D->getNextDeclInContext();
  if (next_decl) {
    info.test_string_ = next_decl->getSourceRange().printToString(D->getASTContext().getSourceManager());
  }
  clang::DeclContext* decl_context = D->getDeclContext();
  kind = decl_context->getDeclKind();
  info.test_string_ = decl_context->getDeclKindName();
  if (decl_context->getParent()) {
    kind = decl_context->getParent()->getDeclKind();
  }
  if (decl_context->getLexicalParent()) {
    kind = decl_context->getParent()->getDeclKind();
  }
  if (decl_context->getLookupParent()) {
    kind = decl_context->getLookupParent()->getDeclKind();
  }
  //当前 Decl 是否在 block 中
  if (decl_context->isClosure()) {
    //获取最内层的 block
    decl_context->getInnermostBlockDecl();
  }
  // 当前 decl 是否在 oc 容器中
  decl_context->isObjCContainer();
  //当前 decl 是否在 函数 中
  decl_context->isFunctionOrMethod();
  //isLookupContext/isFileContext/isTranslationUnit/isRecord/isNamespace/isStdNamespace/isInlineNamespace/isDependentContext
  //isTransparentContext/isExternCContext/isExternCXXContext
  decl_context->isExternCContext(); //extern "C"
  //可以判断两个 context 是否有包含关系
  //containsDecl
  
  //比 print 好用
  //decl_context->dumpDeclContext();
  
//  for (clang::DeclContext::decl_iterator iter = decl_context->decls_begin(); iter != decl_context->decls_end(); iter++) {
//    clang::Decl* decl = *iter;
//    info.test_string_ = decl->getDeclKindName();
//    info.test_string_ = decl->getLocation().printToString(decl->getASTContext().getSourceManager());
//  }
  
  clang::TranslationUnitDecl* translation_unit_decl = D->getTranslationUnitDecl();
  translation_unit_decl->getLocation();
  info.test_string_ = D->getASTContext().getSourceManager().getFilename(D->getLocation()).str();
  
  // getAccess 获取 private/public
  // getAttrs - 属性
  //hasBody() getBody() - 函数/block 执行体
  D->dump();
  
  /*
   - getSpellingLoc，获取源码中的位置
   */
  
  torch_model_.import_infos_.push_back(std::move(info));
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
private:
  using this_class = TorchAction;
  using super_class = clang::ASTFrontendAction;
protected:
  bool PrepareToExecuteAction(clang::CompilerInstance &CI) override;
  std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(clang::CompilerInstance &CI,
                                                        llvm::StringRef InFile) override;
  bool BeginInvocation(clang::CompilerInstance &CI) override;
  bool BeginSourceFileAction(clang::CompilerInstance &CI) override;
  void ExecuteAction() override;
  void EndSourceFileAction() override;
  bool shouldEraseOutputFiles() override;
  
  void Study(clang::CompilerInstance& CI);
};

void TorchAction::Study(clang::CompilerInstance& CI) {
  clang::CompilerInstance& compiler_instance = getCompilerInstance();
  if (&compiler_instance != &CI) {
    llvm::report_fatal_error("PrepareToExecuteAction, CI");
  }
  if (compiler_instance.hasInvocation()) {
    clang::CompilerInvocation& invocation = compiler_instance.getInvocation();
    std::shared_ptr<clang::LangOptions> lang_opts = invocation.LangOpts;
    clang::LangStandard::Kind kind = lang_opts->LangStd;
    std::cout << "kind=[" << kind << "]" << std::endl;
    if (lang_opts->Sanitize.Mask) {
    }
    std::vector<std::string> files = lang_opts->NoSanitizeFiles;
    if (!files.empty()) {
      std::cout << "" << std::endl;
    }
    files = lang_opts->XRayAttrListFiles;
    if (!files.empty()) {
      std::cout << "" << std::endl;
    }
    files = lang_opts->ProfileListFiles;
    if (!files.empty()) {
      std::cout << "" << std::endl;
    }
    clang::ObjCRuntime& objc_runtime = lang_opts->ObjCRuntime;
    clang::ObjCRuntime::Kind objc_kind = objc_runtime.getKind();
    std::cout << "kind=[" << objc_kind << "]" << std::endl;
    std::string version_string = objc_runtime.getVersion().getAsString();
    std::cout << "version=[" << version_string << "]" << std::endl;
    clang::LangOptions::CoreFoundationABI cf_runtime = lang_opts->CFRuntime;
    std::cout << "cf_runtime=[" << (int)cf_runtime << "]" << std::endl;
    std::string constant_string_class = lang_opts->ObjCConstantStringClass;
    if (!constant_string_class.empty()) {
      std::cout << "constant_string_class=[" << constant_string_class << "]" << std::endl;
    }
    std::string overflow_handler = lang_opts->OverflowHandler;
    if (!overflow_handler.empty()) {
      std::cout << "constant_string_class=[" << constant_string_class << "]" << std::endl;
    }
    std::string module_name = lang_opts->ModuleName;
    if (!module_name.empty()) {
      //std::cout << "module_name=[" << module_name << "]" << std::endl;
    }
    std::string current_module = lang_opts->CurrentModule;
    if (!current_module.empty()) {
      //std::cout << "current_module=[" << current_module << "]" << std::endl;
    }
    std::vector<std::string>& module_features = lang_opts->ModuleFeatures;
    if (!module_features.empty()) {
      std::cout << "";
    }
    clang::CommentOptions::BlockCommandNamesTy block_command_names = lang_opts->CommentOpts.BlockCommandNames;
    if (!block_command_names.empty()) {
      std::cout << "";
    }
    std::vector<std::string>& no_builtin_funcs = lang_opts->NoBuiltinFuncs;
    if (!no_builtin_funcs.empty()) {
      std::cout << "";
    }
    std::vector<llvm::Triple> omp_target_triples = lang_opts->OMPTargetTriples;
    if (!omp_target_triples.empty()) {
      std::cout << "";
    }
    std::string omp_host_ir_file = lang_opts->OMPHostIRFile;
    if (!omp_host_ir_file.empty()) {
      std::cout << "";
    }
    std::string cuid = lang_opts->CUID;
    if (!cuid.empty()) {
      std::cout << "";
    }
    if (lang_opts->IsHeaderFile) {
      std::cout << "";
    }
    std::shared_ptr<clang::TargetOptions> target_options = invocation.TargetOpts;
    if (target_options) {
    }
  }
  
  /*
   if (!isCurrentFileAST()) {
   llvm::report_fatal_error("PrepareToExecuteAction, isCurrentFileAST");
   }*/
  
  {
    const clang::FrontendInputFile& current_input = this->getCurrentInput();
    clang::InputKind input_kind = current_input.getKind();
    clang::Language language = input_kind.getLanguage();
    std::cout << "language=[" << (uint8_t)language << "]" << std::endl;
    clang::InputKind::Format format = input_kind.getFormat();
    std::cout << "format=[" << format << "]" << std::endl;
    bool is_preprocessed = input_kind.isPreprocessed();
    std::cout << "is_preprocessed=[" << is_preprocessed << "]" << std::endl;
    bool is_unknown = input_kind.isUnknown();
    std::cout << "is_unknown=[" << is_unknown << "]" << std::endl;
    bool is_objective_c = input_kind.isObjectiveC();
    std::cout << "is_objective_c=[" << is_objective_c << "]" << std::endl;
    
    bool is_system = current_input.isSystem();
    std::cout << "is_system=[" << is_system << "]" << std::endl;
    bool is_empty = current_input.isEmpty();
    std::cout << "is_empty=[" << is_empty << "]" << std::endl;
    bool is_file = current_input.isFile();
    std::cout << "is_file=[" << is_file << "]" << std::endl;
    bool is_buffer = current_input.isBuffer();
    std::cout << "is_buffer=[" << is_buffer << "]" << std::endl;
    is_preprocessed = current_input.isPreprocessed();
    std::cout << "is_preprocessed=[" << is_preprocessed << "]" << std::endl;
    std::string file = current_input.getFile().str();
    if (!file.empty()) {
      std::cout << "file=[" << file << "]" << std::endl;
    }
  }
  
  {
      //clang::ASTUnit& ast_unit = this->getCurrentASTUnit();
  }
  
  {
      //    clang::Module* module = this->getCurrentModule();
      //    std::string name = module->Name;
      //    std::cout << "name=[" << name << "]" << std::endl;
  }
  
  {
    clang::TranslationUnitKind tu_kind = this->getTranslationUnitKind();
    std::cout << "tu_kind=[" << tu_kind << "]" << std::endl;
    bool pch_support = this->hasPCHSupport();
    std::cout << "pch_support=[" << pch_support << "]" << std::endl;
    bool ast_file_support = this->hasASTFileSupport();
    std::cout << "ast_file_support=[" << ast_file_support << "]" << std::endl;
    bool ir_support = this->hasIRSupport();
    std::cout << "ir_support=[" << ir_support << "]" << std::endl;
    bool cc_support = this->hasCodeCompletionSupport();
    std::cout << "cc_support=[" << cc_support << "]" << std::endl;
  }
}

bool TorchAction::PrepareToExecuteAction(clang::CompilerInstance &CI) { //1
  return super_class::PrepareToExecuteAction(CI);
}

std::unique_ptr<clang::ASTConsumer> TorchAction::CreateASTConsumer(clang::CompilerInstance &CI,
                                                                   llvm::StringRef InFile) { //4
  printf("looper_test, CreateASTConsumer\n");
  std::shared_ptr<TorchModel> torch_model = std::make_shared<TorchModel>(InFile.str());
  torch_models.push_back(torch_model);
  return std::make_unique<TorchConsumer>(*torch_model);
}

bool TorchAction::BeginInvocation(clang::CompilerInstance &CI) { //2
  //可能有用，当宏展开信息不足时
  //CI.getPreprocessorOpts().DetailedRecord = true;
  return super_class::BeginInvocation(CI);
}

bool TorchAction::BeginSourceFileAction(clang::CompilerInstance &CI) { //3
  return super_class::BeginSourceFileAction(CI);
}

void TorchAction::ExecuteAction() { //5
  super_class::ExecuteAction();
}

void TorchAction::EndSourceFileAction() { //6
  super_class::EndSourceFileAction();
}

bool TorchAction::shouldEraseOutputFiles() { //7
  return super_class::shouldEraseOutputFiles();
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

/*
 #import <Masonry/Masonry.h>
 [类型 对象地址 <所在文件:起始行号:起始列号> 列号:? 隐式/显示 导入的 module 的 name]
 ImportDecl 0x115466a28 </Users/looperwang/Desktop/WeMeet/app/app/iOS/Src/App/WeMeet/Meet/WeMeetPrefixHeader.pch:7:1> col:1 implicit Masonry
 */

/*
 static NSString *const SCHEME_BROWSER = @"wemeet://browser";
 [类型 对象地址 <所在文件:起始行号:起始列号，结束列号> 变量名起始列号 变量名 '类型' 存储类型 初始化类型 是否需要析构]
 VarDecl 0x116840060 </Users/looperwang/Desktop/WeMeet/app/app/iOS/Src/App/Common/ViewModel/SchemeDefine.h:1:1, col:42> col:24 SCHEME_BROWSER 'NSString *const __strong' static cinit destroyed
 `-ObjCStringLiteral 0x1168401a0 <col:41, col:42> 'NSString *'
 `-StringLiteral 0x116840178 <col:42> 'char [17]' lvalue "wemeet://browser"
 */
