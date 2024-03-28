BUILD_DIR := "./build"
SOURCE_DIR := "."
BUILD_TYPE := "Debug"
TENSE_TEST := "ON"
TENSE_BENCH := "ON"
GENERATOR := `command -v -- ninja >/dev/null && echo "Ninja" || echo "Unix Makefile"`

export CC := "gcc"
export CXX := "g++"

clean:
    rm -fr {{BUILD_DIR}} CMakeCache.txt

config: clean
    cmake -S{{SOURCE_DIR}} -B{{BUILD_DIR}} \
        -G"{{GENERATOR}}" \
        -DCMAKE_BUILD_TYPE="{{BUILD_TYPE}}" \
        -DTENSE_TEST="{{TENSE_TEST}}" \
        -DTENSE_BENCH="{{TENSE_BENCH}}"

build:
    cmake --build {{BUILD_DIR}}

test: build
    {{BUILD_DIR}}/test/unit_test

bench: test
    {{BUILD_DIR}}/bench/bench
