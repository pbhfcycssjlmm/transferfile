// Code generated by protoc-gen-go-grpc. DO NOT EDIT.
// versions:
// - protoc-gen-go-grpc v1.2.0
// - protoc             v3.19.4
// source: proto/transfer_file.proto

package transfer

import (
	context "context"
	grpc "google.golang.org/grpc"
	codes "google.golang.org/grpc/codes"
	status "google.golang.org/grpc/status"
)

// This is a compile-time assertion to ensure that this generated file
// is compatible with the grpc package it is being compiled against.
// Requires gRPC-Go v1.32.0 or later.
const _ = grpc.SupportPackageIsVersion7

// TransferFileClient is the client API for TransferFile service.
//
// For semantics around ctx use and closing/ending streaming RPCs, please refer to https://pkg.go.dev/google.golang.org/grpc/?tab=doc#ClientConn.NewStream.
type TransferFileClient interface {
	Upload(ctx context.Context, opts ...grpc.CallOption) (TransferFile_UploadClient, error)
}

type transferFileClient struct {
	cc grpc.ClientConnInterface
}

func NewTransferFileClient(cc grpc.ClientConnInterface) TransferFileClient {
	return &transferFileClient{cc}
}

func (c *transferFileClient) Upload(ctx context.Context, opts ...grpc.CallOption) (TransferFile_UploadClient, error) {
	stream, err := c.cc.NewStream(ctx, &TransferFile_ServiceDesc.Streams[0], "/transferfile.TransferFile/Upload", opts...)
	if err != nil {
		return nil, err
	}
	x := &transferFileUploadClient{stream}
	return x, nil
}

type TransferFile_UploadClient interface {
	Send(*Chunk) error
	CloseAndRecv() (*Reply, error)
	grpc.ClientStream
}

type transferFileUploadClient struct {
	grpc.ClientStream
}

func (x *transferFileUploadClient) Send(m *Chunk) error {
	return x.ClientStream.SendMsg(m)
}

func (x *transferFileUploadClient) CloseAndRecv() (*Reply, error) {
	if err := x.ClientStream.CloseSend(); err != nil {
		return nil, err
	}
	m := new(Reply)
	if err := x.ClientStream.RecvMsg(m); err != nil {
		return nil, err
	}
	return m, nil
}

// TransferFileServer is the server API for TransferFile service.
// All implementations must embed UnimplementedTransferFileServer
// for forward compatibility
type TransferFileServer interface {
	Upload(TransferFile_UploadServer) error
	mustEmbedUnimplementedTransferFileServer()
}

// UnimplementedTransferFileServer must be embedded to have forward compatible implementations.
type UnimplementedTransferFileServer struct {
}

func (UnimplementedTransferFileServer) Upload(TransferFile_UploadServer) error {
	return status.Errorf(codes.Unimplemented, "method Upload not implemented")
}
func (UnimplementedTransferFileServer) mustEmbedUnimplementedTransferFileServer() {}

// UnsafeTransferFileServer may be embedded to opt out of forward compatibility for this service.
// Use of this interface is not recommended, as added methods to TransferFileServer will
// result in compilation errors.
type UnsafeTransferFileServer interface {
	mustEmbedUnimplementedTransferFileServer()
}

func RegisterTransferFileServer(s grpc.ServiceRegistrar, srv TransferFileServer) {
	s.RegisterService(&TransferFile_ServiceDesc, srv)
}

func _TransferFile_Upload_Handler(srv interface{}, stream grpc.ServerStream) error {
	return srv.(TransferFileServer).Upload(&transferFileUploadServer{stream})
}

type TransferFile_UploadServer interface {
	SendAndClose(*Reply) error
	Recv() (*Chunk, error)
	grpc.ServerStream
}

type transferFileUploadServer struct {
	grpc.ServerStream
}

func (x *transferFileUploadServer) SendAndClose(m *Reply) error {
	return x.ServerStream.SendMsg(m)
}

func (x *transferFileUploadServer) Recv() (*Chunk, error) {
	m := new(Chunk)
	if err := x.ServerStream.RecvMsg(m); err != nil {
		return nil, err
	}
	return m, nil
}

// TransferFile_ServiceDesc is the grpc.ServiceDesc for TransferFile service.
// It's only intended for direct use with grpc.RegisterService,
// and not to be introspected or modified (even as a copy)
var TransferFile_ServiceDesc = grpc.ServiceDesc{
	ServiceName: "transferfile.TransferFile",
	HandlerType: (*TransferFileServer)(nil),
	Methods:     []grpc.MethodDesc{},
	Streams: []grpc.StreamDesc{
		{
			StreamName:    "Upload",
			Handler:       _TransferFile_Upload_Handler,
			ClientStreams: true,
		},
	},
	Metadata: "proto/transfer_file.proto",
}