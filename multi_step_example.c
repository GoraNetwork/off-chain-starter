#include "gora_off_chain.h"

GORA_DECLARE_FUNC_MAIN
int gora_main(struct gora_context_t* gora_ctx) {

  switch (gora_ctx->step) {
    // The first step - ask for postcode earth coordinates to be retrieved.
    case 0:

      gora_request_url("http://api.postcodes.io/postCodes/##",
                       "jsonpath:$.result.latitude\tjsonpath:$.result.longitude");

      // Set first URL template parameter ("##" in the above) to the first oracle
      // request argument (the postcode).
      gora_set_next_url_param(gora_ctx->ora_req_args[0]);

      // End the first step, requesting continuation to the next one.
      return GORA_RC_CONTINUE;

    // Second step - ask for retrival of weather at the postcode coordinates
    // received after the step 1.
    case 1:

      // Check that we received the requested data.
      if (gora_ctx->resp_values[0][0] == 0) // is the first value an empty string?
        return GORA_RC_ERROR_DATA_NOT_RECEIVED; // signal error and abort processing

      // Request the weather data, substituting coordinate values we received.
      gora_request_url("https://api.open-meteo.com/v1/forecast?current_weather=true&latitude=##&longitude=##",
                       "jsonpath:$.current_weather.temperature");
      gora_set_next_url_param(gora_ctx->resp_values[0]);
      gora_set_next_url_param(gora_ctx->resp_values[1]);
      return GORA_RC_CONTINUE;

    // Last step - report temperature value as the oracle result.
    case 2:

      gora_ctx->result = gora_ctx->resp_values[0];
      return GORA_RC_SUCCESS; // finish processing, no more steps

    default:
      // No more steps should be performed.
      return GORA_RC_ERROR_EXTRA_STEP;
  }
}
