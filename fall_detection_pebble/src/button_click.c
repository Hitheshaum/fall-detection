#include <pebble.h>

static Window *window;
static TextLayer *text_date_layer;
static TextLayer *text_time_layer;
static Layer *line_layer;
static uint32_t accel_buffer[]={0,0,0,0,0,0,0,0};
static uint8_t position=0;
static uint8_t WIN=2;
static int count=0;
static uint32_t TRESHOLD= 25000000;;

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


void out_sent_handler(DictionaryIterator *sent, void *context) {
    // outgoing message was delivered
}

void out_failed_handler(DictionaryIterator *failed, AppMessageResult reason, void *context) {
    // outgoing message failed
}


static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
    //text_layer_set_text(text_layer, "Test");
    DictionaryIterator *iter;
    app_message_outbox_begin(&iter);
    Tuplet value = TupletInteger(1, 42);
    dict_write_tuplet(iter, &value);
    app_message_outbox_send();
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
    
    tick_timer_service_subscribe(MINUTE_UNIT, handle_minute_tick);
    handle_minute_tick(NULL, MINUTE_UNIT);
    
    app_message_register_outbox_sent(out_sent_handler);
    app_message_register_outbox_failed(out_failed_handler);
    const uint32_t inbound_size = 64;
    const uint32_t outbound_size = 64;
    app_message_open(inbound_size, outbound_size);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "app message subscribed");
}

static void window_unload(Window *window) {
    //text_layer_destroy(text_layer);
    text_layer_destroy(text_time_layer);
    text_layer_destroy(text_date_layer);
    layer_destroy(line_layer);
    accel_data_service_unsubscribe();
    tick_timer_service_unsubscribe();
}

static void init(void) {
    window = window_create();
    window_set_click_config_provider(window, click_config_provider);
    window_set_window_handlers(window, (WindowHandlers) {
        .load = window_load,
        .unload = window_unload,
    });
    window_set_fullscreen(window, true);
    const bool animated = true;
    window_stack_push(window, animated);
    window_set_background_color(window, GColorBlack);
    
    accel_data_service_subscribe(1, &accel_data_handler);
    accel_service_set_sampling_rate(ACCEL_SAMPLING_100HZ);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "accel subscribed");
}

static void deinit(void) {
    window_destroy(window);
}

int main(void) {
    init();
    app_event_loop();
    deinit();
}