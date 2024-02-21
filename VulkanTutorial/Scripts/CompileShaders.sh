#!/bin/bash

function CompileShader()
{
    local sourceFile=$1
    local outputFilename="${sourceFile}.spv"

    echo "Compiling $sourceFile to $outputFilename"
    glslc ${1} -o $outputFilename
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