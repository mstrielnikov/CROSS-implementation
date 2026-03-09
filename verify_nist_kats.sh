#!/bin/bash
# verify_nist_kats.sh - Build and verify NIST Known Answer Tests for CROSS

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m' # No Color

echo "--- Starting NIST KAT Verification ---"

# 1. Build KAT generation binaries
echo "Building KAT generation binaries..."
mkdir -p Additional_Implementations/KAT_Generation/build
cd Additional_Implementations/KAT_Generation/build
cmake ../ -DREFERENCE=1 > /dev/null
make -j$(nproc) > /dev/null

# 2. Generate KAT vectors
echo "Generating KAT vectors (this may take a moment)..."
for f in bin/*; do
    echo "  Running $(basename $f)..."
    ./$f > /dev/null
done

# 3. Verify against checksums
echo "Verifying generated vectors against checksums..."
# The gen_all_kat.sh moves files, we'll do the same to the top-level KAT dir
mkdir -p ../../../KAT
mv *.req *.rsp ../../../KAT/
cd ../../../KAT

if sha512sum -c sha_512_sum_KATs; then
    echo -e "${GREEN}SUCCESS: All NIST KAT vectors verified!${NC}"
else
    echo -e "${RED}FAILURE: NIST KAT verification failed!${NC}"
    exit 1
fi
