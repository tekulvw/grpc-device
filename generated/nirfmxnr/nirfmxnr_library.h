//---------------------------------------------------------------------
// This file is automatically generated. All manual edits will be lost.
//---------------------------------------------------------------------
// Real implementation of LibraryInterface for NI-RFMXNR
//---------------------------------------------------------------------
#ifndef NIRFMXNR_GRPC_LIBRARY_H
#define NIRFMXNR_GRPC_LIBRARY_H

#include "nirfmxnr_library_interface.h"

#include <server/shared_library.h>

namespace nirfmxnr_grpc {

class NiRFmxNRLibrary : public nirfmxnr_grpc::NiRFmxNRLibraryInterface {
 public:
  NiRFmxNRLibrary();
  virtual ~NiRFmxNRLibrary();

  ::grpc::Status check_function_exists(std::string functionName);
  int32 Close(niRFmxInstrHandle instrumentHandle, int32 forceDestroy);
  int32 Initialize(char resourceName[], char optionString[], niRFmxInstrHandle* handleOut, int32* isNewSession);

 private:
  using ClosePtr = decltype(&RFmxNR_Close);
  using InitializePtr = decltype(&RFmxNR_Initialize);

  typedef struct FunctionPointers {
    ClosePtr Close;
    InitializePtr Initialize;
  } FunctionLoadStatus;

  nidevice_grpc::SharedLibrary shared_library_;
  FunctionPointers function_pointers_;
};

}  // namespace nirfmxnr_grpc

#endif  // NIRFMXNR_GRPC_LIBRARY_H
