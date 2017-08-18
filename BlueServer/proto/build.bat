for %%f in (*.proto) do for /F "tokens=1 delims=1." %%p in ("%%f") do  .\protoc.3.0.0 --cpp_out=..\Cpp\ %%p.proto
REM for %%f in (*.proto) do for /F "tokens=1 delims=1." %%p in ("%%f") do  ..\..\..\3rdLibrary\protobuf\csharpbin\protogen -i:%%p.proto -o:..\CSharp\%%p.cs

