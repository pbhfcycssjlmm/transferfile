# 运行步骤
1. git clone https://github.com/pbhfcycssjlmm/transferfile.git
2. cd transferfile/go
3. 如果想自定义module名称，则删除该文件下的go.mod和go.sum，然后go mod init 你的module名称，否则跳过该步骤 #在这里我使用的是transferfile，如果你需要改动module名称，注意改动transfer_server/main.go中的引用，详细见项目说明第三点
4. go run transfer_server/main.go
5. 自行实现go的client客户端，或者运行transferfile/cpp使用C++实现的client

# 项目说明
1. 由于仅需要go作为服务端，因此client就不在go这边实现了，使用C++的client即可
2. 编译IDL命令：protoc -I ../ --go_out=. --go-grpc_out=.  ../proto/transfer_file.proto。
- 就目前来看: "-I ../" 起的作用是告诉protoc去父目录下找proto文件
- "--go_out=. --go-grpc_out=."配合“go_package = "./pb/transfer";”告诉protoc将生成的pb.go放在go_out+go_package="." + "./pb/transfer"="./pb/transfer"下；grpc.pb.go同理
3. go.mod声明了module transferfile，因此transfer_server/main.go中要引用生成的grpc文件，需要指定module路径+go_package，在此项目中是"transferfile/pb/transfer"