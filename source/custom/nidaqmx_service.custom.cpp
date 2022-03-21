#include <nidaqmx.grpc.pb.h>
#include <nidaqmx/nidaqmx_service.h>
#include <nidaqmx/nidaqmx_library_interface.h>
#include <server/data_moniker_service.h>

namespace nidaqmx_grpc {

//---------------------------------------------------------------------
//---------------------------------------------------------------------
struct MonikerWriteDAQmxData {
 public:
  TaskHandle task;
  int32 numsamples;  // ?
  int32 num_samps_per_chan;
  bool auto_start;
  double timeout;
  int32 data_layout;
  NiDAQmxLibraryInterface* library;
};

//---------------------------------------------------------------------
//---------------------------------------------------------------------
::grpc::Status MonikerWriteAnalogF64Stream(void* data, google::protobuf::Any& packedData)
{
  MonikerWriteDAQmxData* writeData = (MonikerWriteDAQmxData*)data;

  int32 samps_per_chan_written;  

  nidaqmx_grpc::ChannelData channelData;
  packedData.UnpackTo(&channelData);

  const double* values = channelData.data().data();

  // ?
  auto status = writeData->library->WriteAnalogF64(writeData->task, writeData->num_samps_per_chan, writeData->auto_start, writeData->timeout, writeData->data_layout, values, &samps_per_chan_written, nullptr);
  if (status < 0) {
    std ::cout << "DAQmxWriteAnalogF64 error: " << status << std::endl;
    std ::cout << "TaskName " << writeData->task << std::endl;
    std ::cout << "Values " << values[0] << " " << channelData.data_size() << std::endl;
  }
  return ::grpc::Status::OK;
}

//---------------------------------------------------------------------
//---------------------------------------------------------------------
struct MonikerReadDAQmxData {
 public:
  TaskHandle task;
  int32 numSamples;
  int32 num_samps_per_chan;
  double timeout;
  uInt32 array_size_in_samps;
  nidaqmx_grpc::ChannelData channelData;
  int32 fill_mode;
  NiDAQmxLibraryInterface* library;
};

//---------------------------------------------------------------------
//---------------------------------------------------------------------
::grpc::Status MonikerReadAnalogF64Stream(void* data, google::protobuf::Any& packedData)
{
  MonikerReadDAQmxData* readData = (MonikerReadDAQmxData*)data;
  // calling into driver and setting response 
  int32 numRead;
  auto status = readData->library->ReadAnalogF64(readData->task, readData->num_samps_per_chan, readData->timeout, readData->fill_mode, readData->channelData.mutable_data()->mutable_data(), readData->array_size_in_samps, &numRead, NULL);
  if (status < 0) {
    std::cout << "DAQmxReadAnalogF64 error: " << status << std::endl;
  //  std ::cout << "TaskName " << readData->task << std::endl;
  //  std ::cout << "Values " << readData->channelData.data()[0] << " " << readData->channelData.data_size() << std::endl;
  }
  packedData.PackFrom(readData->channelData);
  return ::grpc::Status::OK;
}

//---------------------------------------------------------------------
//---------------------------------------------------------------------
struct MonikerWaitForNextSampleClockData {
 public:
  TaskHandle task;
  double timeout;
  bool32 is_late; //?
  nidaqmx_grpc::ChannelData channelData;
  NiDAQmxLibraryInterface* library;
};

//---------------------------------------------------------------------
//---------------------------------------------------------------------
::grpc::Status MonikerWaitForNextSampleClockStream(void* data, google::protobuf::Any& packedData)
{
  MonikerWaitForNextSampleClockData* waitData = (MonikerWaitForNextSampleClockData*)data;
  // calling into driver and setting response
  auto status = waitData->library->WaitForNextSampleClock(waitData->task, waitData->timeout, NULL);  
  // waitData->is_late, &numRead, NULL);
  if (status < 0) {
    std::cout << "WaitForNextSampleClock error: " << status << std::endl;
  }
 // packedData.PackFrom(waitMoniker->channelData); is_late as part of channelData 
 // similar to write
  //packedData.UnpackTo(&channelData);
  return ::grpc::Status::OK;
}

//---------------------------------------------------------------------
//---------------------------------------------------------------------
::grpc::Status NiDAQmxService::WriteAnalogF64Stream(::grpc::ServerContext* context, const WriteAnalogF64StreamRequest* request, WriteAnalogF64StreamResponse* response)
{
  if (context->IsCancelled()) {
    return ::grpc::Status::CANCELLED;
  }
  try {
    MonikerWriteDAQmxData* writeData = new MonikerWriteDAQmxData(); 
    auto task_grpc_session = request->task();
    writeData->task = session_repository_->access_session(task_grpc_session.id(), task_grpc_session.name());
    // Retrieved from WriteAnalogF64()
    writeData->num_samps_per_chan = request->num_samps_per_chan();
    writeData->auto_start = request->auto_start();
    writeData->timeout = request->timeout();
   // int32 data_layout;
    switch (request->data_layout_enum_case()) {
      case nidaqmx_grpc::WriteAnalogF64StreamRequest::DataLayoutEnumCase::kDataLayout: {
        writeData->data_layout = static_cast<int32>(request->data_layout());
        break;
      }
      case nidaqmx_grpc::WriteAnalogF64StreamRequest::DataLayoutEnumCase::kDataLayoutRaw: {
        writeData->data_layout = static_cast<int32>(request->data_layout_raw());
        break;
      }
      case nidaqmx_grpc::WriteAnalogF64StreamRequest::DataLayoutEnumCase::DATA_LAYOUT_ENUM_NOT_SET: {
        return ::grpc::Status(::grpc::INVALID_ARGUMENT, "The value for data_layout was not specified or out of range");
        break;
      }
    }

    // Register stream 

    writeData->library = library_;
    ni::data_monikers::Moniker* writeMoniker = new ni::data_monikers::Moniker();
    ::ni::data_monikers::DataMonikerService::RegisterMonikerInstance("MonikerWriteAnalogF64Stream", writeData, *writeMoniker);
    response->set_allocated_moniker(writeMoniker);
    return ::grpc::Status::OK;
  }
  catch (nidevice_grpc::LibraryLoadException& ex) {
    return ::grpc::Status(::grpc::NOT_FOUND, ex.what());
  }
}


 //---------------------------------------------------------------------
//-----------------------------------------------------------------------
// Custom code for ReadAnalogF64Stream
::grpc::Status NiDAQmxService::ReadAnalogF64Stream(::grpc::ServerContext* context, const ReadAnalogF64StreamRequest* request, ReadAnalogF64StreamResponse* response)
{
  if (context->IsCancelled()) {
    return ::grpc::Status::CANCELLED;
  }
  try {
    MonikerReadDAQmxData* readData = new MonikerReadDAQmxData();
    auto task_grpc_session = request->task();
    readData->task = session_repository_->access_session(task_grpc_session.id(), task_grpc_session.name());
    readData->array_size_in_samps = request->array_size_in_samps();
    readData->num_samps_per_chan = request->num_samps_per_chan();
    readData->timeout = request->timeout();
    readData->channelData.mutable_data()->Reserve(readData->array_size_in_samps);
    readData->channelData.mutable_data()->Resize(readData->array_size_in_samps, 0.0);
    readData->fill_mode;
    switch (request->fill_mode_enum_case()) {
      case nidaqmx_grpc::ReadAnalogF64StreamRequest::FillModeEnumCase::kFillMode: {
        readData->fill_mode = static_cast<int32>(request->fill_mode());
        break;
      }
      case nidaqmx_grpc::ReadAnalogF64StreamRequest::FillModeEnumCase::kFillModeRaw: {
        readData->fill_mode = static_cast<int32>(request->fill_mode_raw());
        break;
      }
      case nidaqmx_grpc::ReadAnalogF64StreamRequest::FillModeEnumCase::FILL_MODE_ENUM_NOT_SET: {
        return ::grpc::Status(::grpc::INVALID_ARGUMENT, "The value for fill_mode was not specified or out of range");
        break;
      }
    }
    readData->library = library_;
    ni::data_monikers::Moniker* readMoniker = new ni::data_monikers::Moniker();
    ::ni::data_monikers::DataMonikerService::RegisterMonikerInstance("MonikerReadAnalogF64Stream", readData, *readMoniker);
    response->set_allocated_moniker(readMoniker);
    return ::grpc::Status::OK;
  }
  catch (nidevice_grpc::LibraryLoadException& ex) {
    return ::grpc::Status(::grpc::NOT_FOUND, ex.what());
  }
}

//---------------------------------------------------------------------
//---------------------------------------------------------------------
::grpc::Status NiDAQmxService::WaitForNextSampleClockStream(::grpc::ServerContext* context, const WaitForNextSampleClockStreamRequest* request, WaitForNextSampleClockStreamResponse* response)
{
  if (context->IsCancelled()) {
    return ::grpc::Status::CANCELLED;
  }
  try {
    MonikerWaitForNextSampleClockData* waitData = new MonikerWaitForNextSampleClockData();
    auto task_grpc_session = request->task();
    waitData->task = session_repository_->access_session(task_grpc_session.id(), task_grpc_session.name());
    waitData->timeout = request->timeout();
   // waitData->is_late = request->set_is_late(nidaqmx_grpc::MonikerReadDAQmxData::is_late);
    waitData->library = library_;
    ni::data_monikers::Moniker* waitMoniker = new ni::data_monikers::Moniker();
    ::ni::data_monikers::DataMonikerService::RegisterMonikerInstance("MonikerWaitForNextSampleClockStream", waitData, *waitMoniker);
    response->set_allocated_moniker(waitMoniker);
    return ::grpc::Status::OK;
  }
  catch (nidevice_grpc::LibraryLoadException& ex) {
    return ::grpc::Status(::grpc::NOT_FOUND, ex.what());
  }
}

void RegisterMonikers()
{
    ::ni::data_monikers::DataMonikerService::RegisterMonikerEndpoint("MonikerReadAnalogF64Stream", MonikerReadAnalogF64Stream);
    ::ni::data_monikers::DataMonikerService::RegisterMonikerEndpoint("MonikerWaitForNextSampleClockStream", MonikerWaitForNextSampleClockStream);
    ::ni::data_monikers::DataMonikerService::RegisterMonikerEndpoint("MonikerWriteAnalogF64Stream", MonikerWriteAnalogF64Stream);
}

/*
void NiDAQmxService::initialize()
{
  // Check order of registering for wait->read->write
  ::ni::data_monikers::DataMonikerService::RegisterMonikerEndpoint("MonikerReadAnalogF64Stream", MonikerReadAnalogF64Stream);
  ::ni::data_monikers::DataMonikerService::RegisterMonikerEndpoint("MonikerWaitForNextSampleClk", MonikerWaitForNextSampleClk);
  ::ni::data_monikers::DataMonikerService::RegisterMonikerEndpoint("MonikerWriteAnalogF64Stream", MonikerWriteAnalogF64Stream);
  
}
*/
}  // namespace nidaqmx_grpc
