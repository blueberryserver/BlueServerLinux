REM json ���� �ϳ��� ��� ��Ʈ �����ؼ� ����
forjson.exe -type=json -split=single

forjson.exe -type=proto -split=single
xcopy .\*.proto ..\proto /a /d /y

REM �� ��Ʈ���� json ���Ϸ� ����
REM forjson.exe -type=json -split=yes


REM �� ��Ʈ���� csv ���Ϸ� ����
REM forjson.exe -type=csv -split=yes