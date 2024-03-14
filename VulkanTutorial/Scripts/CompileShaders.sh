#!/bin/bash

function CompileShader()
{
    local sourceFile=$1

    echo "Compiling $sourceFile"
    glslangValidator -V ${1} -o ${1}.spv
}

function ScanDirectory()
{
    local targetDirectory=$1
    echo "Scanning target directory ${targetDirectory}"
    local files=`find ${targetDirectory} -type f -name '*.frag' -or -name '*.vert'`
    
    for i in $files 
    do
        CompileShader $i
    done
}

ScanDirectory "../Shaders"