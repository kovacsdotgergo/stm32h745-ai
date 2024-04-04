# Notes to remember

## Merging binaries

``` bash
command = "srec_cat"
args = [
    "${HEX_FILE_0}", "-intel",
    "${HEX_FILE_1}", "-intel",
    "-o", "${MERGE_HEX}", "-intel",
    "-Line_Length", "44"
]
```
