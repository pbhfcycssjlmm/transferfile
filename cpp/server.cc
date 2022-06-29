#include <iostream>
#include <fstream>
#include <string>
#include <grpc/grpc.h>
#include <grpc++/server.h>
#include <grpc++/server_builder.h>
#include <grpc++/server_context.h>
#include <grpc++/security/server_credentials.h>
#include "transfer_file.grpc.pb.h"
#include "transfer_file.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerReader;
using grpc::ServerWriter;
using grpc::Status;
using transferfile::Chunk;
using transferfile::Reply;
using transferfile::DownloadRequest;
using transferfile::TransferFile;
#define CHUNK_SIZE 4096

class TransferFileImpl final : public TransferFile::Service {
public:
    Status Upload(ServerContext* context, ServerReader<Chunk>* reader, Reply* reply);
    Status Download(ServerContext* context,const DownloadRequest* request, ServerWriter<Chunk>* writer);
};

Status TransferFileImpl::Download(ServerContext* context,const DownloadRequest* request, ServerWriter<Chunk>* writer) {
    std::string objectName = request->objectname();
    std::ifstream infile(objectName, std::ios::binary|std::ios::in);
    std::cout << "Server ready to send file:" << objectName << std::endl;
    if (infile.is_open())
    {
        infile.seekg(0,std::ios::end);
        int mlen = infile.tellg();
        std::cout << "open the file successfully" << std::endl;
        std::cout << "file size:" << mlen << std::endl;
        infile.seekg(0,std::ios::beg);
    }
    else{
        std::cout << "fail to open the file" <<std::endl;
        return Status::CANCELLED;//或者抛出异常。
    }
    char data[CHUNK_SIZE];
    Chunk chunk;
    chunk.set_objectname("");
    int sendLen = 0;
    while (!infile.eof()) {
        infile.read(data, CHUNK_SIZE);
        chunk.set_buffer(data, infile.gcount());
        if (!writer->Write(chunk)) {
            std::cout << "Write fail" <<std::endl;
            std::cout << "Broken stream." <<std::endl;            // Broken stream.
            return Status::CANCELLED;
        }
        sendLen += infile.gcount();
    }
    std::cout << "Send file size:" << sendLen << std::endl;
    return Status::OK;
}
Status TransferFileImpl::Upload(ServerContext* context, ServerReader<Chunk>* reader, Reply* reply) {
    Chunk chunk;
    std::ofstream outfile;
    const char *data;
    int cnt=0;
    while (reader->Read(&chunk)) {
        if(cnt==0){
            ++cnt;
            outfile.open(chunk.objectname(), std::ofstream::out | std::ofstream::trunc | std::ofstream::binary);
        }
        data = chunk.buffer().c_str();
        outfile.write(data, chunk.buffer().length());
    }
    long pos = outfile.tellp();
    reply->set_result(pos);
    outfile.close();
    return Status::OK;
}
void RunServer() {
    std::string server_address("0.0.0.0:50051");
    TransferFileImpl service;
    ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);
    std::unique_ptr<Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << server_address << std::endl;
    server->Wait();
}

int main(int argc, char** argv) {
    RunServer();
    return 0;
}