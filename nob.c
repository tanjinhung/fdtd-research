#define NOB_IMPLEMENTATION
#define NOB_STRIP_PREFIX
#include "nob.h"

#define BUILD_FOLDER "build/"
#define SRC_FOLDER "src/"

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

int main(int argc, char **argv) {
  NOB_GO_REBUILD_URSELF(argc, argv);
  if (!nob_mkdir_if_not_exists(BUILD_FOLDER))
    return 1;
  Nob_Cmd cmd = {0};

  // nob_cc(&cmd);
  // nob_cc_flags(&cmd);
  // nob_cc_output(&cmd, BUILD_FOLDER "fdtd");
  // nob_cc_inputs(&cmd, SRC_FOLDER "fdtd.c");
  // nob_cmd_append(&cmd, "-lm", "-lraylib");
  // if (!nob_cmd_run_sync_and_reset(&cmd)) return 1;

  File_Paths fp = {0};
  read_entire_dir(SRC_FOLDER, &fp);

  da_foreach(const char *, file_ptr, &fp) {
    size_t index = file_ptr - fp.items;
    if (fp.items[index][0] == '.')
      continue;
    printf("File: %s\n", fp.items[index]);

    String_Builder sb_inputs = {0};
    String_Builder sb_outputs = {0};
    sb_appendf(&sb_inputs, "%s%s", SRC_FOLDER, fp.items[index]);
    sb_append_null(&sb_inputs);
    strip_suffix((char *)fp.items[index]);
    sb_appendf(&sb_outputs, "%s%s", BUILD_FOLDER, fp.items[index]);
    sb_append_null(&sb_outputs);

    nob_cc(&cmd);
    nob_cc_flags(&cmd);
    nob_cc_inputs(&cmd, sb_inputs.items);
    nob_cc_output(&cmd, sb_outputs.items);
    nob_cmd_append(&cmd, "-lm", "-lraylib");

    if (!nob_cmd_run_sync_and_reset(&cmd))
      return 1;

    sb_free(sb_inputs);
    sb_free(sb_outputs);
  }

  return 0;
}
