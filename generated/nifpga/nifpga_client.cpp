
//---------------------------------------------------------------------
// This file is automatically generated. All manual edits will be lost.
//---------------------------------------------------------------------
// EXPERIMENTAL Client convenience wrapper for NI-FPGA.
//---------------------------------------------------------------------
#include "nifpga_client.h"

#include <grpcpp/grpcpp.h>

#include <nifpga.grpc.pb.h>

#include <cstdint>
#include <memory>
#include <stdexcept>
#include <vector>

namespace nifpga_grpc::experimental::client {

OpenResponse
open(const StubPtr& stub, const pb::string& bitfile, const pb::string& signature, const pb::string& resource, const simple_variant<OpenAttribute, std::int32_t>& attribute)
{
  ::grpc::ClientContext context;

  auto request = OpenRequest{};
  request.set_bitfile(bitfile);
  request.set_signature(signature);
  request.set_resource(resource);
  const auto attribute_ptr = attribute.get_if<OpenAttribute>();
  const auto attribute_raw_ptr = attribute.get_if<std::int32_t>();
  if (attribute_ptr) {
    request.set_attribute_mapped(*attribute_ptr);
  }
  else if (attribute_raw_ptr) {
    request.set_attribute_raw(*attribute_raw_ptr);
  }

  auto response = OpenResponse{};

  raise_if_error(
      stub->Open(&context, request, &response));

  return response;
}

CloseResponse
close(const StubPtr& stub, const nidevice_grpc::Session& session, const simple_variant<CloseAttribute, pb::uint32>& attribute)
{
  ::grpc::ClientContext context;

  auto request = CloseRequest{};
  request.mutable_session()->CopyFrom(session);
  const auto attribute_ptr = attribute.get_if<CloseAttribute>();
  const auto attribute_raw_ptr = attribute.get_if<pb::uint32>();
  if (attribute_ptr) {
    request.set_attribute(*attribute_ptr);
  }
  else if (attribute_raw_ptr) {
    request.set_attribute_raw(*attribute_raw_ptr);
  }

  auto response = CloseResponse{};

  raise_if_error(
      stub->Close(&context, request, &response));

  return response;
}

RunResponse
run(const StubPtr& stub, const nidevice_grpc::Session& session, const simple_variant<RunAttribute, pb::uint32>& attribute)
{
  ::grpc::ClientContext context;

  auto request = RunRequest{};
  request.mutable_session()->CopyFrom(session);
  const auto attribute_ptr = attribute.get_if<RunAttribute>();
  const auto attribute_raw_ptr = attribute.get_if<pb::uint32>();
  if (attribute_ptr) {
    request.set_attribute(*attribute_ptr);
  }
  else if (attribute_raw_ptr) {
    request.set_attribute_raw(*attribute_raw_ptr);
  }

  auto response = RunResponse{};

  raise_if_error(
      stub->Run(&context, request, &response));

  return response;
}

ResetResponse
reset(const StubPtr& stub, const nidevice_grpc::Session& session)
{
  ::grpc::ClientContext context;

  auto request = ResetRequest{};
  request.mutable_session()->CopyFrom(session);

  auto response = ResetResponse{};

  raise_if_error(
      stub->Reset(&context, request, &response));

  return response;
}

DownloadResponse
download(const StubPtr& stub, const nidevice_grpc::Session& session)
{
  ::grpc::ClientContext context;

  auto request = DownloadRequest{};
  request.mutable_session()->CopyFrom(session);

  auto response = DownloadResponse{};

  raise_if_error(
      stub->Download(&context, request, &response));

  return response;
}

ReadBoolResponse
read_bool(const StubPtr& stub, const nidevice_grpc::Session& session, const pb::uint32& indicator)
{
  ::grpc::ClientContext context;

  auto request = ReadBoolRequest{};
  request.mutable_session()->CopyFrom(session);
  request.set_indicator(indicator);

  auto response = ReadBoolResponse{};

  raise_if_error(
      stub->ReadBool(&context, request, &response));

  return response;
}

ReadI8Response
read_i8(const StubPtr& stub, const nidevice_grpc::Session& session, const pb::uint32& indicator)
{
  ::grpc::ClientContext context;

  auto request = ReadI8Request{};
  request.mutable_session()->CopyFrom(session);
  request.set_indicator(indicator);

  auto response = ReadI8Response{};

  raise_if_error(
      stub->ReadI8(&context, request, &response));

  return response;
}

ReadU8Response
read_u8(const StubPtr& stub, const nidevice_grpc::Session& session, const pb::uint32& indicator)
{
  ::grpc::ClientContext context;

  auto request = ReadU8Request{};
  request.mutable_session()->CopyFrom(session);
  request.set_indicator(indicator);

  auto response = ReadU8Response{};

  raise_if_error(
      stub->ReadU8(&context, request, &response));

  return response;
}

ReadI16Response
read_i16(const StubPtr& stub, const nidevice_grpc::Session& session, const pb::uint32& indicator)
{
  ::grpc::ClientContext context;

  auto request = ReadI16Request{};
  request.mutable_session()->CopyFrom(session);
  request.set_indicator(indicator);

  auto response = ReadI16Response{};

  raise_if_error(
      stub->ReadI16(&context, request, &response));

  return response;
}

ReadU16Response
read_u16(const StubPtr& stub, const nidevice_grpc::Session& session, const pb::uint32& indicator)
{
  ::grpc::ClientContext context;

  auto request = ReadU16Request{};
  request.mutable_session()->CopyFrom(session);
  request.set_indicator(indicator);

  auto response = ReadU16Response{};

  raise_if_error(
      stub->ReadU16(&context, request, &response));

  return response;
}

ReadI32Response
read_i32(const StubPtr& stub, const nidevice_grpc::Session& session, const pb::uint32& indicator)
{
  ::grpc::ClientContext context;

  auto request = ReadI32Request{};
  request.mutable_session()->CopyFrom(session);
  request.set_indicator(indicator);

  auto response = ReadI32Response{};

  raise_if_error(
      stub->ReadI32(&context, request, &response));

  return response;
}

ReadI32StreamResponse
read_i32_stream(const StubPtr& stub, const nidevice_grpc::Session& session, const pb::uint32& indicator)
{
  ::grpc::ClientContext context;

  auto request = ReadI32StreamRequest{};
  request.mutable_session()->CopyFrom(session);
  request.set_indicator(indicator);

  auto response = ReadI32StreamResponse{};

  raise_if_error(
      stub->ReadI32Stream(&context, request, &response));

  return response;
}

ReadU32Response
read_u32(const StubPtr& stub, const nidevice_grpc::Session& session, const pb::uint32& indicator)
{
  ::grpc::ClientContext context;

  auto request = ReadU32Request{};
  request.mutable_session()->CopyFrom(session);
  request.set_indicator(indicator);

  auto response = ReadU32Response{};

  raise_if_error(
      stub->ReadU32(&context, request, &response));

  return response;
}

ReadI64Response
read_i64(const StubPtr& stub, const nidevice_grpc::Session& session, const pb::uint32& indicator)
{
  ::grpc::ClientContext context;

  auto request = ReadI64Request{};
  request.mutable_session()->CopyFrom(session);
  request.set_indicator(indicator);

  auto response = ReadI64Response{};

  raise_if_error(
      stub->ReadI64(&context, request, &response));

  return response;
}

ReadU64Response
read_u64(const StubPtr& stub, const nidevice_grpc::Session& session, const pb::uint32& indicator)
{
  ::grpc::ClientContext context;

  auto request = ReadU64Request{};
  request.mutable_session()->CopyFrom(session);
  request.set_indicator(indicator);

  auto response = ReadU64Response{};

  raise_if_error(
      stub->ReadU64(&context, request, &response));

  return response;
}

WriteBoolResponse
write_bool(const StubPtr& stub, const nidevice_grpc::Session& session, const pb::uint32& control, const bool& value)
{
  ::grpc::ClientContext context;

  auto request = WriteBoolRequest{};
  request.mutable_session()->CopyFrom(session);
  request.set_control(control);
  request.set_value(value);

  auto response = WriteBoolResponse{};

  raise_if_error(
      stub->WriteBool(&context, request, &response));

  return response;
}

WriteI8Response
write_i8(const StubPtr& stub, const nidevice_grpc::Session& session, const pb::uint32& control, const pb::int32& value)
{
  ::grpc::ClientContext context;

  auto request = WriteI8Request{};
  request.mutable_session()->CopyFrom(session);
  request.set_control(control);
  request.set_value(value);

  auto response = WriteI8Response{};

  raise_if_error(
      stub->WriteI8(&context, request, &response));

  return response;
}

WriteU8Response
write_u8(const StubPtr& stub, const nidevice_grpc::Session& session, const pb::uint32& control, const pb::uint32& value)
{
  ::grpc::ClientContext context;

  auto request = WriteU8Request{};
  request.mutable_session()->CopyFrom(session);
  request.set_control(control);
  request.set_value(value);

  auto response = WriteU8Response{};

  raise_if_error(
      stub->WriteU8(&context, request, &response));

  return response;
}

WriteI16Response
write_i16(const StubPtr& stub, const nidevice_grpc::Session& session, const pb::uint32& control, const pb::int32& value)
{
  ::grpc::ClientContext context;

  auto request = WriteI16Request{};
  request.mutable_session()->CopyFrom(session);
  request.set_control(control);
  request.set_value(value);

  auto response = WriteI16Response{};

  raise_if_error(
      stub->WriteI16(&context, request, &response));

  return response;
}

WriteU16Response
write_u16(const StubPtr& stub, const nidevice_grpc::Session& session, const pb::uint32& control, const pb::uint32& value)
{
  ::grpc::ClientContext context;

  auto request = WriteU16Request{};
  request.mutable_session()->CopyFrom(session);
  request.set_control(control);
  request.set_value(value);

  auto response = WriteU16Response{};

  raise_if_error(
      stub->WriteU16(&context, request, &response));

  return response;
}

WriteI32Response
write_i32(const StubPtr& stub, const nidevice_grpc::Session& session, const pb::uint32& control, const pb::int32& value)
{
  ::grpc::ClientContext context;

  auto request = WriteI32Request{};
  request.mutable_session()->CopyFrom(session);
  request.set_control(control);
  request.set_value(value);

  auto response = WriteI32Response{};

  raise_if_error(
      stub->WriteI32(&context, request, &response));

  return response;
}

WriteU32Response
write_u32(const StubPtr& stub, const nidevice_grpc::Session& session, const pb::uint32& control, const pb::uint32& value)
{
  ::grpc::ClientContext context;

  auto request = WriteU32Request{};
  request.mutable_session()->CopyFrom(session);
  request.set_control(control);
  request.set_value(value);

  auto response = WriteU32Response{};

  raise_if_error(
      stub->WriteU32(&context, request, &response));

  return response;
}

WriteI64Response
write_i64(const StubPtr& stub, const nidevice_grpc::Session& session, const pb::uint32& control, const pb::int64& value)
{
  ::grpc::ClientContext context;

  auto request = WriteI64Request{};
  request.mutable_session()->CopyFrom(session);
  request.set_control(control);
  request.set_value(value);

  auto response = WriteI64Response{};

  raise_if_error(
      stub->WriteI64(&context, request, &response));

  return response;
}

WriteU64Response
write_u64(const StubPtr& stub, const nidevice_grpc::Session& session, const pb::uint32& control, const pb::uint64& value)
{
  ::grpc::ClientContext context;

  auto request = WriteU64Request{};
  request.mutable_session()->CopyFrom(session);
  request.set_control(control);
  request.set_value(value);

  auto response = WriteU64Response{};

  raise_if_error(
      stub->WriteU64(&context, request, &response));

  return response;
}

ReadArrayI16Response
read_array_i16(const StubPtr& stub, const nidevice_grpc::Session& session, const pb::uint32& indicator, const pb::uint32& size)
{
  ::grpc::ClientContext context;

  auto request = ReadArrayI16Request{};
  request.mutable_session()->CopyFrom(session);
  request.set_indicator(indicator);
  request.set_size(size);

  auto response = ReadArrayI16Response{};

  raise_if_error(
      stub->ReadArrayI16(&context, request, &response));

  return response;
}

ReadArrayU16Response
read_array_u16(const StubPtr& stub, const nidevice_grpc::Session& session, const pb::uint32& indicator, const pb::uint32& size)
{
  ::grpc::ClientContext context;

  auto request = ReadArrayU16Request{};
  request.mutable_session()->CopyFrom(session);
  request.set_indicator(indicator);
  request.set_size(size);

  auto response = ReadArrayU16Response{};

  raise_if_error(
      stub->ReadArrayU16(&context, request, &response));

  return response;
}

ReadArrayI64Response
read_array_i64(const StubPtr& stub, const nidevice_grpc::Session& session, const pb::uint32& indicator, const pb::uint32& size)
{
  ::grpc::ClientContext context;

  auto request = ReadArrayI64Request{};
  request.mutable_session()->CopyFrom(session);
  request.set_indicator(indicator);
  request.set_size(size);

  auto response = ReadArrayI64Response{};

  raise_if_error(
      stub->ReadArrayI64(&context, request, &response));

  return response;
}

ReadArrayU64Response
read_array_u64(const StubPtr& stub, const nidevice_grpc::Session& session, const pb::uint32& indicator, const pb::uint32& size)
{
  ::grpc::ClientContext context;

  auto request = ReadArrayU64Request{};
  request.mutable_session()->CopyFrom(session);
  request.set_indicator(indicator);
  request.set_size(size);

  auto response = ReadArrayU64Response{};

  raise_if_error(
      stub->ReadArrayU64(&context, request, &response));

  return response;
}

WriteArrayI16Response
write_array_i16(const StubPtr& stub, const nidevice_grpc::Session& session, const pb::uint32& indicator, const std::vector<pb::int32>& array, const pb::uint32& size)
{
  ::grpc::ClientContext context;

  auto request = WriteArrayI16Request{};
  request.mutable_session()->CopyFrom(session);
  request.set_indicator(indicator);
  copy_array(array, request.mutable_array());
  request.set_size(size);

  auto response = WriteArrayI16Response{};

  raise_if_error(
      stub->WriteArrayI16(&context, request, &response));

  return response;
}

WriteArrayU16Response
write_array_u16(const StubPtr& stub, const nidevice_grpc::Session& session, const pb::uint32& indicator, const std::vector<pb::uint32>& array, const pb::uint32& size)
{
  ::grpc::ClientContext context;

  auto request = WriteArrayU16Request{};
  request.mutable_session()->CopyFrom(session);
  request.set_indicator(indicator);
  copy_array(array, request.mutable_array());
  request.set_size(size);

  auto response = WriteArrayU16Response{};

  raise_if_error(
      stub->WriteArrayU16(&context, request, &response));

  return response;
}

WriteArrayI64Response
write_array_i64(const StubPtr& stub, const nidevice_grpc::Session& session, const pb::uint32& indicator, const std::vector<pb::int64>& array, const pb::uint32& size)
{
  ::grpc::ClientContext context;

  auto request = WriteArrayI64Request{};
  request.mutable_session()->CopyFrom(session);
  request.set_indicator(indicator);
  copy_array(array, request.mutable_array());
  request.set_size(size);

  auto response = WriteArrayI64Response{};

  raise_if_error(
      stub->WriteArrayI64(&context, request, &response));

  return response;
}

WriteArrayU64Response
write_array_u64(const StubPtr& stub, const nidevice_grpc::Session& session, const pb::uint32& indicator, const std::vector<pb::uint64>& array, const pb::uint32& size)
{
  ::grpc::ClientContext context;

  auto request = WriteArrayU64Request{};
  request.mutable_session()->CopyFrom(session);
  request.set_indicator(indicator);
  copy_array(array, request.mutable_array());
  request.set_size(size);

  auto response = WriteArrayU64Response{};

  raise_if_error(
      stub->WriteArrayU64(&context, request, &response));

  return response;
}


} // namespace nifpga_grpc::experimental::client
