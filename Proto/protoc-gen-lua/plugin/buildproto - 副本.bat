rem �л���.protoЭ�����ڵ�Ŀ¼  
cd E:\MaJiang\Proto\proto_raw 
rem ����ǰ�ļ����е�����Э���ļ�ת��Ϊlua�ļ�  
for %%i in (*.proto) do (  
rem ^%%%%i  
echo %%i  
"E:\MaJiang\Proto\protoc-gen-lua\protoc.exe" --plugin=protoc-gen-lua="E:\MaJiang\Proto\protoc-gen-lua\plugin\protoc-gen-lua.bat" --lua_out=. %%i  
  
)  
echo end  
pause  