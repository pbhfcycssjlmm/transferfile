package main

import (
	"bufio"
	"flag"
	"fmt"
	"github.com/google/uuid"
	"google.golang.org/grpc"
	"io"
	"log"
	"net"
	"os"

	pb "transferfile/pb/transfer"
)

type TransferFileServer struct {
	pb.UnimplementedTransferFileServer // 通过结构体嵌套的方式默认实现mustEmbedUnimplementedQueryServer()这个方法，查看2.2.1
}

func (t *TransferFileServer) Upload(stream pb.TransferFile_UploadServer) error {
	objectName := ""
	guid := uuid.New()
	guidStr := guid.String()
	filePath := guidStr + ".wav"
	var round int
	file, err := os.OpenFile(filePath, os.O_WRONLY|os.O_CREATE|os.O_TRUNC, 0666)
	if err != nil {
		fmt.Println(filePath, "创建失败", err)
	}
	//及时关闭file句柄
	defer file.Close()
	write := bufio.NewWriter(file)
	round = 0
	recvLen := 0
	for {
		round = round + 1
		chunkData, err := stream.Recv()
		if err == io.EOF {
			write.Flush()
			os.Rename(filePath, objectName)
			return stream.SendAndClose(&pb.Reply{
				Result: int32(recvLen),
			})
		}
		if err != nil {
			return err
		}
		if round == 1 {
			objectName = chunkData.ObjectName
		}
		tmp := chunkData.Buffer
		recvLen = recvLen + len(tmp)
		write.Write(tmp)
	}
}

var (
	port = flag.Int("port", 50051, "The server port")
)

func main() {
	flag.Parse()
	lis, err := net.Listen("tcp", fmt.Sprintf(":%d", *port))
	if err != nil {
		log.Fatalf("failed to listen: %v", err)
	}
	s := grpc.NewServer()
	pb.RegisterTransferFileServer(s, &TransferFileServer{})
	log.Printf("server listening at %v", lis.Addr())
	if err := s.Serve(lis); err != nil {
		log.Fatalf("failed to serve: %v", err)
	}
}
