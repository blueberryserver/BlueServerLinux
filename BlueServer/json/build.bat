REM json 파일 하나에 모든 시트 포함해서 생성
forjson.exe -type=json -split=single


REM 각 시트별로 json 파일로 생성
REM forjson.exe -type=json -split=yes


REM 각 시트별로 csv 파일로 생성
REM forjson.exe -type=csv -split=yes