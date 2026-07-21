@echo off
set Protoc_Path=D:\cppsoft\grpc\build_ok\third_party\protobuf\Debug\protoc.exe
set Grpc_Plugin_Path=D:\cppsoft\grpc\build_ok\Debug\grpc_cpp_plugin.exe
set Proto_File=message.proto
echo generating....
%Protoc_Path% -I="." --grpc_out="." --plugin=protoc-gen-grpc="%Grpc_Plugin_Path%" "%Proto_File%"
%Protoc_Path% --cpp_out=. "%Proto_File%"
echo Done.