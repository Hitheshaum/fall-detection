#include <pebble.h>

static Window *window;
static TextLayer *text_layer;
static uint32_t accel_buffer[]={0,0,0,0,0,0,0,0};
static uint8_t position=0;
static uint8_t WIN=2;
static uint8_t count=0;
static uint32_t TRESHOLD=11000000;

void out_sent_handler(DictionaryIterator *sent, void *context) {
   // outgoing message was delivered
 }


 void out_failed_handler(DictionaryIterator *failed, AppMessageResult reason, void *context) {
   // outgoing message failed
 }
static void accel_data_handler(AccelData *data, uint32_t num_samples){
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
    text_layer_set_text(text_layer, "Fall");
    DictionaryIterator *iter;
    app_message_outbox_begin(&iter);
    Tuplet value = TupletInteger(1, 42);
    dict_write_tuplet(iter, &value);
    app_message_outbox_send();
  }
  if((position++)==(WIN-1)){
    position=0;
  }
  
}
static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_text(text_layer, "Test");
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);
  Tuplet value = TupletInteger(1, 42);
  dict_write_tuplet(iter, &value);
  app_message_outbox_send();
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  TRESHOLD +=100000;
  static char buf[] = "12";
  snprintf(buf, sizeof(buf), "%d", ++count);
  text_layer_set_text(text_layer, buf);
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  TRESHOLD -=100000;
  static char buf[] = "12";
  snprintf(buf, sizeof(buf), "%d", --count);
  text_layer_set_text(text_layer, buf);
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  text_layer = text_layer_create((GRect) { .origin = { 0, 72 }, .size = { bounds.size.w, 20 } });
  text_layer_set_text(text_layer, "Press a button");
  text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(text_layer));
  
  accel_data_service_subscribe(1, &accel_data_handler);
  accel_service_set_sampling_rate(ACCEL_SAMPLING_100HZ);
  
  app_message_register_outbox_sent(out_sent_handler);
  app_message_register_outbox_failed(out_failed_handler);
  const uint32_t inbound_size = 64;
  const uint32_t outbound_size = 64;
  app_message_open(inbound_size, outbound_size);
}

static void window_unload(Window *window) {
  text_layer_destroy(text_layer);
  accel_data_service_unsubscribe();
}

static void init(void) {
  window = window_create();
  window_set_click_config_provider(window, click_config_provider);
  window_set_window_handlers(window, (WindowHandlers) {
	.load = window_load,
    .unload = window_unload,
  });
  const bool animated = true;
  window_stack_push(window, animated);
}

static void deinit(void) {
  window_destroy(window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}