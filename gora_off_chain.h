#ifndef GORA_OFF_CHAIN_H
#define GORA_OFF_CHAIN_H

#define GORA_RC_SUCCESS 0
#define GORA_RC_CONTINUE 1
#define GORA_RC_ERROR_GENERAL 100
#define GORA_RC_ERROR_DATA_NOT_RECEIVED 101
#define GORA_RC_ERROR_EXTRA_STEP 102

#define GORA_MAX_REQ_ARGS 100
#define GORA_MAX_REQ_ARG_LEN 1000
#define GORA_MAX_RESP_VALUES 100
#define GORA_MAX_RESP_VALUE_LEN 1000
#define GORA_SCRATCH_SPACE_SIZE 16384

#define GORA_DECLARE_FUNC_MAIN __attribute__((export_name("goraMain"))) extern

extern void gora_log(const char*, const int);
extern void gora_request_url(const char*, const char*);
extern void gora_set_next_url_param(const char*);

struct gora_context_t {
  unsigned int version;
  unsigned int step;
  char *result;
  char ora_req_args[GORA_MAX_REQ_ARGS][GORA_MAX_REQ_ARG_LEN];
  char resp_values[GORA_MAX_RESP_VALUES][GORA_MAX_RESP_VALUE_LEN];
  char scratch[GORA_SCRATCH_SPACE_SIZE];
};

#endif