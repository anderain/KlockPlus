#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <windows.h>
#include "config.h"
#include "builder.h"
#include "preview.h"

BUILDER_CONFIG builder_config = { 0, 0, NULL, NULL, 0 };

#define cur_arg()       (argv[i])
#define next_arg()      (++i)
#define no_more_arg()   (i >= argc)
#define sz_eq(s1, s2)   (strcmp((s1), (s2)) == 0)
#define APP_NAME        "Klock-Builder"

int main(int argc, const char **argv) {
    int i = 0;

    // initialize rand
    srand(time(NULL));

// #ifdef _DEBUG
//     builder_config.use_debug = 1;
//     builder_config.preview_mode = 1;
//     builder_config.in_filename = "test.kwf";
// #endif

    // parse command line arguments
    for (; !no_more_arg(); next_arg()) {
        if (sz_eq(cur_arg(), "build")) {
            builder_config.build_mode = 1;
        }
        else if (sz_eq(cur_arg(), "preview")) {
            builder_config.preview_mode = 1;
        }
        else if (sz_eq(cur_arg(), "-d") || sz_eq(cur_arg(), "--debug")) {
            builder_config.use_debug = 1;
        }
        else if (sz_eq(cur_arg(), "-i")) {
            next_arg();
            if (no_more_arg()) {
                printf("Invalid option '-i', missing input file.\n");
                return -1;
            }
            builder_config.in_filename = cur_arg();
        }
        else if (sz_eq(cur_arg(), "-o")) {
            next_arg();
            if (no_more_arg()) {
                printf("Invalid option '-o', missing output file.\n");
                return -1;
            }
            builder_config.out_filename = cur_arg();
        }
    }

    if (builder_config.build_mode) {
        if (builder_config.in_filename == NULL) {
            printf("Build error: missing input file.\n");
            return -1;
        }
        if (builder_config.out_filename == NULL) {
            printf("Build error: missing output file.\n");
            return -1;
        }
        build_from_kbasic(
            builder_config.in_filename,
            builder_config.out_filename
        );
    }
    else if (builder_config.preview_mode) {
        const char *        filename;
        kb_runtime_error_t  error_ret;
        int                 ret;
        kb_machine_t*       app;

        if (builder_config.in_filename == NULL) {
            printf("Preview error: missing input file.\n");
            return -1;
        }

        filename = builder_config.in_filename;

        app = load_app(filename);
        if (!app) {
            printf("Failed to load '%s', file not exist for invalid format\n", filename);
            return 1;
        }
        
        if (builder_config.use_debug) {
            DBG_print_header(app->header);
        }

        ret = start_preview(app, &error_ret);
        if (!ret) {
            char error_message[200];
            format_exec_error(&error_ret, error_message, sizeof(error_message));
            printf("Runtime Error: %s\n%s\n", filename, error_message);
        }
        machine_destroy(app);
    }
    else {
        OPENFILENAMEA ofn;
        char sz_file[260] = { 0 };

        ZeroMemory(&ofn, sizeof(ofn));
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = NULL;
        ofn.lpstrFile = sz_file;
        ofn.nMaxFile = sizeof(sz_file);
        ofn.lpstrFilter = "Klock watchface file (*.kwf)\0*.kwf\0";
        ofn.nFilterIndex = 1;
        ofn.lpstrFileTitle = "Select KWF file to preview";
        ofn.nMaxFileTitle = 0;
        ofn.lpstrInitialDir = NULL;
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;

        if (GetOpenFileNameA(&ofn) == TRUE) {
            const char *        filename;
            kb_runtime_error_t  error_ret;
            int                 ret;
            kb_machine_t*       app;
            char                message_buf[260];

            filename = sz_file ;

            app = load_app(filename);
            if (!app) {
                sprintf(message_buf, "Failed to load '%s', file not exist for invalid format\n", filename);
                MessageBoxA(NULL, message_buf, APP_NAME, MB_OK);
                return 1;
            }
        
            if (builder_config.use_debug) {
                DBG_print_header(app->header);
            }

            ret = start_preview(app, &error_ret);
            if (!ret) {
                char error_message[200];
                format_exec_error(&error_ret, error_message, sizeof(error_message));
                sprintf(message_buf, "Runtime Error: %s\n%s\n", filename, error_message);
                MessageBoxA(NULL, message_buf, APP_NAME, MB_OK);
            }
            machine_destroy(app);
        }
        else {
            puts("Usage:");
            puts("Build watchface from KBasic script:\n");
            puts("    klockbuilder build -i in_filename -o out_filename\n");
            puts("Preview watchface:\n");
            puts("    klockbuilder preview -i in_filename\n");
            puts("Other options:");
            puts("    -d, --debug: enable debugging information");
        }
    }

    return 0;
}