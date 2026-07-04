param(
    [string]$mode = "a"  # "a" or "b"
)

$threads = 1,2,4,6,8,10,12,14,16
$outfile = "results/q5_${mode}_speedup.csv"
"threads,time_s" | Out-File $outfile -Encoding utf8

foreach ($t in $threads) {
    $env:OMP_NUM_THREADS = "$t"
    $times = @()
    1..5 | ForEach-Object {
        $line = & .\q5_run.exe $mode 42 2000000 $t
        if ($line -match 'seconds=([\d\.]+)') {
            $times += [double]$matches[1]
        }
    }
    $avg = ($times | Measure-Object -Average).Average
    "$t,$avg" | Out-File $outfile -Append -Encoding utf8
}

Write-Host "Created $outfile"
