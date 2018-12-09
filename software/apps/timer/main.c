#include "app_timer.h"
#include "nrf_drv_clock.h"

APP_TIMER_DEF(single_shot_timer); /* Defining core timer */
APP_TIMER_DEF(repeated_timer); /* Defining core timer */


// Function starting the internal LFCLK oscillator.
// This is needed by RTC1 which is used by the application timer
// (When SoftDevice is enabled the LFCLK is always running and this is not needed).
static void lfclk_request(void)

{
    uint32_t err_code = nrf_drv_clock_init();
    APP_ERROR_CHECK(err_code);
    nrf_drv_clock_lfclk_request(NULL);
}

//Single Shot timer handler Function
static void single_shot_timer_handler(void * p_context){
  printf("SINGLE SHOT TIMER FIRED! \n");
}

//Repeater Shot timer handler Function
static void repeated_timer_handler(void * p_context){
  printf("REPEATED TIMER FIRED! \n");
}




//Creating timers

static void create_timers(){
  uint32_t err_code;
  err_code = app_timer_create(&repeated_timer,
                            APP_TIMER_MODE_REPEATED,
                          repeated_timer_handler);
  APP_ERROR_CHECK(err_code);

  err_code = app_timer_create(&single_shot_timer,
                            APP_TIMER_MODE_SINGLE_SHOT,
                          single_shot_timer_handler);
  APP_ERROR_CHECK(err_code);


}

int main(void){
  printf("STARTING FUNCTION! \n");
  // Request LF clock.
  lfclk_request();
  // Initialize the application timer module.
  app_timer_init();
  //creating the timers
  create_timers();
  //Starting Single Shot Timer
  // uint32_t err_code;
  // err_code = app_timer_start(single_shot_timer, APP_TIMER_TICKS(200), NULL);
  // APP_ERROR_CHECK(err_code);
  //Starting Repeated Timer
  uint32_t err_code;
  err_code = app_timer_start(repeated_timer, APP_TIMER_TICKS(200), NULL);
  APP_ERROR_CHECK(err_code);

  // Main loop.
  while (true)
  {
    // Wait for interrupt.
    __WFI();
  }

}
