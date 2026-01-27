#!/bin/bash
# Script to fetch Unity test framework (minimal files only)

UNITY_VERSION="v2.6.0"
UNITY_REPO="https://raw.githubusercontent.com/ThrowTheSwitch/Unity/${UNITY_VERSION}/src"
UNITY_DIR="shlib/unity"

echo "Setting up Unity (version: ${UNITY_VERSION})..."

# Create target directory
mkdir -p ${UNITY_DIR}

# Files to fetch
FILES=("unity.c" "unity.h" "unity_internals.h")

for FILE in "${FILES[@]}"; do
    TARGET="${UNITY_DIR}/${FILE}"
    if [ ! -f "${TARGET}" ]; then
        echo "Downloading ${FILE}..."
        curl -sSL "${UNITY_REPO}/${FILE}" -o "${TARGET}"
    else
        echo "Already exists: ${TARGET}"
    fi
done

echo "Unity setup complete!"