#define NOB_IMPLEMENTATION
#define NOB_STRIP_PREFIX
#include "nob.h"

static inline void strip_suffix(char *s) {
  char *p = s;
  char *last_dot = NULL;
  while (*p) {
    if (*p == '.') {
      last_dot = p;
    }
    p++;
  }

  // Strip the last suffix if it exists
  if (last_dot && last_dot != s) {
    *last_dot = '\0';
  }
}

typedef struct {
  const char *name;
  const char *parent;
  Nob_File_Type type;
} File;

typedef struct {
  File *items;
  size_t count;
  size_t capacity;
} Files;

static void traverse_directory_and_append_to_files(const char *source,
                                                   Files *files) {
  File_Paths fp = {0};
  read_entire_dir(source, &fp);
  da_foreach(const char *, file, &fp) {
    size_t index = file - fp.items;
    File curr = {0};
    String_Builder sb = {0};

    if (fp.items[index][0] == '.')
      continue;

    sb_appendf(&sb, "%s%s", source, *file);

    curr.name = *file;
    curr.type = get_file_type(sb.items);
    curr.parent = source;

    if (curr.type == NOB_FILE_DIRECTORY) {
      sb_append_cstr(&sb, "/");
      traverse_directory_and_append_to_files(sb.items, files);
      continue; // Don't list directories in the files to process
    }

    sb_append_null(&sb);

    if (curr.type != NOB_FILE_REGULAR) {
      nob_log(NOB_ERROR, "Unsupported file type for: %s", sb.items);
      return;
    }

    da_append(files, curr);
  }
}

int main(int argc, char **argv) {
  NOB_GO_REBUILD_URSELF(argc, argv);

  char *src_folder = "src/";
  char *dst_folder = "build/";

  if (!nob_mkdir_if_not_exists(dst_folder))
    return 1;

  Nob_Cmd cmd = {0};
  Nob_Procs proc = {0};
  Files files_to_process = {0};

  traverse_directory_and_append_to_files(src_folder, &files_to_process);

  da_foreach(File, file, &files_to_process) {
    size_t index = file - files_to_process.items;
    String_Builder input = {0};
    String_Builder output = {0};
    sb_appendf(&input, "%s%s", file->parent, file->name);
    strip_suffix((char *)file->name); // Strip the suffix for output
    sb_appendf(&output, "%s%s", dst_folder, file->name);
    sb_append_null(&input);
    sb_append_null(&output);

    nob_log(NOB_INFO, "Processing file: %zu %s -> %s", index, input.items,
            output.items);
    nob_cc(&cmd);
    nob_cc_flags(&cmd);
    nob_cc_inputs(&cmd, input.items);
    nob_cc_output(&cmd, output.items);
    nob_cmd_append(&cmd, "-lm", "-lraylib");
    da_append(&proc, nob_cmd_run_async_and_reset(&cmd));
  }
  nob_procs_wait(proc);

  return 0;
}
