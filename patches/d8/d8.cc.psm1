Import-Module (Join-Path $PSScriptRoot "..\utils.psm1")

function Patch {
    param([string]$Content)

    $Content = Add-LineBelow -Content $Content `
        -Patterns @('#include .+') `
        -Insert @"
#include <cstring>
#include <fstream>
#include "include/v8-version.h"
#include "src/snapshot/code-serializer.h"
"@
    $Content = Edit-FunctionBody -Content $Content `
        -FunctionName "Local<ObjectTemplate> Shell::CreateGlobalTemplate" `
        -Converter {
        param($Body)
        $Body = Add-BeforeReturn -Body $Body `
            -Insert @"
  global_template->Set(isolate, "loadBytecode",
                       FunctionTemplate::New(isolate, LoadBytecode));
  global_template->Set(isolate, "dumpOpcodes",
                       FunctionTemplate::New(isolate, DumpOpcodes));
"@
        return $Body
    }

    $implements = Get-Content `
        -Path (Join-Path $PSScriptRoot "disassemble.cc") `
        -Raw
    $implements += Get-Content `
        -Path (Join-Path $PSScriptRoot "metadata.cc") `
        -Raw
    $Content = Add-LineBelow -Content $Content `
        -Patterns @('void Shell::Print\(', '^}$') `
        -Insert $implements

    return $Content
}
