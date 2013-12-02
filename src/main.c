#include <pebble.h>
#include "pebble_fonts.h"

const bool animated = true;
	
static Window *window;
static TextLayer *text_layer;

void up_single_click_handler(ClickRecognizerRef recognizer, void *context)
{
	text_layer_set_text(text_layer, "Up!");
}

void down_single_click_handler(ClickRecognizerRef recognizer, void *context)
{
	text_layer_set_text(text_layer, "Down!");
}

void select_single_click_handler(ClickRecognizerRef recognizer, void *context)
{
	text_layer_set_text(text_layer, "Select!");
}

void select_long_click_handler(ClickRecognizerRef recognizer, void *context)
{
	text_layer_set_text(text_layer, "Long!");
}

void select_long_click_release_handler(ClickRecognizerRef recognizer, void *context)
{
	text_layer_set_text(text_layer, "Released!");
}

void click_config_provider(void *context)
{
	window_set_click_context(BUTTON_ID_UP, context);
	window_set_click_context(BUTTON_ID_SELECT, context);
	window_set_click_context(BUTTON_ID_DOWN, context);
	window_single_click_subscribe(BUTTON_ID_UP, up_single_click_handler);
	window_single_click_subscribe(BUTTON_ID_SELECT, select_single_click_handler);
	//window_long_click_subscribe(BUTTON_ID_SELECT, 100, select_long_click_handler, select_long_click_release_handler);
	window_single_click_subscribe(BUTTON_ID_DOWN, down_single_click_handler);
}

// Standard app initialisation

void handle_init()
{
	window = window_create();
	window_stack_push(window, animated);
	
	Layer *window_layer = window_get_root_layer(window);
	GRect bounds = layer_get_frame(window_layer);

	text_layer = text_layer_create(bounds);
	text_layer_set_text(text_layer, "Remote");
	//text_layer_set_font(text_layer, fonts_get_system_font(FONT_KEY_GOTHAM_30_BLACK));

	layer_add_child(window_layer, text_layer_get_layer(text_layer));

	// Attach our desired button functionality
	window_set_click_config_provider(window, click_config_provider);
}

void handle_deinit()
{
	text_layer_destroy(text_layer);
	window_destroy(window);
}

int main(void) {
	handle_init();
	app_event_loop();
	handle_deinit();
}
