#define NOB_IMPLEMENTATION
#include "nob.h"

#define BUILD_FOLDER "build/"
#define SRC_FOLDER "src/"

int main(int argc, char **argv) {
  NOB_GO_REBUILD_URSELF(argc, argv);
  if (!nob_mkdir_if_not_exists(BUILD_FOLDER))
    return 1;
  Nob_Cmd cmd = {0};

  nob_cc(&cmd);
  nob_cc_flags(&cmd);
  nob_cc_output(&cmd, BUILD_FOLDER "fdtd");
  nob_cc_inputs(&cmd, SRC_FOLDER "fdtd.c");
  nob_cmd_append(&cmd, "-lm");

  if (!nob_cmd_run_sync_and_reset(&cmd))
    return 1;

  nob_cmd_append(&cmd, BUILD_FOLDER "fdtd");

  if (!nob_cmd_run_sync_and_reset(&cmd))
    return 1;

  return 0;
}
