# 0 "main.c"
# 0 "<built-in>"
# 0 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 0 "<command-line>" 2
# 1 "main.c"
# 1 "foo.h" 1
# 1 "my_math.h" 1
float my_sin(float);
# 2 "foo.h" 2

void foo() {
    float x01 = my_sin(0.1f);
}
# 2 "main.c" 2
# 1 "bar.h" 1
# 1 "my_math.h" 1
float my_sin(float);
# 2 "bar.h" 2

void bar() {
    float x01 = my_sin(0.1f);
}
# 3 "main.c" 2

int main() {
    return 0;
}
