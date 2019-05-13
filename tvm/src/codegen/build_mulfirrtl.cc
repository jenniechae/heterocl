/*!
 *  Copyright (c) 2018 by Contributors
 * \file build_mulfirrtl.cc
 * \brief Build FIRRTL modules for iterative multiplier from source.
 */
#include "./codegen_mulfirrtl.h"
#include "./build_common.h"

namespace tvm {
namespace codegen {

std::string BuildMULFIRRTL(Array<LoweredFunc> funcs) {
  using tvm::runtime::Registry;

  CodeAnalysMerlinC ca;
  CodeGenMULFIRRTL cg;
  for (LoweredFunc f : funcs) {
    // 1st pass: Analyze AST and collect necessary information
    ca.AddFunction(f);
    str2tupleMap<std::string, Type> map_arg_type;
    map_arg_type = ca.Finish();
    // 2nd pass: Generate kernel code
    cg.AddFunction(f, map_arg_type);
  }
  std::string code = cg.Finish();

  if (const auto* f = Registry::Get("tvm_callback_mulfirrtl_postproc")) {
    code = (*f)(code).operator std::string();
  }
  LOG(WARNING) << "FIRRTL doesn't have runtime, return kernel code";
  return code;
}

TVM_REGISTER_API("codegen.build_mulfirrtl")
.set_body([](TVMArgs args, TVMRetValue* rv) {
    *rv = BuildMULFIRRTL(args[0]);
  });
}  // namespace codegen
}  // namespace tvm
