#include <pebble.h>

#include <stdlib.h>
#include<math.h>

static Window *s_main_window;
static Layer *window_layer;
static TextLayer *s_time_layer;

static BitmapLayer *s_background_layer;

static GBitmap *s_background_bitmap;
static GBitmap *s_background2_bitmap;

static void main_window_load(Window *window){
  window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  
  // Create GBitmap
  s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_CHANX);
  s_background2_bitmap = gbitmap_create_with_resource(RESOURCE_ID_CHANX_EYECLOSE);
  
  // Create BitmapLayer to display the GBitmap
  s_background_layer = bitmap_layer_create(bounds);
  // Set the bitmap onto the layer and add to the window
  bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);
  layer_add_child(window_layer, bitmap_layer_get_layer(s_background_layer));
  
  
  s_time_layer = text_layer_create(GRect(0,10,bounds.size.w,50));
                                   
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorBlack);
  text_layer_set_text(s_time_layer, "00:00");
  text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  
  layer_add_child(window_layer, text_layer_get_layer(s_time_layer));
}

static void main_window_unload(Window *window){
  text_layer_destroy(s_time_layer);
  bitmap_layer_destroy(s_background_layer);
}

static void update_time(){
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);
  
  static char s_buffer[8];
  strftime(s_buffer, sizeof(s_buffer), clock_is_24h_style() ? "%H:%M":"%I:%M", tick_time);
  
  text_layer_set_text(s_time_layer, s_buffer);
}

bool brink;
AppTimer *timer;

void timer_callback(){
  bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);
}

static void fbrink(void){
   if(rand() % 10 < 2){
    bitmap_layer_set_bitmap(s_background_layer, s_background2_bitmap);
    timer = app_timer_register((rand() % 150) + 10, timer_callback, NULL);
  }else{
    bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);
  }
}

static void tick_handler(struct tm *tick_time,TimeUnits units_changed){
  update_time();
  fbrink();
}


void handle_init(void) {
  s_main_window = window_create();
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  srand(time(NULL));
  //tick_timer_service_subscribe(SECOND_UNIT, brink_handler);
  tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
    
  window_stack_push(s_main_window, true);
  window_set_background_color(s_main_window, GColorWhite);
  update_time();
}

void handle_deinit(void) {
  window_destroy(s_main_window);
}

int main(void) {
  handle_init();
  app_event_loop();
  handle_deinit();
}
