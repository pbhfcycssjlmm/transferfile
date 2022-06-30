#include <iostream>
#include <string>
#include <fstream>
#include <sys/time.h>
#include <grpc/grpc.h>
#include <grpc++/channel.h>
#include <grpc++/client_context.h>
#include <grpc++/create_channel.h>
#include <grpc++/security/credentials.h>
#include "transfer_file.grpc.pb.h"
using grpc::Channel;
using grpc::ClientContext;
using grpc::ClientWriter;
using grpc::ClientReader;
using grpc::Status;
using transferfile::Chunk;
using transferfile::Reply;
using transferfile::TransferFile;
using transferfile::DownloadRequest;
#define CHUNK_SIZE 1048576
class TransferFileClient
{
public:
    TransferFileClient(std::shared_ptr<Channel> channel) : stub_(TransferFile::NewStub(channel)){};
    void Upload(std::string objectName,std::string filePath);
    void Download(std::string objectName,std::string savePath);
private:
    std::unique_ptr<TransferFile::Stub> stub_;
};

void TransferFileClient::Download(std::string objectName,std::string savePath)
{
    Chunk chunk;
    std::ofstream outfile;
    const char *data;
    ClientContext context;
    DownloadRequest downloadRequest;
    downloadRequest.set_objectname(objectName);
//    std::unique_ptr<ClientReader<Chunk>> Download(::grpc::ClientContext* context, const ::transferfile::DownloadRequest& request)
    std::unique_ptr<ClientReader<Chunk>> reader(stub_->Download(&context,downloadRequest));
    outfile.open(savePath, std::ofstream::out | std::ofstream::trunc | std::ofstream::binary);
    int recvLen = 0;
    while (reader->Read(&chunk)) {
        data = chunk.buffer().c_str();
        outfile.write(data, chunk.buffer().length());
        recvLen = recvLen + chunk.buffer().length();
        std::cout << "chunk.buffer().length():" << chunk.buffer().length() <<std::endl;
    }
    long pos = outfile.tellp();
    std::cout << "Client recv:" << recvLen <<std::endl;
    std::cout << "Client download:" << pos <<std::endl;
    outfile.close();
    Status status = reader->Finish();
    if (status.ok()) {
        std::cout << "Download rpc succeeded." << std::endl;
    } else {
        std::cout << "Download rpc failed." << std::endl;
    }
}
void TransferFileClient::Upload(std::string objectName,std::string filePath)
{
    Chunk chunk;
    char data[CHUNK_SIZE];
    Reply stats;
    ClientContext context;
    int len = 0;
    struct timeval start, end;

    gettimeofday(&start, NULL);
    std::ifstream infile(filePath, std::ios::binary|std::ios::in);
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
        return ;//或者抛出异常。
    }

    std::unique_ptr<ClientWriter<Chunk>> writer(stub_->Upload(&context, &stats));
    while (!infile.eof()) {
        infile.read(data, CHUNK_SIZE);
        chunk.set_buffer(data, infile.gcount());
        chunk.set_objectname(objectName);
        if (!writer->Write(chunk)) {
            std::cout << "Write fail" <<std::endl;
            std::cout << "Broken stream." <<std::endl;            // Broken stream.
            break;
        }
        len += infile.gcount();
    }
    writer->WritesDone();
    Status status = writer->Finish();
    if (status.ok()) {
        gettimeofday(&end, NULL);
        std::cout << "Transfer file costs:" <<
            (end.tv_sec-start.tv_sec) + (double)(end.tv_usec-start.tv_usec)/1000000 <<
            "s" << std::endl;
        std::cout << "server recv:" << stats.result() <<std::endl;
    } else {
        std::cout << "TransferFile rpc failed." << std::endl;
    }
}
int main(int argc, char** argv){
    TransferFileClient guide(grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials()));
//    guide.Upload("short-5000.wav","../short-5000.wav");
    guide.Download("short-5000.wav","../short-5000-download-from-cpp.wav");
    return 0;
}