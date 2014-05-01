#include <pebble.h>
static int count_down3=5;
static TextLayer *text_layer3, *note_layer, *note_layer2;
static Window *window;

static void select_click_handler3(ClickRecognizerRef recognizer, void *context);
static void up_click_handler3(ClickRecognizerRef recognizer, void *context);
static void down_click_handler3(ClickRecognizerRef recognizer, void *context);
static void count_down_handler3(struct tm *tick_time, TimeUnits units_changed);

static void window_load3(Window *window3) {
  Layer *window_layer = window_get_root_layer(window3);
  GRect bounds = layer_get_bounds(window_layer);
  //text_layer = text_layer_create((GRect) { .origin = { 0, 72 }});
  text_layer3 = text_layer_create((GRect) { { 5, 0 }, { bounds.size.w - 2*5, bounds.size.h } });
  text_layer_set_text_alignment(text_layer3, GTextAlignmentCenter);
  text_layer_set_font(text_layer3, fonts_get_system_font(FONT_KEY_ROBOTO_BOLD_SUBSET_49));
  layer_add_child(window_layer, text_layer_get_layer(text_layer3));
  text_layer_set_text(text_layer3, "10");
  
  note_layer = text_layer_create((GRect) { .origin = { 0, 72 }, .size = { bounds.size.w, 28 } });
  text_layer_set_font(note_layer, fonts_get_system_font(FONT_KEY_ROBOTO_CONDENSED_21));
  text_layer_set_text(note_layer, "Press back again");
  text_layer_set_text_alignment(note_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(note_layer));
  
  note_layer2 = text_layer_create((GRect) { .origin = { 0, 100 }, .size = { bounds.size.w, 28 } });
  text_layer_set_font(note_layer2, fonts_get_system_font(FONT_KEY_ROBOTO_CONDENSED_21));
  text_layer_set_text(note_layer2, "to exit");
  text_layer_set_text_alignment(note_layer2, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(note_layer2));

}

static void count_down_handler3(struct tm *tick_time, TimeUnits units_changed){
  static char buf[] = "12";
  snprintf(buf, sizeof(buf), "%d", count_down3--);
  text_layer_set_text(text_layer3, buf);
  if(count_down3==0){
    window_stack_push(window,true);
  }
}

static void window_appear3(Window *window3){
  count_down3=5;
  tick_timer_service_subscribe(SECOND_UNIT, count_down_handler3);
  count_down_handler3( NULL, SECOND_UNIT);
}

static void window_disappear3(Window *window3){
  tick_timer_service_unsubscribe();
}

static void window_unload3(Window *window3) {
  text_layer_destroy(text_layer3);
  text_layer_destroy(note_layer);
  text_layer_destroy(note_layer2);
}

static void click_config_provider3(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler3);
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler3);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler3);
}

static void down_click_handler3(ClickRecognizerRef recognizer, void *context){
  
}

static void up_click_handler3(ClickRecognizerRef recognizer, void *context) {
  
}

static void select_click_handler3(ClickRecognizerRef recognizer, void *context){
  window_stack_push(window,true);
}