REM for %%f in (*.proto) do for /F "tokens=1 delims=1." %%p in ("%%f") do  .\protoc.3.0.0 --cpp_out=..\Cpp\ %%p.proto
for %%f in (*.proto) do for /F "tokens=1 delims=1." %%p in ("%%f") do  .\csharpbin\protogen -i:%%p.proto -o:..\CSharp\%%p.cs

