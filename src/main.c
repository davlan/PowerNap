#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"
#include "mini-printf.h"

#define MY_UUID { 0xD9, 0x5E, 0x3C, 0x87, 0x91, 0x86, 0x4B, 0xD7, 0xAE, 0xB3, 0xA7, 0x71, 0xD3, 0x5D, 0x2B, 0xED }
PBL_APP_INFO(MY_UUID, "Power Nap", "davidgray Photography", 1, 0, RESOURCE_ID_IMAGE_MENU_ICON, APP_INFO_STANDARD_APP /* App version */);

Window window;
AppTimerHandle timer_handle;
AppTimerHandle timer_handle_update;
TextLayer textLayer;
int time_remaining = 15;
bool stop;
static char str[60];

#define COOKIE_END 1
#define COOKIE_UPDATE 2

void handle_timer(AppContextRef ctx, AppTimerHandle handle, uint32_t cookie) {
	(void)ctx;
	(void)handle;

	if (cookie == COOKIE_END) {
		const VibePattern custom_pattern = {
		  .durations = (uint32_t []) {500, 500, 500, 500, 500, 500, 500, 500, 5000, 0},
		  .num_segments = 10
	    };
	  
		vibes_enqueue_custom_pattern(custom_pattern);
		text_layer_set_text(&textLayer, "Nap time is over. Go forth and conquer!");
	} 
	
	if (cookie == COOKIE_UPDATE) {
		--time_remaining;
		if(time_remaining > 0) {
			mini_snprintf(str, 21, "%d minutes remaining", time_remaining);
			text_layer_set_text(&textLayer, str);
			timer_handle_update = app_timer_send_event(ctx, 60000 /* milliseconds */, COOKIE_UPDATE);
		}
	}
}

void handle_init(AppContextRef ctx) {
  (void)ctx;

  	window_init(&window, "Power Nap App");
  	window_stack_push(&window, true /* Animated */);
	timer_handle = app_timer_send_event(ctx, 900000 /* milliseconds */, COOKIE_END);
	timer_handle_update = app_timer_send_event(ctx, 60000 /* milliseconds */, COOKIE_UPDATE);
	
  	text_layer_init(&textLayer, window.layer.frame);
	mini_snprintf(str, 21, "%d minutes remaining", time_remaining);
	text_layer_set_text(&textLayer, str);

  	text_layer_set_font(&textLayer, fonts_get_system_font(FONT_KEY_ROBOTO_CONDENSED_21));
	text_layer_set_text_alignment(&textLayer, GTextAlignmentCenter);
	
	layer_add_child(&window.layer, &textLayer.layer);
 	
}


void pbl_main(void *params) {
	PebbleAppHandlers handlers = {
    	.init_handler = &handle_init,
		.timer_handler = &handle_timer
    };
  	app_event_loop(params, &handlers);
}
