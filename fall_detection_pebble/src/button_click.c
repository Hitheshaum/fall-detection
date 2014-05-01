#include <pebble.h>
#include "window2.c"
#include "window3.c"
  
static Window *window2, *window3;
extern Window *window;
static TextLayer *text_date_layer;
static TextLayer *text_time_layer;
static Layer *line_layer;
static uint32_t accel_buffer[]={0,0,0,0,0,0,0,0};
static uint8_t position=0;
static uint8_t WIN=2;
static int count=0;
static uint32_t TRESHOLD= 25000000;


extern void click_config_provider2(void *context);
extern void window_load2(Window *window2);
extern void window_unload2(Window *window2);
extern void window_disappear2(Window *window2);
extern void window_appear2(Window *window2);

extern void click_config_provider3(void *context);
extern void window_load3(Window *window3);
extern void window_unload3(Window *window3);
extern void window_disappear3(Window *window3);
extern void window_appear3(Window *window3);

static void out_sent_handler(DictionaryIterator *sent, void *context) {
   // outgoing message was delivered
}

static void out_failed_handler(DictionaryIterator *failed, AppMessageResult reason, void *context) {
   // outgoing message failed
}

static void in_received_handler(DictionaryIterator *received, void *context) {
  Tuple *tuple = dict_find(received, 0);
  uint8_t multiplier= tuple->value->uint8;
  TRESHOLD= 20000000+(multiplier*100000);
  if(multiplier==25){
    APP_LOG(APP_LOG_LEVEL_DEBUG, "data received");
  }
}


static void in_dropped_handler(AppMessageResult reason, void *context) {
   // incoming message dropped
}

  
static void accel_data_handler(AccelData *data, uint32_t num_samples){
  //APP_LOG(APP_LOG_LEVEL_DEBUG, "acceleration read");
  accel_buffer[position]=(((data->x)*(data->x))+((data->y)*(data->y))+((data->z)*(data->z)));
  uint32_t minimum;
  uint32_t maximum;
  minimum=accel_buffer[0];
  maximum=accel_buffer[0];
  for ( uint8_t c = 1 ; c < WIN ; c++ ) {
    if ( accel_buffer[c] < minimum ) {
      minimum = accel_buffer[c];
    }
    if(accel_buffer[c]> maximum){
      maximum=accel_buffer[c];
    }
  }
  if((maximum-minimum)>TRESHOLD){
    //text_layer_set_text(text_layer, "Fall");
    APP_LOG(APP_LOG_LEVEL_DEBUG, "fall");
    /*DictionaryIterator *iter;
    app_message_outbox_begin(&iter);
    Tuplet value = TupletInteger(1, 42);
    dict_write_tuplet(iter, &value);
    app_message_outbox_send();*/
    window_stack_push(window2, true/*animated*/);
  }
  if((position++)==(WIN-1)){
    position=0;
  }
  
}

void line_layer_update_callback(Layer *layer, GContext* ctx) {
  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_fill_rect(ctx, layer_get_bounds(layer), 0, GCornerNone);
}

void handle_minute_tick(struct tm *tick_time, TimeUnits units_changed) {
  // Need to be static because they're used by the system later.
  static char time_text[] = "00:00";
  static char date_text[] = "Xxxxxxxxx 00";
  char *time_format;
  if (!tick_time) {
    time_t now = time(NULL);
    tick_time = localtime(&now);
  }
  // TODO: Only update the date when it's changed.
  strftime(date_text, sizeof(date_text), "%B %e", tick_time);
  text_layer_set_text(text_date_layer, date_text);
  if (clock_is_24h_style()) {
    time_format = "%R";
  } else {
    time_format = "%I:%M";
  }
  strftime(time_text, sizeof(time_text), time_format, tick_time);
  // Kludge to handle lack of non-padded hour format string
  // for twelve hour clock.
  if (!clock_is_24h_style() && (time_text[0] == '0')) {
    memmove(time_text, &time_text[1], sizeof(time_text) - 1);
  }
  text_layer_set_text(text_time_layer, time_text);
}



static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  //text_layer_set_text(text_layer, "Test");
  /*DictionaryIterator *iter;
  app_message_outbox_begin(&iter);
  Tuplet value = TupletInteger(1, 42);
  dict_write_tuplet(iter, &value);
  app_message_outbox_send();*/
  window_stack_push(window2, true/*animated*/);
}



static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  TRESHOLD +=100000;
  static char buf[] = "12";
  //snprintf(buf, sizeof(buf), "%d", ++count);
  //text_layer_set_text(text_layer, buf);
}



static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  TRESHOLD -=100000;
  static char buf[] = "12";
  //snprintf(buf, sizeof(buf), "%d", --count);
  //text_layer_set_text(text_layer, buf);
}


static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}



static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  /*text_layer = text_layer_create((GRect) { .origin = { 0, 72 }, .size = { bounds.size.w, 20 } });
  text_layer_set_text(text_layer, "Press a button");
  text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(text_layer));*/
  
  text_date_layer = text_layer_create(GRect(8, 68, 144-8, 168-68));
  text_layer_set_text_color(text_date_layer, GColorWhite);
  text_layer_set_background_color(text_date_layer, GColorClear);
  text_layer_set_font(text_date_layer, fonts_get_system_font(FONT_KEY_ROBOTO_CONDENSED_21));
  layer_add_child(window_layer, text_layer_get_layer(text_date_layer));

  text_time_layer = text_layer_create(GRect(7, 92, 144-7, 168-92));
  text_layer_set_text_color(text_time_layer, GColorWhite);
  text_layer_set_background_color(text_time_layer, GColorClear);
  text_layer_set_font(text_time_layer, fonts_get_system_font(FONT_KEY_ROBOTO_BOLD_SUBSET_49));
  layer_add_child(window_layer, text_layer_get_layer(text_time_layer));

  GRect line_frame = GRect(8, 97, 139, 2);
  line_layer = layer_create(line_frame);
  layer_set_update_proc(line_layer, line_layer_update_callback);
  layer_add_child(window_layer, line_layer);
  
  app_message_register_outbox_sent(out_sent_handler);
  app_message_register_outbox_failed(out_failed_handler);
  app_message_register_inbox_received(in_received_handler);
  app_message_register_inbox_dropped(in_dropped_handler);
  const uint32_t inbound_size = 64;
  const uint32_t outbound_size = 64;
  app_message_open(inbound_size, outbound_size);
  //APP_LOG(APP_LOG_LEVEL_DEBUG, "app message subscribed");
  
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);
  Tuplet value = TupletInteger(1, 12);
  dict_write_tuplet(iter, &value);
  app_message_outbox_send();
}



static void window_unload(Window *window) {
  //text_layer_destroy(text_layer);
  text_layer_destroy(text_time_layer);
  text_layer_destroy(text_date_layer);
  layer_destroy(line_layer);
  //window_stack_remove(window, true);
  //window_stack_push(window3, true);
}

static void window_appear(Window *window){
  accel_data_service_subscribe(1, &accel_data_handler);
  accel_service_set_sampling_rate(ACCEL_SAMPLING_100HZ);
  //APP_LOG(APP_LOG_LEVEL_DEBUG, "accel subscribed");
  tick_timer_service_subscribe(MINUTE_UNIT, handle_minute_tick);
  handle_minute_tick(NULL, MINUTE_UNIT);
}

static void window_disappear(Window *window){
  accel_data_service_unsubscribe();
  tick_timer_service_unsubscribe();
}

static void init(void) {
  window = window_create();
  window_set_click_config_provider(window, click_config_provider);
  window_set_window_handlers(window, (WindowHandlers) {
	  .load = window_load,
    .appear=window_appear,
    .disappear=window_disappear,
    .unload = window_unload,
  });
  window_set_fullscreen(window, true);
  
  window_set_background_color(window, GColorBlack);
  
  window2 = window_create();
  window_set_click_config_provider(window2, click_config_provider2);
  window_set_window_handlers(window2, (WindowHandlers) {
	  .load = window_load2,
    .appear=window_appear2,
    .disappear=window_disappear2,
    .unload = window_unload2,
  });
  
  window3 = window_create();
  window_set_click_config_provider(window3, click_config_provider3);
  window_set_window_handlers(window3, (WindowHandlers) {
	  .load = window_load3,
    .appear=window_appear3,
    .disappear=window_disappear3,
    .unload = window_unload3,
  });
  
  window_stack_push(window3, false/*animated*/);
  window_stack_push(window, true/*animated*/);
}

static void deinit(void) {
  window_destroy(window);
  window_destroy(window2);
  window_destroy(window3);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}