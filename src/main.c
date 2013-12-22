#include <pebble.h>
#include "pebble_fonts.h"

enum KEYS
{
	NULL_KEY = 0x0,		// NOT USED
    DOWN_KEY = 0x1,		// TUPLE_INT
    UP_KEY = 0x2,		// TUPLE_INT
	SELECT_KEY = 0x3,	// TUPLE_INT
    CONTEXT_KEY = 0x4,	// TUPLE_INT
    LEFT_KEY = 0x5,		// TUPLE_INT
    RIGHT_KEY = 0x6,	// TUPLE_INT
	OUTPUT_TEXT = 0x7	// TUPLE_STRING
};

const bool animated = true;
	
static Window *window;
static TextLayer *text_layer;
static TextLayer *result_layer;
static TextLayer *javascript_result_layer;

void send_message(enum KEYS symbol)
{
	DictionaryIterator *iter;
	app_message_outbox_begin(&iter);
	if (iter == NULL) {
		text_layer_set_text(javascript_result_layer, "No buffer!");
		return;
	}
	
	Tuplet value = TupletInteger(symbol, symbol); // ok this is a little derpy, but it works I hope
	dict_write_tuplet(iter, &value);
	dict_write_end(iter);
	app_message_outbox_send();
}

void up_single_click_handler(ClickRecognizerRef recognizer, void *context)
{
	text_layer_set_text(text_layer, "Up!");
	send_message(UP_KEY);
}

void down_single_click_handler(ClickRecognizerRef recognizer, void *context)
{
	text_layer_set_text(text_layer, "Down!");
	send_message(DOWN_KEY);
}

void select_single_click_handler(ClickRecognizerRef recognizer, void *context)
{
	text_layer_set_text(text_layer, "Select!");
	send_message(SELECT_KEY);
}

void select_long_click_handler(ClickRecognizerRef recognizer, void *context)
{
	text_layer_set_text(text_layer, "Long!");
	send_message(CONTEXT_KEY);
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
	window_single_repeating_click_subscribe(BUTTON_ID_UP, (uint16_t) 30, up_single_click_handler);
	window_single_click_subscribe(BUTTON_ID_SELECT, select_single_click_handler);
	//window_long_click_subscribe(BUTTON_ID_SELECT, 100, select_long_click_handler, select_long_click_release_handler);
	window_single_repeating_click_subscribe(BUTTON_ID_DOWN, (uint16_t) 30, down_single_click_handler);
}



// AppMessage handlers

void out_sent_handler(DictionaryIterator *sent, void *context)
{
	// outgoing message was delivered
	text_layer_set_text(result_layer, "Success!");
}

char *get_failure_string(AppMessageResult reason)
{
	switch (reason) {
		case (APP_MSG_OK):
			return "Failed: APP_MSG_OK. Wait. What the fuck.";
			;;
		case (APP_MSG_SEND_TIMEOUT):
			return "Failed: APP_MSG_SEND_TIMEOUT";
			;;
		case (APP_MSG_SEND_REJECTED):
			return "Failed: APP_MSG_SEND_REJECTED";
			;;
		case (APP_MSG_NOT_CONNECTED):
			return "Failed: APP_MSG_NOT_CONNECTED";
			;;
		case (APP_MSG_APP_NOT_RUNNING):
			return "Failed: APP_MSG_APP_NOT_RUNNING";
			;;
		case (APP_MSG_INVALID_ARGS):
			return "Failed: APP_MSG_INVALID_ARGS";
			;;
		case (APP_MSG_BUSY):
			return"Failed: APP_MSG_BUSY";
			;;
		case (APP_MSG_BUFFER_OVERFLOW):
			return "Failed: APP_MSG_BUFFER_OVERFLOW";
			;;
		case (APP_MSG_ALREADY_RELEASED):
			return "Failed: APP_MSG_ALREADY_RELEASED";
			;;
		case (APP_MSG_CALLBACK_ALREADY_REGISTERED):
			return "Failed: APP_MSG_CALLBACK_ALREADY_REGISTERED";
			;;
		case (APP_MSG_CALLBACK_NOT_REGISTERED):
			return "Failed: APP_MSG_CALLBACK_NOT_REGISTERED";
			;;
		case (APP_MSG_OUT_OF_MEMORY):
			return "Failed: APP_MSG_OUT_OF_MEMORY";
			;;
		default:
			return "Failed: unknown reason.";
			;;
	}
}

void out_failed_handler(DictionaryIterator *failed, AppMessageResult reason, void *context)
{
	// outgoing message failed
	text_layer_set_text(result_layer, get_failure_string(reason));
}

void in_received_handler(DictionaryIterator *received, void *context)
{
	// incoming message received
	Tuple *output_tuple = dict_find(received, OUTPUT_TEXT);
	if (output_tuple) {
		char *output = output_tuple->value->cstring;
		text_layer_set_text(javascript_result_layer, output);
	} else {
		text_layer_set_text(javascript_result_layer, "ERROR: invalid message recieved.");
	}
}

void in_dropped_handler(AppMessageResult reason, void *context)
{
	// incoming message dropped
	text_layer_set_text(javascript_result_layer, get_failure_string(reason));
}

// Standard app initialisation

void handle_init()
{
	window = window_create();
	window_stack_push(window, animated);
	
	Layer *window_layer = window_get_root_layer(window);
	GRect bounds = layer_get_frame(window_layer);

	text_layer = text_layer_create(
		(GRect) { .origin = {2,20}, .size = {bounds.size.w-4,20} }
	);
	text_layer_set_text_alignment(text_layer, GTextAlignmentLeft);
	text_layer_set_text(text_layer, "Remote");
	//text_layer_set_font(text_layer, fonts_get_system_font(FONT_KEY_GOTHAM_30_BLACK));
	
	result_layer = text_layer_create(
		(GRect) { .origin = {2,45}, .size = {bounds.size.w-4,40} }
	);
	text_layer_set_text_alignment(result_layer, GTextAlignmentLeft);
	text_layer_set_text(result_layer, "AppMessage local output goes here.");	
	
	javascript_result_layer = text_layer_create(
		(GRect) { .origin = {2,90}, .size = {bounds.size.w-4,40} }
	);
	text_layer_set_text_alignment(javascript_result_layer, GTextAlignmentLeft);
	text_layer_set_text(javascript_result_layer, "AppMessage Remote Output goes here.");
	
	layer_add_child(window_layer, text_layer_get_layer(text_layer));
	layer_add_child(window_layer, text_layer_get_layer(result_layer));
	layer_add_child(window_layer, text_layer_get_layer(javascript_result_layer));
	
	// Attach our desired button functionality
	window_set_click_config_provider(window, click_config_provider);
	
	app_message_register_inbox_received(in_received_handler);
	app_message_register_inbox_dropped(in_dropped_handler);
	app_message_register_outbox_sent(out_sent_handler);
	app_message_register_outbox_failed(out_failed_handler);
	const uint32_t inbound_size = 64;
	const uint32_t outbound_size = 64;
	app_message_open(inbound_size, outbound_size);
	
	//app_message_init(); //not documented...
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
