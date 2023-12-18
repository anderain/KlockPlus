#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <time.h>
#include "../core/kbasic.h"
#include "convert.h"
#include "preview.h"

char *read_text_file(const char *filename) {
    FILE *fp;
    int length = 0;
    char *buf;

    fp = fopen(filename, "rb");

    if (!fp) {
        return NULL;
    }

    fseek(fp, 0, SEEK_END);
    length = ftell(fp) + 1;

    buf = (char *)malloc(length);
    memset(buf, 0, length);
    fseek(fp, 0, SEEK_SET);
    fread(buf, length, 1, fp);

    fclose(fp);
    return buf;
}

int write_compiled_file(const char *filename, const void * data, int length) {
    FILE *fp;

    fp = fopen(filename, "wb");

    if (!fp) {
        return 0;
    }

    fwrite(data, length, 1, fp);

    fclose(fp);

    return 1;
}

int build_from_kbasic(const char *filename, const char * output_filename, const char *assets_path) {
    kb_error_t      error_ret;
    kb_context_t    *context;
    int             ret;
    char            *text_buf;
    char            *text_ptr;
    int             is_success = 1;
    int             line_count = 1;
    int             result_byte_length;
    unsigned char * result_raw;

    text_buf = read_text_file("sample.kbs");
    if (!text_buf) {
        puts("failed to read");
        return 0;
    }

    context = kb_compile_start();

    // set bmp file loader and assets path
    context->bmp_load_convert = k_convert_bmp_asset;
    context->asset_root_path = assets_path;

    // first loop, scan all label
    for (text_ptr = text_buf, line_count = 1; *text_ptr; line_count++) {
        char line[1000];
        char *line_ptr = line;

        // get line from file buffer
        do {
            *line_ptr++ = *text_ptr++;
        } while(*text_ptr != '\n' && *text_ptr != '\0');

        *line_ptr = '\0';

        ret = kb_scan_label(line, context, &error_ret);

        if (!ret) {
            is_success = 0;
            goto compile_end;
        }
    }

    // second loop, compile commands
    // after the second loop,
    // the `ptr` in `param` of GOTO and IFGOTO is filled with the label pointer.
    for (text_ptr = text_buf, line_count = 1; *text_ptr; line_count++) {
        char line[1000];
        char *line_ptr = line;

        // get line from file buffer
        do {
            *line_ptr++ = *text_ptr++;
        } while(*text_ptr != '\n' && *text_ptr != '\0');

        *line_ptr = '\0';

        // remove remarks
        for (line_ptr = line; *line_ptr && *line_ptr != '#'; ++line_ptr) NULL;
        *line_ptr = '\0';

        ret = kb_compile_line(line, context, &error_ret);

        if (!ret) {
            is_success = 0;
            goto compile_end;
        }
    }

    // This call will update the `param` of
    // GOTO and IFGOTO commands and add a last STOP command
    kb_compile_end(context);

compile_end:

    if (is_success) {
        puts("-------- STRING --------");
        DBG_print_context_list_text(context);

        puts("-------- VARIABLE --------");
        DBG_print_context_variables(context);

        puts("-------- IMAGES --------");
        DBG_print_context_list_image(context);

        puts("-------- LABELS --------");
        DBG_print_context_list_label(context);

        puts("-------- COMMAND --------");
        DBG_print_context_command_list(context);
    }
    else {
        char buf[200];
        kb_format_error(&error_ret, buf, sizeof(buf));
        printf("(%d) ERROR: %s\n", line_count, buf);
    }

    ret = kb_serialize(context, &result_raw, &result_byte_length);
    if (ret) {
        ret = write_compiled_file(output_filename, result_raw, result_byte_length);
        if (!ret) {
            puts("Failed to write output file");
        }

        // DBG_print_header((KLOCK_WATCHFACE_HEADER *)result_raw);

        free(result_raw);
    } else {
        puts("Failed to serialize");
    }

    context_destroy(context);
    free(text_buf);

    return is_success ? 1 : 0;
}

int main(int argc, const char **argv) {
    srand(time(NULL));

    if (argc == 4 && strcmp(argv[1], "build") == 0) {
        const char *    in_filename = argv[2];
        const char *    out_filename = argv[3];
        char            path[1000];
        int             i;

        GetFullPathNameA(in_filename, sizeof(path), path, NULL);

        for (i = strlen(path) - 1; i >= 0; --i) {
            if (path[i] == '\\') {
                break;
            }
        }

        if (i <= 0) {
            puts("Invalid script file path...");
            return -1; 
        }

        path[i + 1] = '\0';

        build_from_kbasic(in_filename, out_filename, path);
    }
    else {

        const char *        filename = "test.kwf";
        kb_runtime_error_t  error_ret;
        int                 ret;
        kb_machine_t*       app;

        app = load_app(filename);
        if (!app) {
            printf("Failed to load '%s'\n", filename);
            return 1;
        }
        DBG_print_header(app->header);
        
        ret = start_preview(app, &error_ret);
        if (!ret) {
            char error_message[200];
            format_exec_error(&error_ret, error_message, sizeof(error_message));
            printf("Runtime Error: %s\n%s\n", filename, error_message);
        }

        machine_destroy(app);
    }
    // printf("Usage:\n");
    // printf("Build from KBasic script:\n%s build input_file output_file\n", argv[0]);
    return 0;
}