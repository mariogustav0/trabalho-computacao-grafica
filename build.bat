@echo off
echo ================================================================
echo  Compilando Trabalho de Computacao Grafica I (UFC)
echo  Releitura Salvador Dali - Modulo Mesa e Bloco (Marlon)
echo ================================================================

gcc main.c -lglut -lopengl32 -lglu32 -o main.exe

if %ERRORLEVEL% equ 0 (
    echo [SUCESSO] Compilacao concluida sem erros!
    echo Executando main.exe...
    main.exe
) else (
    echo [ERRO] Falha na compilacao. Verifique se o GCC e FreeGLUT estao no PATH.
    pause
)
