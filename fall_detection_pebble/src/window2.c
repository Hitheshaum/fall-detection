#include <pebble.h>
static int count_down=15;
static TextLayer *text_layer, *note_layer, *note_layer2;

static void select_click_handler2(ClickRecognizerRef recognizer, void *context);
static void up_click_handler2(ClickRecognizerRef recognizer, void *context);
static void down_click_handler2(ClickRecognizerRef recognizer, void *context);
static void count_down_handler(struct tm *tick_time, TimeUnits units_changed);



static void window_load2(Window *window2) {
  Layer *window_layer = window_get_root_layer(window2);
  GRect bounds = layer_get_bounds(window_layer);
  //text_layer = text_layer_create((GRect) { .origin = { 0, 72 }});
  text_layer = text_layer_create((GRect) { { 5, 0 }, { bounds.size.w - 2*5, bounds.size.h } });
  text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
  text_layer_set_font(text_layer, fonts_get_system_font(FONT_KEY_ROBOTO_BOLD_SUBSET_49));
  layer_add_child(window_layer, text_layer_get_layer(text_layer));
  text_layer_set_text(text_layer, "15");
  
  note_layer = text_layer_create((GRect) { .origin = { 0, 72 }, .size = { bounds.size.w, 28 } });
  text_layer_set_font(note_layer, fonts_get_system_font(FONT_KEY_ROBOTO_CONDENSED_21));
  text_layer_set_text(note_layer, "Press any button");
  text_layer_set_text_alignment(note_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(note_layer));
  
  note_layer2 = text_layer_create((GRect) { .origin = { 0, 100 }, .size = { bounds.size.w, 28 } });
  text_layer_set_font(note_layer2, fonts_get_system_font(FONT_KEY_ROBOTO_CONDENSED_21));
  text_layer_set_text(note_layer2, "to stop broadcast");
  text_layer_set_text_alignment(note_layer2, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(note_layer2));

}

static void count_down_handler(struct tm *tick_time, TimeUnits units_changed){
  static char buf[] = "12";
  snprintf(buf, sizeof(buf), "%d", count_down--);
  text_layer_set_text(text_layer, buf);
  if(count_down<5){
    vibes_short_pulse();
  }
  if(count_down==0){
    DictionaryIterator *iter;
    app_message_outbox_begin(&iter);
    Tuplet value = TupletInteger(1, 42);
    dict_write_tuplet(iter, &value);
    app_message_outbox_send();
    vibes_long_pulse();
    window_stack_pop(true);
  }
}

static void window_appear2(Window *window2){
  tick_timer_service_subscribe(SECOND_UNIT, count_down_handler);
  count_down_handler( NULL, SECOND_UNIT);
  count_down=15;
  vibes_long_pulse();
}

static void window_disappear2(Window *window2){
  tick_timer_service_unsubscribe();
}

static void window_unload2(Window *window2) {
  text_layer_destroy(text_layer);
  text_layer_destroy(note_layer);
  text_layer_destroy(note_layer2);
}

static void click_config_provider2(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler2);
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler2);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler2);
}

static void down_click_handler2(ClickRecognizerRef recognizer, void *context){
  window_stack_pop(true);
}

static void up_click_handler2(ClickRecognizerRef recognizer, void *context) {
  window_stack_pop(true);
}

static void select_click_handler2(ClickRecognizerRef recognizer, void *context){
  window_stack_pop(true);
}