#pragma ONCE
#ifndef PROGRAM_H
#define PROGRAM_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef __cheaderfile
extern "C" {
#endif
#ifndef BUILD_MY_DLL
#define SHARED_LIB __declspec(dllexport)
#else
#define SHARED_LIB __declspec(dllexport)
#endif

#ifdef _GNUC_
    #define NORETERN __attribute__((__noreturn__))
    void helloworld_set_hello(helloworld *self, int age);
    int helloworld_get_hello(const helloworld *self);
    void helloWorld(int age);
    void compileAndRun();
    helloworld new_helloworld(void);
#endif

#define CMD(json) \
    do { \
        FILE *file = fopen(json, "r"); \
        if (file == NULL) { \
            fprintf(stderr, "파일을 열 수 없습니다.\n"); \
            exit(1); \
        } \
        fseek(file, 0, SEEK_END); \
        long fileSize = ftell(file); \
        fseek(file, 0, SEEK_SET); \
        char *jsonContent = (char *)malloc(fileSize + 1); \
        fread(jsonContent, 1, fileSize, file); \
        fclose(file); \
        jsonContent[fileSize] = '\0'; \
        clock_t start = clock(); \
        JsonObject parsedJson = parseJson(jsonContent); \
        int year = 0; \
        for (size_t i = 0; i < parsedJson.size; i++) { \
            if (strcmp(parsedJson.pairs[i].key, "year") == 0) { \
                year = atoi(parsedJson.pairs[i].value); \
            } \
        } \
        Command helloworld_cmd = { .execute = helloWorld }; \
        helloworld_cmd.execute(year); \
        free(jsonContent); \
        free(parsedJson.pairs[0].value); \
        free(parsedJson.pairs); \
        clock_t end = clock(); \
        double elapsed_time = ((double) (end - start)) / CLOCKS_PER_SEC; \
        printf("파일을 읽는 데 걸린 시간: %f 초\n", elapsed_time); \
    } while (0);

#define MAIN(json) \
    int main() { \
        helloworld hello = new_helloworld(); \
        hello.sethello(&hello, 31); \
        hello.gethello(&hello); \
        CMD(json); \
        return 0; \
    }

typedef struct {
    char* key;
    char* value;
} JsonPair;

typedef struct {
    JsonPair* pairs;
    size_t size;
} JsonObject;

typedef struct {
    void (*execute)(int);
} Command;
typedef struct helloworld helloworld;

struct helloworld {
    int age;

    void (*sethello)(helloworld *, int);
    int (*gethello)(const helloworld *);
};


void helloworld_set_hello(helloworld *self, int age);
int helloworld_get_hello(const helloworld *self);

void helloWorld(int age) {
    printf("HELLO %d년!\n", age);
}

JsonObject parseJson(const char* json) {
    JsonObject result = {0};
    
    const char* yearStart = strstr(json, "\"year\"");
    if (yearStart != NULL) {
        const char* yearValueStart = strchr(yearStart, ':');
        if (yearValueStart != NULL) {
            yearValueStart++;
            const char* yearEnd = strchr(yearValueStart, ',');
            if (yearEnd == NULL) {
                yearEnd = strchr(yearValueStart, '}');
            }
            
            if (yearEnd != NULL) {
                size_t yearLength = yearEnd - yearValueStart;
                result.pairs = (JsonPair*)malloc(sizeof(JsonPair));
                // result.pairs[0].key = "year";
                result.pairs[0].key = strdup("year");
                result.pairs[0].value = (char*)malloc(yearLength + 1);
                strncpy(result.pairs[0].value, yearValueStart, yearLength);
                result.pairs[0].value[yearLength] = '\0';
                result.size = 1;
            }
        }
    }

    return result;
}

helloworld new_helloworld(void) {
    helloworld hello;
    hello.age = 0;
    hello.sethello = helloworld_set_hello;
    hello.gethello = helloworld_get_hello;

    return hello;
}

void helloworld_set_hello(helloworld *self, int age) {
    self->age = age;
}

int helloworld_get_hello(const helloworld *self) {
    printf("내 나이: %d\n", self->age);
    return self->age;
}

#endif