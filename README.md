# BuildDccEX

[![DCC-EX CS compile](https://github.com/grbba/BuildDccEX/actions/workflows/build_dccex_cs.yml/badge.svg)](https://github.com/grbba/BuildDccEX/actions/workflows/build_dccex_cs.yml)

Builds the DCC-EX commandstation on all supported boards once per day from master

It replaces the files from the CS repo with the files in the patch folder.

Under normal circumstances we wold only support the latest realease of the CS ( as applying the patches on a ongoing dev branch is risky :) ) and build a release from that.

you can use the cli without any patches applied and connect to the CS but dias only over serial connection and operational commnads will work - all the spcific cli stuff not

