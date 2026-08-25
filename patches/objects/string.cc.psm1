Import-Module (Join-Path $PSScriptRoot "..\utils.psm1")

function Patch {
    param([string]$Content)
    $legacyV8 = $Content -match 'StringCharacterStream stream\(\*this, start\);'

    $Content = Edit-FunctionBody -Content $Content `
        -FunctionName "void String::StringShortPrint" `
        -Converter {
        param($Body)
        $ifCondition = "len > kMaxShortPrintLength"
        $Body = Set-CommentLine -Content $Body `
            -Pattern $ifCondition
        $Body = Add-LineBelow -Content $Body `
            -Patterns @($ifCondition) `
            -Insert "  /*"
        $Body = Add-LineBelow -Content $Body `
            -Patterns @($ifCondition, '}') `
            -Insert "  */"
        return $Body
    }

    $Content = Edit-FunctionBody -Content $Content `
        -FunctionName "void String::PrintUC16" `
        -Parameter "StringStream" `
        -Converter {
        param($Body)

        $streamReceiver = if ($legacyV8) { "*this" } else { "this" }

        return @"
  if (end < 0) end = length();
  StringCharacterStream stream($streamReceiver, start);
  for (int i = start; i < end && stream.HasMore(); i++) {
    uint16_t c = stream.GetNext();
    if (c == '\n') {
      accumulator->Add("\\n");
    } else if (c == '\r') {
      accumulator->Add("\\r");
    } else if (c == '\\') {
      accumulator->Add("\\\\");
    } else if (c < 32 || (c >= 127 && c < 160)) {
      accumulator->Add("\\x%02x", c);
    } else {
      accumulator->Add("\\u%04x", c);
    }
  }
"@
    }

    return $Content
}
