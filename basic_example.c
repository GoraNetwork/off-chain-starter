#include "gora_off_chain.h"

static char* basic_result = "Hello world!";

GORA_DECLARE_FUNC_MAIN
int gora_main(struct gora_context_t* gora_ctx) {

  gora_ctx->result = basic_result;
  return GORA_RC_SUCCESS;
}
